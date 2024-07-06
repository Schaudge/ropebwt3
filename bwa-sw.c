#include <string.h>
#include <stdio.h>
#include "rb3priv.h"
#include "libsais16.h"
#include "io.h" // for rb3_nt6_table[]
#include "fm-index.h"
#include "kalloc.h"
#include "khashl-km.h"
KHASHL_MAP_INIT(KH_LOCAL, sw_hash_t, sw_hash, uint64_t, uint64_t, kh_hash_uint64, kh_eq_generic)

/*******************
 * Lightweight BWT *
 *******************/

static uint32_t bwtl_cnt_table[256];

typedef struct {
	void *km;
	int32_t seq_len, bwt_size, n_occ;
	int32_t primary;
	int32_t *occ, *sa, L2[5];
	uint32_t *bwt;
} bwtl_t;

#define bwtl_B0(b, k) ((b)->bwt[(k)>>4]>>((~(k)&0xf)<<1)&3)

void bwtl_init_cnt_table(void)
{
	int i, c;
	for (i = 0; i != 256; ++i) {
		uint32_t x = 0;
		for (c = 0; c != 4; ++c)
			x |= (((i&3) == c) + ((i>>2&3) == c) + ((i>>4&3) == c) + (i>>6 == c)) << (c<<3);
		bwtl_cnt_table[i] = x;
	}
}

bwtl_t *bwtl_gen(void *km, int len, const uint8_t *seq)
{
	bwtl_t *b;
	int32_t i;
	b = Kcalloc(km, bwtl_t, 1);
	b->km = km;
	b->seq_len = len;

	{ // calculate b->bwt
		uint8_t *s;
		uint16_t *s16;
		s16 = Kcalloc(km, uint16_t, len);
		for (i = 0; i < len; ++i) s16[i] = rb3_nt6_table[seq[i]];
		b->sa = Kcalloc(km, int32_t, len + 1);
		libsais16(s16, &b->sa[1], len, 0, 0);
		b->sa[0] = len;
		s = Kcalloc(km, uint8_t, len + 1);
		for (i = 0; i <= len; ++i) {
			if (b->sa[i] == 0) b->primary = i;
			else s[i] = s16[b->sa[i] - 1] - 1;
		}
		kfree(km, s16);
		for (i = b->primary; i < len; ++i) s[i] = s[i + 1];
		b->bwt_size = (len + 15) / 16;
		b->bwt = Kcalloc(km, uint32_t, b->bwt_size);
		for (i = 0; i < len; ++i)
			b->bwt[i>>4] |= s[i] << ((15 - (i&15)) << 1);
		kfree(km, s);
	}
	{ // calculate b->occ
		int32_t c[4];
		b->n_occ = (len + 15) / 16 * 4;
		b->occ = Kcalloc(km, int32_t, b->n_occ);
		memset(c, 0, 16);
		for (i = 0; i < len; ++i) {
			if (i % 16 == 0)
				memcpy(b->occ + (i/16) * 4, c, 16);
			++c[bwtl_B0(b, i)];
		}
		memcpy(b->L2+1, c, 16);
		b->L2[0] = 1;
		for (i = 1; i < 5; ++i) b->L2[i] += b->L2[i-1];
	}
	return b;
}

void bwtl_rank1a(const bwtl_t *bwt, int32_t k, int32_t cnt[4])
{
	uint32_t x, b;
	if (k > bwt->primary) --k; // because $ is not in bwt
	memcpy(cnt, bwt->occ + (k>>4<<2), 16);
	if (k % 16 == 0) return;
	--k;
	b = bwt->bwt[k>>4] & ~((1U<<((~k&15)<<1)) - 1);
	x = bwtl_cnt_table[b&0xff] + bwtl_cnt_table[b>>8&0xff] + bwtl_cnt_table[b>>16&0xff] + bwtl_cnt_table[b>>24];
	x -= 15 - (k&15);
	cnt[0] += x&0xff; cnt[1] += x>>8&0xff; cnt[2] += x>>16&0xff; cnt[3] += x>>24;
}

void bwtl_rank2a(const bwtl_t *bwt, int32_t k, int32_t l, int32_t cntk[4], int32_t cntl[4])
{
	bwtl_rank1a(bwt, k, cntk);
	bwtl_rank1a(bwt, l, cntl);
}

void bwtl_destroy(bwtl_t *bwt)
{
	kfree(bwt->km, bwt->occ);
	kfree(bwt->km, bwt->bwt);
	kfree(bwt->km, bwt->sa);
	kfree(bwt->km, bwt);
}

/**********
 * BWA-SW *
 **********/

sw_hash_t *rb3_sw_count_pre(void *km, const bwtl_t *bwt)
{
	sw_hash_t *h;
	uint32_t n = 0, m = 16;
	uint64_t *a;
	khint_t itr;

	h = sw_hash_init2(km);
	a = Kmalloc(km, uint64_t, m);
	a[n++] = bwt->seq_len + 1;
	while (n > 0) {
		uint64_t x = a[--n];
		int32_t k = x>>32, l = (int32_t)x;
		int32_t rk[4], rl[4];
		int c, absent;
		bwtl_rank2a(bwt, k, l, rk, rl);
		//fprintf(stderr, "[%d,%d)\n", k, l);
		for (c = 3; c >= 0; --c) {
			uint64_t y;
			k = bwt->L2[c] + rk[c];
			l = bwt->L2[c] + rl[c];
			if (k == l) continue;
			y = (uint64_t)k << 32 | l;
			itr = sw_hash_put(h, y, &absent);
			//fprintf(stderr, "- c=%c, [%d,%d): %d\n", "ACGTN"[c], k, l, absent);
			if (absent) {
				kh_val(h, itr) = 1ULL<<32;
				Kgrow(km, uint64_t, a, n, m);
				a[n++] = y;
			} else kh_val(h, itr) += 1ULL<<32;
		}
	}
	kfree(km, a);
	return h;
}

void rb3_sw_hash_print(const sw_hash_t *h)
{
	khint_t k;
	kh_foreach(h, k) {
		fprintf(stderr, "[%d,%d): %d\n", (int32_t)(kh_key(h, k)>>32), (int32_t)kh_key(h, k), (int32_t)(kh_val(h, k)>>32));
	}
}

void rb3_bwa_sw(void *km, const rb3_fmi_t *f, int len, const uint8_t *seq)
{
	bwtl_t *q;
	sw_hash_t *h;
	q = bwtl_gen(km, len, seq);
	//h = rb3_sw_count_pre(km, q);
	rb3_sw_hash_print(h);
	sw_hash_destroy(h);
	bwtl_destroy(q);
}

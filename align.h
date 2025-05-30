#ifndef RB3_ALIGN_H
#define RB3_ALIGN_H

#include "fm-index.h"

#define RB3_SWF_E2E      0x1
#define RB3_SWF_HAPDIV   0x2
#define RB3_SWF_KEEP_RS  0x4
#define RB3_SWF_MAX_HIS  0x8
#define RB3_SWF_SPEC     0x10

typedef struct {
	uint32_t flag;
	int32_t n_best;
	int32_t min_sc, max_hc, end_len, min_mem_len;
	int32_t match, mis;
	int32_t e2e_drop;
	int32_t gap_open, gap_ext;
	int32_t r2cache_size;
} rb3_swopt_t;

typedef struct {
	int32_t score;
	int32_t qlen, rlen;
	int32_t n_cigar, cs_len;
	int32_t n_qoff, blen, mlen;
	int64_t lo, hi; // SA interval
	int64_t sid, pos; // contig ID and position of a semi-random hit in [lo,hi)
	uint8_t *rseq; // reference sequence in the alignment
	uint32_t *cigar; // cigar in the BAM encoding
	int32_t *qoff; // list of query offsets for the same hit
    uint32_t *rhc; // all ref hit counts (respect to rhs)
	char *cs; // different sequence for alignment
    char *rhs; // all ref hit names/ids
} rb3_swhit_t;

typedef struct {
	int32_t n;
	rb3_swhit_t *a;
} rb3_swrst_t;

#define RB2_SW_MAX_ED 6

typedef struct {
	int32_t n_al, max_ed, n_hap[RB2_SW_MAX_ED + 1];
} rb3_hapdiv_t;

void rb3_swopt_init(rb3_swopt_t *opt);
void rb3_sw(void *km, rb3_sai_v *mem, const rb3_swopt_t *opt, const rb3_fmi_t *f, int len, const uint8_t *seq, rb3_swrst_t *rst);
void rb3_hapdiv(void *km, const rb3_swopt_t *opt, const rb3_fmi_t *f, int len, const uint8_t *seq, rb3_hapdiv_t *hd);
void rb3_swrst_free(rb3_swrst_t *rst);

#endif

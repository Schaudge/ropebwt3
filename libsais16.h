/*--

This file is a part of libsais, a library for linear time suffix array,
longest common prefix array and burrows wheeler transform construction.

   Copyright (c) 2021-2024 Ilya Grebnov <ilya.grebnov@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

Please see the file LICENSE for full copyright information.

--*/

#ifndef LIBSAIS16_H
#define LIBSAIS16_H 1

#define LIBSAIS16_VERSION_MAJOR   2
#define LIBSAIS16_VERSION_MINOR   8
#define LIBSAIS16_VERSION_PATCH   4
#define LIBSAIS16_VERSION_STRING  "2.8.4"

#ifdef _WIN32
    #ifdef LIBSAIS_SHARED
        #ifdef LIBSAIS_EXPORTS
            #define LIBSAIS16_API __declspec(dllexport)
        #else
            #define LIBSAIS16_API __declspec(dllimport)
        #endif
    #else
        #define LIBSAIS16_API
    #endif
#else
    #define LIBSAIS16_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdint.h>

    /**
    * Creates the libsais16 context that allows reusing allocated memory with each libsais16 operation. 
    * In multi-threaded environments, use one context per thread for parallel executions.
    * @return the libsais16 context, NULL otherwise.
    */
    LIBSAIS16_API void * libsais16_create_ctx(void);

#if defined(LIBSAIS_OPENMP)
    /**
    * Creates the libsais16 context that allows reusing allocated memory with each parallel libsais16 operation using OpenMP. 
    * In multi-threaded environments, use one context per thread for parallel executions.
    * @param threads The number of OpenMP threads to use (can be 0 for OpenMP default).
    * @return the libsais16 context, NULL otherwise.
    */
    LIBSAIS16_API void * libsais16_create_ctx_omp(int32_t threads);
#endif

    /**
    * Destroys the libsass context and free previusly allocated memory.
    * @param ctx The libsais16 context (can be NULL).
    */
    LIBSAIS16_API void libsais16_free_ctx(void * ctx);

    /**
    * Constructs the suffix array of a given 16-bit string.
    * @param T [0..n-1] The input 16-bit string.
    * @param SA [0..n-1+fs] The output array of suffixes.
    * @param n The length of the given 16-bit string.
    * @param fs The extra space available at the end of SA array (0 should be enough for most cases).
    * @param freq [0..65535] The output 16-bit symbol frequency table (can be NULL).
    * @return 0 if no error occurred, -1 or -2 otherwise.
    */
    LIBSAIS16_API int32_t libsais16(const uint16_t * T, int32_t * SA, int32_t n, int32_t fs, int32_t * freq);

    /**
    * Constructs the suffix array of a given integer array.
    * Note, during construction input array will be modified, but restored at the end if no errors occurred.
    * @param T [0..n-1] The input integer array.
    * @param SA [0..n-1+fs] The output array of suffixes.
    * @param n The length of the integer array.
    * @param k The alphabet size of the input integer array.
    * @param fs Extra space available at the end of SA array (can be 0, but 4k or better 6k is recommended for optimal performance).
    * @return 0 if no error occurred, -1 or -2 otherwise.
    */
    LIBSAIS16_API int32_t libsais16_int(int32_t * T, int32_t * SA, int32_t n, int32_t k, int32_t fs);

    /**
    * Constructs the suffix array of a given 16-bit string using libsais16 context.
    * @param ctx The libsais16 context.
    * @param T [0..n-1] The input 16-bit string.
    * @param SA [0..n-1+fs] The output array of suffixes.
    * @param n The length of the given 16-bit string.
    * @param fs The extra space available at the end of SA array (0 should be enough for most cases).
    * @param freq [0..65535] The output 16-bit symbol frequency table (can be NULL).
    * @return 0 if no error occurred, -1 or -2 otherwise.
    */
    LIBSAIS16_API int32_t libsais16_ctx(const void * ctx, const uint16_t * T, int32_t * SA, int32_t n, int32_t fs, int32_t * freq);

#if defined(LIBSAIS_OPENMP)
    /**
    * Constructs the suffix array of a given 16-bit string in parallel using OpenMP.
    * @param T [0..n-1] The input 16-bit string.
    * @param SA [0..n-1+fs] The output array of suffixes.
    * @param n The length of the given 16-bit string.
    * @param fs The extra space available at the end of SA array (0 should be enough for most cases).
    * @param freq [0..65535] The output 16-bit symbol frequency table (can be NULL).
    * @param threads The number of OpenMP threads to use (can be 0 for OpenMP default).
    * @return 0 if no error occurred, -1 or -2 otherwise.
    */
    LIBSAIS16_API int32_t libsais16_omp(const uint16_t * T, int32_t * SA, int32_t n, int32_t fs, int32_t * freq, int32_t threads);

    /**
    * Constructs the suffix array of a given integer array in parallel using OpenMP.
    * Note, during construction input array will be modified, but restored at the end if no errors occurred.
    * @param T [0..n-1] The input integer array.
    * @param SA [0..n-1+fs] The output array of suffixes.
    * @param n The length of the integer array.
    * @param k The alphabet size of the input integer array.
    * @param fs Extra space available at the end of SA array (can be 0, but 4k or better 6k is recommended for optimal performance).
    * @param threads The number of OpenMP threads to use (can be 0 for OpenMP default).
    * @return 0 if no error occurred, -1 or -2 otherwise.
    */
    LIBSAIS16_API int32_t libsais16_int_omp(int32_t * T, int32_t * SA, int32_t n, int32_t k, int32_t fs, int32_t threads);
#endif

#ifdef __cplusplus
}
#endif

#endif

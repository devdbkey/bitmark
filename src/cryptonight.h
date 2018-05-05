#ifndef __CRYPTONIGHT_H_INCLUDED
#define __CRYPTONIGHT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "cryptonight/oaes_lib.h"
//#include "miner.h"
#include "cryptonight/compat.h"

#define MEMORY         (1 << 21) /* 2 MiB */
#define ITER           (1 << 20)
#define AES_BLOCK_SIZE  16
#define AES_KEY_SIZE    32 /*16*/
#define INIT_SIZE_BLK   8
#define INIT_SIZE_BYTE (INIT_SIZE_BLK * AES_BLOCK_SIZE)	// 128
#define USE_LOBOTOMIZED_AES

#pragma pack(push, 1)
union hash_state {
  uint8_t b[200];
  uint64_t w[25];
};
#pragma pack(pop)

#pragma pack(push, 1)
union cn_slow_hash_state {
    union hash_state hs;
    struct {
        uint8_t k[64];
        uint8_t init[INIT_SIZE_BYTE];
    };
};
#pragma pack(pop)

#ifdef USE_LOBOTOMIZED_AES

struct cryptonight_ctx {
    uint8_t long_state[MEMORY] __attribute((aligned(16)));
    union cn_slow_hash_state state;
    uint8_t text[INIT_SIZE_BYTE] __attribute((aligned(16)));
    uint8_t a[AES_BLOCK_SIZE] __attribute__((aligned(16)));
    uint8_t b[AES_BLOCK_SIZE] __attribute__((aligned(16)));
    uint8_t c[AES_BLOCK_SIZE] __attribute__((aligned(16)));
    oaes_ctx* aes_ctx;
};

#else

struct cryptonight_ctx {
    uint8_t long_state[MEMORY] __attribute((aligned(16)));
    union cn_slow_hash_state state;
    uint8_t text[INIT_SIZE_BYTE] __attribute((aligned(16)));
    uint64_t a[AES_BLOCK_SIZE >> 3] __attribute__((aligned(16)));
    uint64_t b[AES_BLOCK_SIZE >> 3] __attribute__((aligned(16)));
    uint8_t c[AES_BLOCK_SIZE] __attribute__((aligned(16)));
    oaes_ctx* aes_ctx;
};

#endif

void do_blake_hash(const void* input, size_t len, char* output);
void do_groestl_hash(const void* input, size_t len, char* output);
void do_jh_hash(const void* input, size_t len, char* output);
void do_skein_hash(const void* input, size_t len, char* output);
void xor_blocks_dst(const uint8_t * a, const uint8_t * b, uint8_t * dst);
int cryptonight_hash_ctx(void* output, const void* input, int inlen, struct cryptonight_ctx* ctx, int variant);
int cryptonight_hash(void* output, const void* input, size_t len);

int keccak(const uint8_t *in, int inlen, uint8_t *md, int mdlen);
void keccakf(uint64_t st[25], int rounds);
extern void (* const extra_hashes[4])(const void *, size_t, char *);

#define VARIANT1_1(p) \
  do if (variant > 0) \
  { \
    const uint8_t tmp = ((const uint8_t*)(p))[11]; \
    static const uint32_t table = 0x75310; \
    const uint8_t index = (((tmp >> 3) & 6) | (tmp & 1)) << 1; \
    ((uint8_t*)(p))[11] = tmp ^ ((table >> index) & 0x30); \
  } while(0)

#define VARIANT1_INIT() \
  if (variant > 0 && inlen < 43) \
  { \
    return 0; \
  } \
  const uint64_t tweak1_2 = variant > 0 ? *((const uint64_t*) (((const uint8_t*)input) + 35)) ^ ctx->state.hs.w[24] : 0

#ifdef __cplusplus
}
#endif
  
#endif
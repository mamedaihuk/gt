#ifndef _LZSS_H_
#define _LZSS_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define LZSS_MAX_SIZE ((1 << 24) - 1)
#define LZSS_BUFFER_SIZE (1 << 25)
#define LZSS_TYPE_10 0x10
#define LZSS_TYPE_11 0x11

extern ssize_t lzss_decompress(const uint8_t* in, size_t in_size, uint8_t* out, size_t out_size);
extern ssize_t lzss_compress(const uint8_t* in, size_t in_size, uint8_t* out, size_t out_size, int type);

#endif

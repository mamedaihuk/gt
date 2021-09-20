#include "lzss.h"
#include <stdbool.h>
#include <string.h>

static bool decompress_lzss_10(const uint8_t* in, size_t in_size, uint8_t* out, size_t out_size) {
    //const uint8_t* is = in;
    const uint8_t* ip = in;
    const uint8_t* ie = in + in_size;
    uint8_t* os = out;
    uint8_t* op = out;
    uint8_t* oe = out + out_size;

    while(op < oe) {
        int byte;

        if((ie - ip) < 1)
            return false;

        byte = *ip++;

        for(int i = 7; i >= 0; i--) {
            int flag = (byte >> i) & 1;

            if(!flag) {
                if((ie - ip) < 1 || (oe - op) < 1)
                    return false;

                *op++ = *ip++;
            } else {
                int v;
                int count;
                int disp;

                if((ie - ip) < 2)
                    return false;

                v = (ip[0] << 8) | ip[1];
                ip += 2;

                count = 3 + ((v & 0xf000) >> 12);
                disp = 1 + (v & 0x0fff);

                if((oe - op) < count)
                    return false;

                if((op - os) < disp)
                    return false;

                for(int j = 0; j < count; j++)
                    op[j] = op[j - disp];

                op += count;
            }

            if(op == oe)
                break;
        }
    }

    return (op == oe);
}

static bool decompress_lzss_11(const uint8_t* in, size_t in_size, uint8_t* out, size_t out_size) {
    //const uint8_t* is = in;
    const uint8_t* ip = in;
    const uint8_t* ie = in + in_size;
    uint8_t* os = out;
    uint8_t* op = out;
    uint8_t* oe = out + out_size;

    while(op < oe) {
        int byte;

        if((ie - ip) < 1)
            return false;

        byte = *ip++;

        for(int i = 7; i >= 0; i--) {
            int flag = (byte >> i) & 1;

            if(!flag) {
                if((ie - ip) < 1 || (oe - op) < 1)
                    return false;

                *op++ = *ip++;
            } else {
                int v;
                int type;
                int count;
                int disp;

                if((ie - ip) < 1)
                    return false;

                v = *ip++;
                type = (v & 0xf0) >> 4;

                if(type == 0) {
                    count = (v & 0x0f) << 4;

                    if((ie - ip) < 1)
                        return false;

                    v = *ip++;
                    count |= ((v & 0xf0) >> 4);
                    count += 17;
                } else if(type == 1) {
                    count = (v & 0x0f) << 12;

                    if((ie - ip) < 2)
                        return false;

                    v = (ip[0] << 8) | ip[1];
                    ip += 2;

                    count |= (v & 0xfff0) >> 4;
                    count += 273;
                } else {
                    count = 1 + type;
                }

                disp = (v & 0x0f) << 8;

                if((ie - ip) < 1)
                    return false;

                disp |= *ip++;
                disp += 1;

                if((oe - op) < count)
                    return false;

                if((op - os) < disp)
                    return false;

                for(int j = 0; j < count; j++)
                    op[j] = op[j - disp];

                op += count;
            }

            if(op == oe)
                break;
        }
    }

    return (op == oe);
}

extern ssize_t lzss_decompress(const uint8_t* in, size_t in_size, uint8_t* out, size_t out_size) {
    uint8_t header[4];
    int type;
    size_t uncompressed_size;

    if (in_size < 4)
        return -1;

    memcpy(header, in, 4);
    in += 4;
    in_size -= 4;

    type = header[0];
    uncompressed_size = header[1] | (header[2] << 8) | (header[3] << 16);

    if (out_size < uncompressed_size)
        return -1;

    out_size = uncompressed_size;

    if (type == LZSS_TYPE_10) {
        if (!decompress_lzss_10(in, in_size, out, out_size))
            return -1;
    } else if (type == LZSS_TYPE_11) {
        if (!decompress_lzss_11(in, in_size, out, out_size))
            return -1;
    } else {
        return -1;
    }

    return out_size;
}

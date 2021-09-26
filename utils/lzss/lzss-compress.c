#include "lzss.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct index {
    struct index* next;
    size_t value;
} index_t;

typedef struct {
    index_t* head;
    index_t* tail;
} index_list_t;

typedef struct match {
    struct match* next;
    size_t count;
    size_t disp;
} match_t;

typedef struct {
    match_t* head;
    match_t* tail;
} match_list_t;

typedef struct {
    const uint8_t* data;
    size_t data_len;
    size_t size;
    size_t disp_min;
    size_t match_min;
    size_t match_max;
    size_t start;
    size_t stop;
    size_t index;
    index_list_t hash[256];
    bool full;
} window_t;

typedef union {
    match_t match;
    uint8_t byte;
} token_t;

static size_t minimum(size_t a, size_t b) {
    return (a < b) ? a : b;
}

static bool index_list_push(index_list_t* il, size_t value) {
    index_t* ie = malloc(sizeof(index_t));

    if(ie == NULL)
        return false;

    ie->next = NULL;
    ie->value = value;

    if(il->tail == NULL) {
        il->head = ie;
        il->tail = ie;
    } else {
        il->tail->next = ie;
        il->tail = ie;
    }

    return true;
}

static bool index_list_pop(index_list_t* il) {
    index_t* ie = il->head;

    if(ie == NULL)
        return false;

    il->head = ie->next;
    free(ie);

    if(il->head == NULL)
        il->tail = NULL;

    return true;
}

static bool match_list_push(match_list_t* ml, size_t count, size_t disp) {
    match_t* me = malloc(sizeof(match_t));

    if(me == NULL)
        return false;

    me->next = NULL;
    me->count = count;
    me->disp = disp;

    if(ml->tail == NULL) {
        ml->head = me;
        ml->tail = me;
    } else {
        ml->tail->next = me;
        ml->tail = me;
    }

    return true;
}

static bool match_list_pop(match_list_t* ml) {
    match_t* me = ml->head;

    if(me == NULL)
        return false;

    ml->head = me->next;
    free(me);

    if(ml->head == NULL)
        ml->tail = NULL;

    return true;
}

static window_t* window_create(const uint8_t* data, size_t data_len, size_t match_max) {
    window_t* w = malloc(sizeof(window_t));

    if(w == NULL)
        return NULL;

    w->data = data;
    w->data_len = data_len;
    w->size = 4096;
    w->disp_min = 2;
    w->match_min = 3;
    w->match_max = match_max;
    w->start = 0;
    w->stop = 0;
    w->index = 0;
    memset(&w->hash, 0, sizeof(w->hash));
    w->full = false;

    return w;
}

static void window_destroy(window_t* w) {
    if(w == NULL)
        return;

    for(size_t i = 0; i < 256; i++)
        while(index_list_pop(&w->hash[i]))
            ;

    free(w);
}

static bool window_next(window_t* w) {
    if(w->full)
        index_list_pop(&w->hash[w->data[w->start]]);

    if(!index_list_push(&w->hash[w->data[w->stop]], w->stop))
        return false;

    w->stop++;
    w->index++;

    if(w->full)
        w->start++;
    else if(w->size <= w->stop)
        w->full = true;

    return true;
}

static bool window_advance(window_t* w, size_t n) {
    size_t i;

    for(i = 0; i < n; i++)
        if(!window_next(w))
            break;

    return (i == n);
}

static size_t window_match(window_t* w, size_t start, size_t bufstart) {
    size_t size = w->index - start;
    size_t matchlen = 0;
    size_t i = 0;
    size_t j;

    if(size == 0)
        return 0;

    j = minimum(w->data_len - bufstart, w->match_max);

    for(; i < j; i++)
        if(w->data[start + (i % size)] == w->data[bufstart + i])
            matchlen++;
        else
            break;

    return matchlen;
}

static int window_search(window_t* w, match_t* me) {
    size_t match_min = w->match_min;
    size_t match_max = w->match_max;
    match_list_t counts = {};
    index_list_t* il = &w->hash[w->data[w->index]];

    for(index_t* ip = il->head; ip != NULL; ip = ip->next) {
        size_t i = ip->value;
        size_t matchlen = window_match(w, i, w->index);

        if(matchlen >= match_min) {
            size_t disp = w->index - i;
            if(w->disp_min <= disp) {
                if(!match_list_push(&counts, matchlen, disp)) {
                    while(match_list_pop(&counts))
                        ;
                    return -1;
                }

                if(matchlen >= match_max) {
                    *me = *counts.tail;
                    while(match_list_pop(&counts))
                        ;
                    return 1;
                }
            }
        }
    }

    if(counts.head == NULL)
        return 0;

    memset(me, 0, sizeof(match_t));

    for(match_t* mp = counts.head; mp != NULL; mp = mp->next)
        if(me->count < mp->count)
            *me = *mp;

    while(match_list_pop(&counts))
        ;

    return 1;
}

static int window_next_token(window_t* w, size_t* pos, uint8_t* flag, token_t* token) {
    int ms;
    size_t count;

    if(*pos >= w->data_len)
        return 0;

    if((ms = window_search(w, &token->match)) < 0)
        return -1;

    *flag <<= 1;

    if(ms == 0) {
        token->byte = w->data[*pos];
        count = 1;
        *flag |= 0;
    } else {
        count = token->match.count;
        *flag |= 1;
    }

    if(!window_advance(w, count))
        return -1;

    *pos += count;

    return 1;
}

static int window_next_chunk(window_t* w, size_t* pos, uint8_t* flag, token_t* tokens) {
    size_t i;
    int ts;

    *flag = 0;

    for(i = 0; i < 8; i++)
        if((ts = window_next_token(w, pos, flag, &tokens[i])) != 1)
            break;

    if(ts == -1)
        return -1;

    if(ts == 0)
        *flag <<= 8 - i;

    return i;
}

static ssize_t compress_lzss_10(const uint8_t* in, size_t in_size, uint8_t* out, size_t out_size) {
    ssize_t size = -1;
    window_t* w = window_create(in, in_size, 18);
    size_t pos = 0;
    uint8_t flags;
    token_t tokens[8];
    int cs;
    uint8_t* os = out;
    uint8_t* op = out;
    uint8_t* oe = out + out_size;

    if(w == NULL)
        goto bail;

    while((cs = window_next_chunk(w, &pos, &flags, tokens)) > 0) {
        if((oe - op) < 1)
            goto bail;

        *op++ = flags;

        for(int i = 7; i >= (8 - cs); i--) {
            size_t flag = (flags >> i) & 1;
            token_t* token = &tokens[7 - i];

            if(!flag) {
                if((oe - op) < 1)
                    goto bail;

                *op++ = token->byte;
            } else {
                size_t count = token->match.count;
                size_t disp = token->match.disp;
                uint16_t v;

                if((oe - op) < 2)
                    goto bail;

                count -= 3;
                disp -= 1;
                v = (count << 12) | disp;

                op[0] = (v & 0xff00) >> 8;
                op[1] = (v & 0x00ff) >> 0;
                op += 2;
            }
        }
    }

    size = (op - os);

bail:
    window_destroy(w);
    return size;
}

static ssize_t compress_lzss_11(const uint8_t* in, size_t in_size, uint8_t* out, size_t out_size) {
    ssize_t size = -1;
    window_t* w = window_create(in, in_size, 65808);
    size_t pos = 0;
    uint8_t flags;
    token_t tokens[8];
    int cs;
    uint8_t* os = out;
    uint8_t* op = out;
    uint8_t* oe = out + out_size;

    if(w == NULL)
        goto bail;

    while((cs = window_next_chunk(w, &pos, &flags, tokens)) > 0) {
        if((oe - op) < 1)
            goto bail;

        *op++ = flags;

        for(int i = 7; i >= (8 - cs); i--) {
            size_t flag = (flags >> i) & 1;
            token_t* token = &tokens[7 - i];

            if(!flag) {
                if((oe - op) < 1)
                    goto bail;

                *op++ = token->byte;
            } else {
                size_t count = token->match.count;
                size_t disp = token->match.disp;
                uint32_t v;

                disp -= 1;

                if(count <= 16) {
                    if((oe - op) < 2)
                        goto bail;

                    count -= 1;
                    v = (count << 12) | disp;

                    op[0] = (v & 0xff00) >> 8;
                    op[1] = (v & 0x00ff) >> 0;
                    op += 2;
                } else if(count <= 272) {
                    if((oe - op) < 3)
                        goto bail;

                    count -= 17;
                    v = (0 << 20) | (count << 12) | disp;

                    op[0] = (v & 0xff0000) >> 16;
                    op[1] = (v & 0x00ff00) >>  8;
                    op[2] = (v & 0x0000ff) >>  0;
                    op += 3;
                } else if(count <= 65808) {
                    if((oe - op) < 4)
                        goto bail;

                    count -= 273;
                    v = (1 << 28) | (count << 12) | disp;

                    op[0] = (v & 0xff000000) >> 24;
                    op[1] = (v & 0x00ff0000) >> 16;
                    op[2] = (v & 0x0000ff00) >>  8;
                    op[3] = (v & 0x000000ff) >>  0;
                    op += 4;
                } else {
                    goto bail;
                }
            }
        }
    }

    size = (op - os);

bail:
    window_destroy(w);
    return size;
}

extern ssize_t lzss_compress(const uint8_t* in, size_t in_size, uint8_t* out, size_t out_size, int type) {
    ssize_t size;
    size_t padding;

    if(in_size > LZSS_MAX_SIZE)
        return -1;

    if(out_size < 4)
        return -1;

    out[0] = type;
    out[1] = (in_size & 0x0000ff) >>  0;
    out[2] = (in_size & 0x00ff00) >>  8;
    out[3] = (in_size & 0xff0000) >> 16;

    out += 4;
    out_size -= 4;

    if(type == LZSS_TYPE_10) {
        if((size = compress_lzss_10(in, in_size, out, out_size)) < 0)
            return -1;
    } else if(type == LZSS_TYPE_11) {
        if((size = compress_lzss_11(in, in_size, out, out_size)) < 0)
            return -1;
    } else {
        return -1;
    }

    out += size;
    out_size -= size;
    padding = 4 - (size % 4);

    if(padding != 4) {
        if(out_size < padding)
            return -1;

        for(size_t i = 0; i < padding; i++)
            out[i] = 0xff;

        out += padding;
        out_size -= padding;
        size += padding;
    }

    return 4 + size;
}

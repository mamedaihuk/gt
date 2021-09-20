#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include "lzss.h"

enum {
    DECODE = 1,
    ENCODE = 2,
};

static const char* progname;
static const char* suffix;
static int mode;
static int method;

static int parse_arguments(int argc, char** argv) {
    int opt;

    progname = argv[0];
    suffix = ".lz";

    while((opt = getopt(argc, argv, ":edm:s:h")) != -1) {
        switch(opt) {
            case 'e':
                mode = ENCODE;
                break;

            case 'd':
                mode = DECODE;
                break;

            case 'm':
                if(strcmp(optarg, "10") == 0) {
                    method = LZSS_TYPE_10;
                } else if(strcmp(optarg, "11") == 0) {
                    method = LZSS_TYPE_11;
                } else {
                    fprintf(stderr, "%s: unknown encoding method '%s'\n", progname, optarg);
                    return -1;
                }
                break;

            case 's':
                suffix = optarg;
                break;

            case 'h':
                fprintf(stderr,
                    "Usage: %s [options] <files>\n"
                    "\n"
                    "  -e Encode operation\n"
                    "  -d Decode operation\n"
                    "  -m Encoding method (10 or 11)\n"
                    "  -s Change file suffix (default: .lz)\n"
                    "  -h This help message\n",
                    progname);
                return 1;

            case ':':
                fprintf(stderr, "%s: missing argument '%c'\n", progname, optopt);
                return -1;

            case '?':
                fprintf(stderr, "%s: unknown argument '%c'\n", progname, optopt);
                return -1;
        }
    }

    if(mode != ENCODE && mode != DECODE) {
        fprintf(stderr, "%s: mode not specified\n", progname);
        return -1;
    }

    if(mode == ENCODE && method == 0) {
        fprintf(stderr, "%s: encoding method not specified\n", progname);
        return -1;
    }

    if(suffix[0] == '\0') {
        fprintf(stderr, "%s: suffix cannot be empty\n", progname);
        return -1;
    }

    if(optind >= argc) {
        fprintf(stderr, "%s: no files to process\n", progname);
        return -1;
    }

    return optind;
}

static int decode_files(int argc, char** argv) {
    int rv = -1;
    size_t in_size = LZSS_BUFFER_SIZE;
    uint8_t* in = malloc(in_size);
    const char* in_path;
    int in_fd = -1;
    size_t out_size = LZSS_BUFFER_SIZE;
    uint8_t* out = malloc(out_size);
    char out_path[PATH_MAX];
    int out_fd = -1;
    char* p;
    size_t n;
    struct stat st;
    ssize_t bytes;

    if(in == NULL || out == NULL) {
        fprintf(stderr, "%s: failed to allocate io buffers\n", progname);
        goto bail;
    }

    for(int i = 0; i < argc; i++) {
        in_path = argv[i];

        if((p = strstr(in_path, suffix)) == NULL) {
            fprintf(stderr, "%s: suffix not present '%s'\n", progname, in_path);
            goto bail;
        }

        if ((n = p - in_path) >= sizeof(out_path)) {
            fprintf(stderr, "%s: path too large '%s'\n", progname, in_path);
            goto bail;
        }

        memcpy(out_path, in_path, n);
        out_path[n] = '\0';

        if((in_fd = open(in_path, O_RDONLY)) == -1) {
            fprintf(stderr, "%s: failed to open input file '%s'\n", progname, in_path);
            goto bail;
        }

        if((out_fd = creat(out_path, 0644)) == -1) {
            fprintf(stderr, "%s: failed to open output file '%s'\n", progname, out_path);
            goto bail;
        }

        if(fstat(in_fd, &st) == -1) {
            fprintf(stderr, "%s: failed to fstat input file '%s'\n", progname, in_path);
            goto bail;
        }

        if(st.st_size > LZSS_BUFFER_SIZE) {
            fprintf(stderr, "%s: input file too large '%s'\n", progname, in_path);
            goto bail;
        }

        in_size = st.st_size;

        if(read(in_fd, in, in_size) != in_size) {
            fprintf(stderr, "%s: failed to read input file '%s'\n", progname, in_path);
            goto bail;
        }

        if((bytes = lzss_decompress(in, in_size, out, out_size)) < 0) {
            fprintf(stderr, "%s: failed to decode input file '%s'\n", progname, in_path);
            goto bail;
        }

        if(write(out_fd, out, bytes) != bytes) {
            fprintf(stderr, "%s: failed to write output file '%s'\n", progname, out_path);
            goto bail;
        }

        close(in_fd);
        in_fd = -1;

        close(out_fd);
        out_fd = -1;
    }

    rv = 0;

bail:
    free(in);
    free(out);
    if(in_fd != -1)
        close(in_fd);
    if(out_fd != -1)
        close(out_fd);
    return rv;
}

static int encode_files(int argc, char** argv) {
    int rv = -1;
    size_t in_size = LZSS_BUFFER_SIZE;
    uint8_t* in = malloc(in_size);
    const char* in_path;
    int in_fd = -1;
    size_t out_size = LZSS_BUFFER_SIZE;
    uint8_t* out = malloc(out_size);
    char out_path[PATH_MAX];
    int out_fd = -1;
    char* p;
    struct stat st;
    ssize_t bytes;

    if(in == NULL || out == NULL) {
        fprintf(stderr, "%s: failed to allocate io buffers\n", progname);
        goto bail;
    }

    for(int i = 0; i < argc; i++) {
        in_path = argv[i];

        p = memccpy(out_path, in_path, '\0', sizeof(out_path));
        --p;
        memccpy(p, suffix, '\0', sizeof(out_path) - (p - out_path));

        if((in_fd = open(in_path, O_RDONLY)) == -1) {
            fprintf(stderr, "%s: failed to open input file '%s'\n", progname, in_path);
            goto bail;
        }

        if((out_fd = creat(out_path, 0644)) == -1) {
            fprintf(stderr, "%s: failed to open output file '%s'\n", progname, out_path);
            goto bail;
        }

        if(fstat(in_fd, &st) == -1) {
            fprintf(stderr, "%s: failed to fstat input file '%s'\n", progname, in_path);
            goto bail;
        }

        if(st.st_size > LZSS_MAX_SIZE) {
            fprintf(stderr, "%s: input file too large '%s'\n", progname, in_path);
            goto bail;
        }

        in_size = st.st_size;

        if(read(in_fd, in, in_size) != in_size) {
            fprintf(stderr, "%s: failed to read input file '%s'\n", progname, in_path);
            goto bail;
        }

        if((bytes = lzss_compress(in, in_size, out, out_size, method)) < 0) {
            fprintf(stderr, "%s: failed to decode input file '%s'\n", progname, in_path);
            goto bail;
        }

        if(write(out_fd, out, bytes) != bytes) {
            fprintf(stderr, "%s: failed to write output file '%s'\n", progname, out_path);
            goto bail;
        }

        close(in_fd);
        in_fd = -1;

        close(out_fd);
        out_fd = -1;
    }

    rv = 0;

bail:
    free(in);
    free(out);
    if(in_fd != -1)
        close(in_fd);
    if(out_fd != -1)
        close(out_fd);
    return rv;
}

int main(int argc, char** argv) {
    int i;

    if((i = parse_arguments(argc, argv)) < 0)
        return 1;

    if(mode == DECODE && decode_files(argc - i, argv + i) < 0)
        return 1;

    if(mode == ENCODE && encode_files(argc - i, argv + i) < 0)
        return 1;

    return 0;
}

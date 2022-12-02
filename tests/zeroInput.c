/*
 * Copyright (c) Yann Collet, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */

#include <stdio.h> /* printf */
#include <stdint.h>
#include <string.h> /* strlen */
#define ZSTD_STATIC_LINKING_ONLY
#include "zstd.h"

int main(void)
{
    /* Compress some data. */
    char compressed[100];
    size_t compressedSize;
    {
        const char *inputData = "foobar";
        size_t got = ZSTD_compress(compressed, sizeof(compressed), inputData, strlen(inputData) + 1, 1);
        if (ZSTD_isError(got))
        {
            printf("got error on ZSTD_compress: %zu (%s)\n", got, ZSTD_getErrorName(got));
            return 1;
        }
        compressedSize = got;
    }

    /* Decompress the data with the stream API. */
    {
        char decompressed[100] = {0};
        ZSTD_DStream *stream = ZSTD_createDStream();
        ZSTD_inBuffer input = (ZSTD_inBuffer){.src = compressed, .size = compressedSize, .pos = 0};
        ZSTD_outBuffer output = (ZSTD_outBuffer){.dst = decompressed, .size = sizeof(decompressed), .pos = 0};
        while (1)
        {
            size_t got = ZSTD_decompressStream(stream, &output, &input);
            if (ZSTD_isError(got))
            {
                printf("got error on ZSTD_decompressStream: %zu (%s)\n", got, ZSTD_getErrorName(got));
                return 1;
            }
            if (got == 0) {
                /* Done. */
                break;
            }
        }
        if (0 != strcmp (decompressed, "foobar")) {
            printf ("expected 'foobar', got: '%s'\n", decompressed);
            return 1;
        }
        ZSTD_freeDStream(stream);
    }

    /* Decompress the data with the stream API. */
    {
        char decompressed[100] = {0};
        ZSTD_DStream *stream = ZSTD_createDStream();
        ZSTD_inBuffer input = (ZSTD_inBuffer){.src = compressed, .size = 0, .pos = 0};
        ZSTD_outBuffer output = (ZSTD_outBuffer){.dst = decompressed, .size = sizeof(decompressed), .pos = 0};
        int max = 100;
        while (1) {
            size_t got = ZSTD_decompressStream(stream, &output, &input);
            if (ZSTD_isError(got))
            {
                printf("got error on ZSTD_decompressStream: %zu (%s)\n", got, ZSTD_getErrorName(got));
                return 1;
            }
            printf ("after decompressing 0 bytes, got: %zu\n", got);
            printf ("decompressed is: %s\n", decompressed);
            if (got == 0) {
                break;
            }
            
            if (max-- < 0) {
                break;
            }
        }
        
        ZSTD_freeDStream(stream);
    }
}

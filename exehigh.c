/*

EXEHIGH - touch .exe header to load the image into highest address
          in the (conventiocal) memory
          (set MINALLOC and MAXALLOC in the EXE header to zero)

history:
2023-07-13 lpproj   quick'n dirty makeup


This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int optHelp;
char *param;

int mygetopt(int argc, char **argv)
{
    int paramcnt = 0;

    while(--argc > 0) {
        char *s = *++argv;
        if (*s == '/' || *s == '-') {
            char c = *++s;
            if (c == '?' || c == 'h' || c == 'H') optHelp = 1;
        }
        else {
            if (!param) {
                param = s;
                ++paramcnt;
            }
        }
    }
    return paramcnt;
}

int main(int argc, char **argv)
{
    unsigned char mz[0x20];
    long flen = -1L;
    FILE *f;
    size_t rwcnt;

    if (mygetopt(argc, argv) < 1 && !optHelp) {
        fprintf(stderr, "Type EXEHIGH -? to help.\n");
        return 1;
    }
    if (optHelp) {
        const char msg[] = 
            "Usage: EXEHIGH exefile\n"
            "touch EXE to set MINALLOC and MAXALLOC = 0\n"
            ;
        printf("%s", msg);
        return 0;
    }
    f = fopen(param, "rb");
    if (!f) {
        fprintf(stderr, "error: can't open '%s'\n.", param);
        return 1;
    }
    rwcnt = fread(mz, sizeof(mz), 1, f);
    if (rwcnt > 0 && fseek(f, 0L, SEEK_END) == 0) flen = ftell(f);
    fclose(f);
    if (flen == -1L || mz[0] != 0x4d || mz[1] != 0x5a) {
        fprintf(stderr, "error: file '%s' read err (or not EXE)\n", param);
        return 1;
    }

    mz[0x0a] = mz[0x0b] = 0;    /* MINALLOC = 0 */
    mz[0x0c] = mz[0x0d] = 0;    /* MAXALLOC = 0 */

    f = fopen(param, "r+b");
    if (!f) {
        fprintf(stderr, "error: can't open '%s' to write.\n.", param);
        return 1;
    }
    setvbuf(f, NULL, _IONBF, 0);
    fseek(f, 0, SEEK_SET);
    rwcnt = fwrite(mz, sizeof(mz), 1, f);
    fflush(f);
    fseek(f, flen, SEEK_SET);
    fclose(f);
    if (rwcnt == 0) {
        fprintf(stderr, "error: file '%s' write err.\n.", param);
        return 1;
    }

    return 0;
}


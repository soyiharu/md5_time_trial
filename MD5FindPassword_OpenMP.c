#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include "md5.h"

#define MD5_DIGEST_LENGTH 16

int main(int argc, char* argv[])
{
    char origin[256];
    char hex[3];
    char result[MD5_DIGEST_LENGTH * 2 + 1];
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX ctx;
    int i, j;

    if (argc != 3) return 1;
    char* SALT = argv[1];
    char* PW = argv[2];

    const clock_t start_time = clock();

    #pragma omp parallel private(origin, hex, result, hash, ctx, i, j) num_threads(4)
    {
        #pragma omp for
        for (i = 0; i < 1000000; i++) {
            result[0] = '\0';
            sprintf_s(origin, sizeof(origin), "%s$%06d", SALT, i);

            MD5_Init(&ctx);
            MD5_Update(&ctx, origin, (unsigned long)strlen(origin));
            MD5_Final(hash, &ctx);
            for (j = 0; j < MD5_DIGEST_LENGTH; j++) {
                sprintf_s(hex, sizeof(hex), "%02x", hash[j]);
                strcat_s(result, sizeof(result), hex);
            }
            if (strcmp(result, PW) == 0) {
                printf("match[%06d]\n", i);
            }
        }
    }

    const clock_t end_time = clock();
    const double sec = (end_time - start_time) / 1e3;
    printf("elapsed time:%fs\n", sec);

    return 0;
}

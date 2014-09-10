#include <iostream>
#include <boost/chrono.hpp>
#include <boost/format.hpp>
#include <boost/assert.hpp>
#include <tbb/tbb.h>
#include "md5.h"

#define MD5_DIGEST_LENGTH 16
union MD5_HASH {
    unsigned char hash[MD5_DIGEST_LENGTH];
    unsigned int hash32[MD5_DIGEST_LENGTH / 4];
};

int main(int argc, char* argv[])
{
    if (argc != 3) return 1;
    BOOST_ASSERT(sizeof(int) == 4);
    
    const size_t num_threads = 4;
    const int max_loop = static_cast<int>(1e6);
    const int step = max_loop / num_threads;

    const char* SALT = argv[1];
    const char* PW = argv[2];

    MD5_HASH reference;

    const boost::chrono::system_clock::time_point start_time = boost::chrono::system_clock::now();
    
    tbb::task_scheduler_init init(num_threads);

    char buf[9];
    buf[8] = '\0';
    for (int i = 0; i < MD5_DIGEST_LENGTH / 4; ++i) {
        buf[0] = PW[i * 8 + 6];
        buf[1] = PW[i * 8 + 7];
        buf[2] = PW[i * 8 + 4];
        buf[3] = PW[i * 8 + 5];
        buf[4] = PW[i * 8 + 2];
        buf[5] = PW[i * 8 + 3];
        buf[6] = PW[i * 8 + 0];
        buf[7] = PW[i * 8 + 1];
        char* endptr;
        reference.hash32[i] = std::strtol(buf, &endptr, 16);
    }
    
    const size_t size = strlen(SALT) + 1;
    tbb::parallel_for(0, max_loop, step, [&](const size_t begin) {
        MD5_HASH hash;
        MD5_CTX ctx;
        
        char *origin = new char[size + 6 + 1];
        sprintf_s(origin, size + 6 + 1, "%s$", SALT);
        
        const size_t end = begin + step;
        for (size_t i = begin; i != end; ++i) {
            sprintf_s(origin + size, 6 + 1, "%06d", i);

            MD5_Init(&ctx);
            MD5_Update(&ctx, origin, size + 6);
            MD5_Final(hash.hash, &ctx);

            if (reference.hash32[0] == hash.hash32[0] && reference.hash32[1] == hash.hash32[1] 
                    && reference.hash32[2] == hash.hash32[2] && reference.hash32[3] == hash.hash32[3]) {
                std::cout << boost::format("match[%1$06d]") % i << std::endl;
            }
        }
        delete[] origin;
    });
    
    const boost::chrono::system_clock::time_point end_time = boost::chrono::system_clock::now();
    const boost::chrono::duration<double> sec = end_time - start_time;
    std::cout << "elapsed time:" << sec.count() << "s" << std::endl;
    return 0;
}
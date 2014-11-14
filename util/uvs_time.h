#ifndef __COM_GITHUB_UNIX1986_UNIVERSE_UTIL_UVS_TIME_H__
#define __COM_GITHUB_UNIX1986_UNIVERSE_UTIL_UVS_TIME_H__
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

namespace github{
namespace unix1986{
namespace universe{
namespace util{

class UvsTime{
public:
    // elapsed seconds
    static uint64_t GetTimeS() {
        return static_cast<uint64_t>(time(NULL));
    }
    // elapsed milliseconds
    static uint64_t GetTimeMs() {
        struct timeval now;
        gettimeofday(&now, NULL);
        return static_cast<uint64_t>(now.tv_sec * 1000 + now.tv_usec / 1000);
    }
    // elapsed microseconds
    static uint64_t GetTimeUs() {
        struct timeval now;
        gettimeofday(&now, NULL);
        return static_cast<uint64_t>(now.tv_sec * 1000 * 1000 + now.tv_usec);
    }
    // elapsed nanoseconds, may not work
    static uint64_t GetTimeNs() {
        struct timespec now;
        #ifdef __POSIX_TIMERS
        if (__POSIX_TIMERS > 0) {
            clock_gettime(CLOCK_REALTIME, &now);
        }
        #else
        memset(&now, 0, sizeof(now));
        #endif
        return static_cast<uint64_t>(now.tv_sec * 1000 * 1000 * 1000 + now.tv_nsec);
    }
};

}}}} // github::unix1986::universe:util
#endif  //COM_GITHUB_UNIX1986_UNIVERSE_UTIL_UVS_TIME_H__

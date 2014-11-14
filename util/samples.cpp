#include "dbg_log.h"
#include "uvs_time.h"

using namespace github::unix1986::universe::util;
int main(int argc, char *argv[])
{
    printf("sec: %llu\n", UvsTime::GetTimeS());
    printf("msec: %llu\n", UvsTime::GetTimeMs());
    printf("usec: %llu\n", UvsTime::GetTimeUs());
    printf("nsec: %llu\n", UvsTime::GetTimeNs());
    return 0;
}

// coming from libevhtp-htparse.c
// unix1986@qq.com
// Format: [mon-day hour:min:sec] "source_file_name":[line_no] func_name: user_content
#ifndef __TOOL_INCLUDE_DBG_LOG__
#define __TOOL_INCLUDE_DBG_LOG__

#include <stdio.h>
#include <time.h>

#ifdef __DBG_LOG__
#define __QUOTE(x)                  # x
#define  _QUOTE(x)                  __QUOTE(x)
#define dbglog_debug_strlen(x)     strlen(x)

// Interface
#define __dbg_log(fmt, ...) do {                                                                                     \
        time_t      t  = time(NULL);                                                                                 \
        struct tm * dm = localtime(&t);                                                                              \
                                                                                                                     \
        fprintf(stdout, "[%02d-%02d %02d:%02d:%02d] "_QUOTE(__FILE__)":[" _QUOTE(__LINE__) "] %-s: "                 \
        fmt "\n", dm->tm_mon + 1, dm->tm_mday, dm->tm_hour, dm->tm_min, dm->tm_sec, __func__, ## __VA_ARGS__);       \
        fflush(stdout);                                                                                              \
} while (0)

#else
#define dbglog_debug_strlen(x)     0
#define __dbg_log(fmt, ...) do {} while (0)
#endif

#endif //__TOOL_INCLUDE_DBG_LOG__

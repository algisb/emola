#ifndef DEBUG_H_
#define DEBUG_H_
#include <stdio.h>

#define SHOW_DEBUG_INFO

#ifdef SHOW_DEBUG_INFO
#define PRINT_DEBUG(_str, ...) \
dbg_printLog(_str, ##__VA_ARGS__)
#else
#define PRINT_DEBUG(_str)
#endif

void dbg_initLogging();
void dbg_deinitLogging();
void dbg_printLog(char * _str, ...);

#endif

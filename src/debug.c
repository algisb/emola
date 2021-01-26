#include "debug.h"
#include <stdarg.h>

FILE * dbg_logFile;
void dbg_initLogging()
{
    //discard old file contents
    dbg_logFile = fopen("debug.log", "w");
    fclose(dbg_logFile);
}

void dbg_deinitLogging()
{
}

void dbg_printLog(char * _str, ...)
{
    va_list args;

    va_start(args, _str);
    vprintf(_str, args);
    va_end(args);
    
    va_start(args, _str);
    dbg_logFile = fopen("debug.log", "a+");
    vfprintf(dbg_logFile, _str, args);
    fclose(dbg_logFile);
    va_end(args);
}

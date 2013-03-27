/*
 * Logging header
 * Version: $Id: logging.h 147 2009-11-12 23:33:24Z tipok $
 *
 */

#ifndef LOGGING_H_
#define LOGGING_H_

enum {
    LOG_ERROR,
    LOG_INFO,
    LOG_DEBUG,
    LOG_XTREME,
    LOG_DUMP,
};

extern int     loglevel;

void logwrite(int level, const char *format, ...);
void print_bytes(int level, char *bytes, int len);

#endif /*LOGGING_H_*/

/*
 * Logging code
 * Version: $Id: logging.c 147 2009-11-12 23:33:24Z tipok $
 *
 */


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/param.h>
#include <time.h>
#include <ctype.h>

#include "logging.h"

int     loglevel=LOG_ERROR;

void logwrite(int level, const char *format, ...) {
	va_list		pvar;
	char		logbuffer[MAXPATHLEN];
	time_t		t;

	if (level > loglevel)
		return;

	va_start (pvar, format);
	vsnprintf(logbuffer, sizeof(logbuffer), format, pvar);
	va_end (pvar);

	t=time(NULL);

	fprintf(stderr, "%-24.24s %s\n",
			ctime (&t),
			logbuffer);
}

void print_bytes(int level, char *bytes, int len) {
    int i;
    int count;
    int done = 0;

    if (level > loglevel)
		return;

    while (len > done) {
		if (len - done > 32){
			count = 32;
		} else {
			count = len - done;
		}
		
		fprintf(stderr, "\t");
		
		for (i=0; i<count; i++) {
	    	fprintf(stderr, "%02x ", (int)((unsigned char)bytes[done+i]));
		}
		
		for (; i<32; i++) {
			fprintf(stderr, "   ");
		}

		fprintf(stderr, "\t\"");
		
	        for (i=0; i<count; i++) {
		    	fprintf(stderr, "%c", isprint(bytes[done+i]) ? bytes[done+i] : '.');
	        }

        fprintf(stderr, "\"\n");
    	done += count;
    }
}

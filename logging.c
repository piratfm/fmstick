/*
 * code for logging and dumping
 * $Id$
 * (part of FmStick).
 *
 * (C) 2011 Serge <piratfm@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/param.h>
#include <time.h>
#include <ctype.h>

#include "logging.h"

#ifdef QT_NO_DEBUG
int     loglevel=LOG_INFO;
#else
int     loglevel=LOG_DEBUG;
#endif

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
    fflush(stderr);
}

void print_bytes(int level, const char *bytes, int len) {
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
            fprintf(stderr, "%02x ", (int)((const unsigned char)bytes[done+i]));
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
    fflush(stderr);
}

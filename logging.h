/*
 * logging header
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

#ifndef LOGGING_H_
#define LOGGING_H_

#ifdef __cplusplus
extern "C" {
#endif

enum {
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_XTREME,
    LOG_DUMP,
};

extern int     loglevel;

void logwrite(int level, const char *format, ...);
void print_bytes(int level, const char *bytes, int len);

#ifdef __cplusplus
}
#endif

#endif /*LOGGING_H_*/

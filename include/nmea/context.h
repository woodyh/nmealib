/*
 * This file is part of nmealib.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __NMEA_CONTEXT_H__
#define __NMEA_CONTEXT_H__

#include <nmea/conf.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Function type definition for tracing
 *
 * @param str the string to trace
 * @param str_size the length of the string
 */
typedef void (*nmeaTraceFunc)(const char *str, int str_size);

/**
 * Function type definition for error logging
 *
 * @param str the string to log
 * @param str_size the length of the string
 */
typedef void (*nmeaErrorFunc)(const char *str, int str_size);

void nmea_context_set_trace_func(nmeaTraceFunc func);
void nmea_context_set_error_func(nmeaErrorFunc func);

void nmea_trace(const char *str, ...) __attribute__ ((format(printf, 1, 2)));
void nmea_trace_buff(const char *buff, int buff_size);
void nmea_error(const char *str, ...) __attribute__ ((format(printf, 1, 2)));

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __NMEA_CONTEXT_H__ */

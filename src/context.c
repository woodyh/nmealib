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

#include <nmea/context.h>

#include <stdarg.h>
#include <stdio.h>

/**
 * The structure with nmealib context.
 */
typedef struct _nmeaPROPERTY {
	nmeaErrorFunc error_func; /**< the error function, defaults to NULL (disabled) */
	int parse_buff_size; /**< the size to use for temporary buffers, minimum is NMEA_MIN_PARSEBUFF */
} nmeaPROPERTY;

/** the nmealib context */
#if NMEA_ERROR
static nmeaPROPERTY property = { .error_func = NULL, .parse_buff_size = NMEA_DEF_PARSEBUFF };

/**
 * Set the error function
 *
 * @param func the error function
 */
void nmea_context_set_error_func(nmeaErrorFunc func) {
	property.error_func = func;
}

/**
 * Log a formatted error string
 *
 * @param str a formatted error string
 */
void nmea_error(const char *str, ...) {
	nmeaErrorFunc func = property.error_func;

	if (func) {
		int size;
		va_list arg_list;
		char buff[property.parse_buff_size];

		va_start(arg_list, str);
		size = vsnprintf(&buff[0], property.parse_buff_size - 1, str, arg_list);
		va_end(arg_list);

		if (size > 0)
			(*func)(&buff[0], size);
	}
}
#endif

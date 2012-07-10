/*
 * This file is part of nmealib.
 *
 * Copyright (c) 2008 Timur Sinitsyn
 * Copyright (c) 2011 Ferry Huberts
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

#ifndef __NMEA_PARSE_H__
#define __NMEA_PARSE_H__

#include <nmea/info.h>
#include <nmea/sentence.h>

#include <stdbool.h>
#include <stddef.h>

#ifdef  __cplusplus
extern "C" {
#endif

bool nmea_string_has_invalid_chars(const char * str, const char * strName, char * report, size_t reportSize);

int nmea_pack_type(const char *s, int len);
int nmea_find_tail(const char *s, int len, int *checksum);

int nmea_parse_GPGGA(const char *s, int len, nmeaGPGGA *pack);
int nmea_parse_GPGSA(const char *s, int len, nmeaGPGSA *pack);
int nmea_parse_GPGSV(const char *s, int len, nmeaGPGSV *pack);
int nmea_parse_GPRMC(const char *s, int len, nmeaGPRMC *pack);
int nmea_parse_GPVTG(const char *s, int len, nmeaGPVTG *pack);

#ifdef  __cplusplus
}
#endif

#endif /* __NMEA_PARSE_H__ */

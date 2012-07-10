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

#ifndef __NMEA_GENERATE_H__
#define __NMEA_GENERATE_H__

#include <nmea/info.h>
#include <nmea/sentence.h>

#ifdef  __cplusplus
extern "C" {
#endif

int nmea_generate(char *buff, int buff_sz, const nmeaINFO *info, int generate_mask);

int nmea_gen_GPGGA(char *s, int len, nmeaGPGGA *pack);
int nmea_gen_GPGSA(char *s, int len, nmeaGPGSA *pack);
int nmea_gen_GPGSV(char *s, int len, nmeaGPGSV *pack);
int nmea_gen_GPRMC(char *s, int len, nmeaGPRMC *pack);
int nmea_gen_GPVTG(char *s, int len, nmeaGPVTG *pack);

void nmea_info2GPGGA(const nmeaINFO *info, nmeaGPGGA *pack);
void nmea_info2GPGSA(const nmeaINFO *info, nmeaGPGSA *pack);
void nmea_info2GPGSV(const nmeaINFO *info, nmeaGPGSV *pack, int pack_idx);
void nmea_info2GPRMC(const nmeaINFO *info, nmeaGPRMC *pack);
void nmea_info2GPVTG(const nmeaINFO *info, nmeaGPVTG *pack);

#ifdef  __cplusplus
}
#endif

#endif /* __NMEA_GENERATE_H__ */

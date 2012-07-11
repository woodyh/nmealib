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

#include <nmea/info.h>

#include <nmea/gmath.h>
#include <nmea/generate.h>

#include <stdio.h>
#include <unistd.h>

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	nmeaINFO info;
	char buff[2048];
	int gen_sz;
	int it;

	nmea_zero_INFO(&info);

	info.sig = 3;
	info.fix = 3;
	info.lat = 5000.0;
	info.lon = 3600.0;
	info.speed = 2.14 * NMEA_TUS_MS;
	info.elv = 10.86;
	info.track = 45;
	info.magvar = 55;
	info.HDOP = 2.3;
	info.VDOP = 1.2;
	info.PDOP = 2.594224354;

	nmea_INFO_set_present(&info, SIG);
	nmea_INFO_set_present(&info, FIX);
	nmea_INFO_set_present(&info, LAT);
	nmea_INFO_set_present(&info, LON);
	nmea_INFO_set_present(&info, SPEED);
	nmea_INFO_set_present(&info, ELV);
	nmea_INFO_set_present(&info, TRACK);
	nmea_INFO_set_present(&info, MAGVAR);
	nmea_INFO_set_present(&info, HDOP);
	nmea_INFO_set_present(&info, VDOP);
	nmea_INFO_set_present(&info, PDOP);

	info.satinfo.inuse = NMEA_MAXSAT;
	nmea_INFO_set_present(&info, SATINUSECOUNT);
	for (it = 0; it < NMEA_MAXSAT; it++) {
		info.satinfo.in_use[it] = it + 1;
	}
	nmea_INFO_set_present(&info, SATINUSE);

	info.satinfo.inview = 1;
	for (it = 0; it < NMEA_MAXSAT; it++) {
		info.satinfo.sat[0].id = it + 1;
		info.satinfo.sat[0].elv = (it * 10);
		info.satinfo.sat[0].azimuth = it + 1;
		info.satinfo.sat[0].sig = 99 - it;
	}
	nmea_INFO_set_present(&info, SATINVIEW);

	for (it = 0; it < 10; it++) {
		gen_sz = nmea_generate(&buff[0], 2048, &info, GPGGA | GPGSA | GPGSV | GPRMC | GPVTG);

		buff[gen_sz] = 0;
		printf("%s\n", &buff[0]);

		usleep(500000);

		info.speed += .1;
	}

	return 0;
}

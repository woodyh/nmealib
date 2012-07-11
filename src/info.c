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

#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

/**
 * Reset the time to now
 *
 * @param utc a pointer to the time structure
 * @param present a pointer to a present field. when non-NULL then the UTCDATE
 * and UTCTIME flags are set in it.
 */
void nmea_time_now(nmeaTIME *utc, uint32_t * present) {
	struct timeval tp;
	struct tm tt;

	assert(utc);

	gettimeofday(&tp, NULL );
	gmtime_r(&tp.tv_sec, &tt);

	utc->year = tt.tm_year;
	utc->mon = tt.tm_mon;
	utc->day = tt.tm_mday;
	utc->hour = tt.tm_hour;
	utc->min = tt.tm_min;
	utc->sec = tt.tm_sec;
	utc->hsec = (tp.tv_usec / 10000);
	if (present) {
		*present |= (UTCDATE | UTCTIME);
	}
}

/**
 * Clear an info structure.
 * Resets the time to now, sets up the signal as BAD, the FIX as BAD, and
 * signals presence of these fields.
 * Resets all other fields to 0.
 *
 * @param info a pointer to the structure
 */
void nmea_zero_INFO(nmeaINFO *info) {
	if (!info) {
		return;
	}

	memset(info, 0, sizeof(nmeaINFO));
	nmea_time_now(&info->utc, &info->present);

	info->sig = NMEA_SIG_BAD;
	nmea_INFO_set_present(info, SIG);

	info->fix = NMEA_FIX_BAD;
	nmea_INFO_set_present(info, FIX);
}

/**
 * Sanitise the NMEA info, make sure that:
 * - latitude is in the range [-9000, 9000],
 * - longitude is in the range [-18000, 18000],
 * - DOPs are positive,
 * - speed is positive,
 * - track is in the range [0, 360>.
 *
 * Time is set to the current time when not present.
 *
 * When a field is not present then it is reset to its default (NMEA_SIG_BAD,
 * NMEA_FIX_BAD, 0).
 *
 * Satinfo is not touched.
 *
 * @param nmeaInfo
 * the NMEA info structure to sanitise
 */
void nmea_INFO_sanitise(nmeaINFO *nmeaInfo) {
	double lat = 0;
	double lon = 0;
	double speed = 0;
	double track = 0;
	double mtrack = 0;
	bool latAdjusted = false;
	bool lonAdjusted = false;
	bool speedAdjusted = false;
	bool trackAdjusted = false;
	bool mtrackAdjusted = false;
	nmeaTIME utc;

	if (!nmeaInfo) {
		return;
	}

	nmeaInfo->present = nmeaInfo->present & NMEA_INFO_PRESENT_MASK;

	if (!nmea_INFO_is_present(nmeaInfo, UTCDATE) || !nmea_INFO_is_present(nmeaInfo, UTCTIME)) {
		nmea_time_now(&utc, NULL);
	}

	if (!nmea_INFO_is_present(nmeaInfo, UTCDATE)) {
		nmeaInfo->utc.year = utc.year;
		nmeaInfo->utc.mon = utc.mon;
		nmeaInfo->utc.day = utc.day;
	}

	if (!nmea_INFO_is_present(nmeaInfo, UTCTIME)) {
		nmeaInfo->utc.hour = utc.hour;
		nmeaInfo->utc.min = utc.min;
		nmeaInfo->utc.sec = utc.sec;
		nmeaInfo->utc.hsec = utc.hsec;
	}

	if (!nmea_INFO_is_present(nmeaInfo, SIG)) {
		nmeaInfo->sig = NMEA_SIG_BAD;
	}

	if (!nmea_INFO_is_present(nmeaInfo, FIX)) {
		nmeaInfo->fix = NMEA_FIX_BAD;
	}

	if (!nmea_INFO_is_present(nmeaInfo, PDOP)) {
		nmeaInfo->PDOP = 0;
	} else {
		nmeaInfo->PDOP = fabs(nmeaInfo->PDOP);
	}

	if (!nmea_INFO_is_present(nmeaInfo, HDOP)) {
		nmeaInfo->HDOP = 0;
	} else {
		nmeaInfo->HDOP = fabs(nmeaInfo->HDOP);
	}

	if (!nmea_INFO_is_present(nmeaInfo, VDOP)) {
		nmeaInfo->VDOP = 0;
	} else {
		nmeaInfo->VDOP = fabs(nmeaInfo->VDOP);
	}

	if (!nmea_INFO_is_present(nmeaInfo, LAT)) {
		nmeaInfo->lat = 0;
	}

	if (!nmea_INFO_is_present(nmeaInfo, LON)) {
		nmeaInfo->lon = 0;
	}

	if (!nmea_INFO_is_present(nmeaInfo, ELV)) {
		nmeaInfo->elv = 0;
	}

	if (!nmea_INFO_is_present(nmeaInfo, SPEED)) {
		nmeaInfo->speed = 0;
	}

	if (!nmea_INFO_is_present(nmeaInfo, TRACK)) {
		nmeaInfo->track = 0;
	}

	if (!nmea_INFO_is_present(nmeaInfo, MTRACK)) {
		nmeaInfo->mtrack = 0;
	}

	if (!nmea_INFO_is_present(nmeaInfo, MAGVAR)) {
		nmeaInfo->magvar = 0;
	}

	/* satinfo is not used */

	/*
	 * lat
	 */

	lat = nmeaInfo->lat;
	lon = nmeaInfo->lon;

	/* force lat in [-18000, 18000] */
	while (lat < -18000.0) {
		lat += 36000.0;
		latAdjusted = true;
	}
	while (lat > 18000.0) {
		lat -= 36000.0;
		latAdjusted = true;
	}

	/* lat is now in [-18000, 18000] */

	/* force lat from <9000, 18000] in [9000, 0] */
	if (lat > 9000.0) {
		lat = 18000.0 - lat;
		lon += 18000.0;
		latAdjusted = true;
		lonAdjusted = true;
	}

	/* force lat from [-18000, -9000> in [0, -9000] */
	if (lat < -9000.0) {
		lat = -18000.0 - lat;
		lon += 18000.0;
		latAdjusted = true;
		lonAdjusted = true;
	}

	/* lat is now in [-9000, 9000] */

	if (latAdjusted) {
		nmeaInfo->lat = lat;
	}

	/*
	 * lon
	 */

	/* force lon in [-18000, 18000] */
	while (lon < -18000.0) {
		lon += 36000.0;
		lonAdjusted = true;
	}
	while (lon > 18000.0) {
		lon -= 36000.0;
		lonAdjusted = true;
	}

	/* lon is now in [-18000, 18000] */

	if (lonAdjusted) {
		nmeaInfo->lon = lon;
	}

	/*
	 * speed
	 */

	speed = nmeaInfo->speed;
	track = nmeaInfo->track;
	mtrack = nmeaInfo->mtrack;

	if (speed < 0.0) {
		speed = -speed;
		track += 180.0;
		mtrack += 180.0;
		speedAdjusted = true;
		trackAdjusted = true;
	}

	/* speed is now in [0, max> */

	if (speedAdjusted) {
		nmeaInfo->speed = speed;
	}

	/*
	 * track
	 */

	/* force track in [0, 360> */
	while (track < 0.0) {
		track += 360.0;
		trackAdjusted = true;
	}
	while (track >= 360.0) {
		track -= 360.0;
		trackAdjusted = true;
	}

	/* track is now in [0, 360> */

	if (trackAdjusted) {
		nmeaInfo->track = track;
	}

	/*
	 * mtrack
	 */

	/* force mtrack in [0, 360> */
	while (mtrack < 0.0) {
		mtrack += 360.0;
		mtrackAdjusted = true;
	}
	while (mtrack >= 360.0) {
		mtrack -= 360.0;
		mtrackAdjusted = true;
	}

	/* mtrack is now in [0, 360> */

	if (mtrackAdjusted) {
		nmeaInfo->mtrack = mtrack;
	}
}

/**
 * Converts the position fields to degrees and DOP fields to meters so that
 * all fields use normal metric units.
 *
 * @param nmeaInfo
 * the nmeaINFO
 */
void nmea_INFO_unit_conversion(nmeaINFO * nmeaInfo) {
	if (!nmeaInfo) {
		return;
	}

	/* smask (already in correct format) */

	/* utc (already in correct format) */

	/* sig (already in correct format) */
	/* fix (already in correct format) */

	if (nmea_INFO_is_present(nmeaInfo, PDOP)) {
		nmeaInfo->PDOP = nmea_dop2meters(nmeaInfo->PDOP);
	}

	if (nmea_INFO_is_present(nmeaInfo, HDOP)) {
		nmeaInfo->HDOP = nmea_dop2meters(nmeaInfo->HDOP);
	}

	if (nmea_INFO_is_present(nmeaInfo, VDOP)) {
		nmeaInfo->VDOP = nmea_dop2meters(nmeaInfo->VDOP);
	}

	if (nmea_INFO_is_present(nmeaInfo, LAT)) {
		nmeaInfo->lat = nmea_ndeg2degree(nmeaInfo->lat);
	}

	if (nmea_INFO_is_present(nmeaInfo, LON)) {
		nmeaInfo->lon = nmea_ndeg2degree(nmeaInfo->lon);
	}

	/* elv (already in correct format) */
	/* speed (already in correct format) */
	/* track (already in correct format) */
	/* declination (already in correct format) */

	/* satinfo (not used) */
}

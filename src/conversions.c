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

#include <nmea/conversions.h>

#include <nmea/gmath.h>

#include <assert.h>
#include <string.h>
#include <math.h>

/**
 * Determine the number of GSV sentences needed for a number of sats
 *
 * @param sats the number of sats
 * @return the number of GSV sentences needed
 */
int nmea_gsv_npack(int sats) {
	int pack_count = sats / NMEA_SATINPACK;

	if ((sats % NMEA_SATINPACK) > 0)
		pack_count++;

	if (!pack_count)
		pack_count++;

	return pack_count;
}

/**
 * Fill nmeaINFO structure from GGA packet structure
 *
 * @param pack a pointer to the packet structure
 * @param info a pointer to the nmeaINFO structure
 */
void nmea_GPGGA2info(const nmeaGPGGA *pack, nmeaINFO *info) {
	assert(pack);
	assert(info);

	info->present |= pack->present;
	nmea_INFO_set_present(info, SMASK);
	info->smask |= GPGGA;
	if (nmea_INFO_is_present(pack, UTCTIME)) {
		info->utc.hour = pack->utc.hour;
		info->utc.min = pack->utc.min;
		info->utc.sec = pack->utc.sec;
		info->utc.hsec = pack->utc.hsec;
	}
	if (nmea_INFO_is_present(pack, LAT)) {
		info->lat = ((pack->ns == 'N') ? pack->lat : -pack->lat);
	}
	if (nmea_INFO_is_present(pack, LON)) {
		info->lon = ((pack->ew == 'E') ? pack->lon : -pack->lon);
	}
	if (nmea_INFO_is_present(pack, SIG)) {
		info->sig = pack->sig;
	}
	if (nmea_INFO_is_present(pack, HDOP)) {
		info->HDOP = pack->HDOP;
	}
	if (nmea_INFO_is_present(pack, ELV)) {
		info->elv = pack->elv;
	}
	/* ignore diff and diff_units */
	/* ignore dgps_age and dgps_sid */
}

/**
 * Convert an nmeaINFO structure into an nmeaGPGGA structure
 *
 * @param info a pointer to the nmeaINFO structure
 * @param pack a pointer to the nmeaGPGGA structure
 */
void nmea_info2GPGGA(const nmeaINFO *info, nmeaGPGGA *pack) {
	nmea_zero_GPGGA(pack);

	pack->present = info->present;
	nmea_INFO_unset_present(pack, SMASK);
	if (nmea_INFO_is_present(info, UTCTIME)) {
		pack->utc.hour = info->utc.hour;
		pack->utc.min = info->utc.min;
		pack->utc.sec = info->utc.sec;
		pack->utc.hsec = info->utc.hsec;
	}
	if (nmea_INFO_is_present(info, LAT)) {
		pack->lat = fabs(info->lat);
		pack->ns = ((info->lat > 0) ? 'N' : 'S');
	}
	if (nmea_INFO_is_present(info, LON)) {
		pack->lon = fabs(info->lon);
		pack->ew = ((info->lon > 0) ? 'E' : 'W');
	}
	if (nmea_INFO_is_present(info, SIG)) {
		pack->sig = info->sig;
	}
	if (nmea_INFO_is_present(info, SATINUSE)) {
		pack->satinuse = info->satinfo.inuse;
	}
	if (nmea_INFO_is_present(info, HDOP)) {
		pack->HDOP = info->HDOP;
	}
	if (nmea_INFO_is_present(info, ELV)) {
		pack->elv = info->elv;
		pack->elv_units = 'M';
	}
	/* defaults for (ignored) diff and diff_units */
	pack->diff = 0;
	pack->diff_units = 'M';
	/* defaults for (ignored) dgps_age and dgps_sid */
	pack->dgps_age = 0;
	pack->dgps_sid = 0;
}

/**
 * Fill nmeaINFO structure from GSA packet structure
 *
 * @param pack a pointer to the packet structure
 * @param info a pointer to the nmeaINFO structure
 */
void nmea_GPGSA2info(const nmeaGPGSA *pack, nmeaINFO *info) {
	int i = 0;

	assert(pack);
	assert(info);

	info->present |= pack->present;
	nmea_INFO_set_present(info, SMASK);
	info->smask |= GPGSA;
	/* fix_mode is ignored */
	if (nmea_INFO_is_present(pack, FIX)) {
		info->fix = pack->fix_type;
	}
	if (nmea_INFO_is_present(pack, SATINUSE)) {
		assert(sizeof(info->satinfo.in_use) == sizeof(info->satinfo.in_use));
		memcpy(info->satinfo.in_use, pack->sat_prn, sizeof(info->satinfo.in_use));
		for (i = 0; i < NMEA_MAXSAT; ++i) {
			if (pack->sat_prn[i]) {
				info->satinfo.inuse++;
			}
		}
	}
	if (nmea_INFO_is_present(pack, PDOP)) {
		info->PDOP = pack->PDOP;
	}
	if (nmea_INFO_is_present(pack, HDOP)) {
		info->HDOP = pack->HDOP;
	}
	if (nmea_INFO_is_present(pack, VDOP)) {
		info->VDOP = pack->VDOP;
	}
}

/**
 * Convert an nmeaINFO structure into an nmeaGPGSA structure
 *
 * @param info a pointer to the nmeaINFO structure
 * @param pack a pointer to the nmeaGPGSA structure
 */
void nmea_info2GPGSA(const nmeaINFO *info, nmeaGPGSA *pack) {
	nmea_zero_GPGSA(pack);

	pack->present = info->present;
	nmea_INFO_unset_present(pack, SMASK);
	pack->fix_mode = 'A';
	pack->fix_type = info->fix;
	memcpy(pack->sat_prn, info->satinfo.in_use, sizeof(pack->sat_prn));
	pack->PDOP = info->PDOP;
	pack->HDOP = info->HDOP;
	pack->VDOP = info->VDOP;
}

/**
 * Fill nmeaINFO structure from GSV packet structure
 *
 * @param pack a pointer to the packet structure
 * @param info a pointer to the nmeaINFO structure
 */
void nmea_GPGSV2info(const nmeaGPGSV *pack, nmeaINFO *info) {
	int sat_offset;
	int sat_count;
	int sat_index;
	int pack_index;

	assert(pack);
	assert(info);

	if ((pack->pack_index > pack->pack_count) || ((pack->pack_index * NMEA_SATINPACK) > NMEA_MAXSAT))
		return;

	pack_index = pack->pack_index;
	if (pack_index < 1)
		pack_index = 1;

	info->present |= pack->present;
	nmea_INFO_set_present(info, SMASK);
	info->smask |= GPGSV;
	if (nmea_INFO_is_present(pack, SATINVIEW)) {
		info->satinfo.inview = pack->sat_count;

		/* index of 1st sat in pack */
		sat_offset = (pack_index - 1) * NMEA_SATINPACK;
		/* the number of sats in this sentence */
		sat_count = ((sat_offset + NMEA_SATINPACK) > pack->sat_count) ? (pack->sat_count - sat_offset) : NMEA_SATINPACK;

		for (sat_index = 0; sat_index < sat_count; sat_index++) {
			info->satinfo.sat[sat_offset + sat_index].id = pack->sat_data[sat_index].id;
			info->satinfo.sat[sat_offset + sat_index].elv = pack->sat_data[sat_index].elv;
			info->satinfo.sat[sat_offset + sat_index].azimuth = pack->sat_data[sat_index].azimuth;
			info->satinfo.sat[sat_offset + sat_index].sig = pack->sat_data[sat_index].sig;
		}
	}
}

/**
 * Convert an nmeaINFO structure into an nmeaGPGSV structure
 *
 * @param info a pointer to the nmeaINFO structure
 * @param pack a pointer to the nmeaGPGSV structure
 * @param pack_idx pack index (zero based)
 */
void nmea_info2GPGSV(const nmeaINFO *info, nmeaGPGSV *pack, int pack_idx) {
	int sit, pit;

	nmea_zero_GPGSV(pack);

	pack->present = info->present;
	nmea_INFO_unset_present(pack, SMASK);
	pack->sat_count = (info->satinfo.inview <= NMEA_MAXSAT) ? info->satinfo.inview : NMEA_MAXSAT;
	pack->pack_count = nmea_gsv_npack(pack->sat_count);

	if (pack_idx >= pack->pack_count)
		pack->pack_index = pack_idx % pack->pack_count;
	else
		pack->pack_index = pack_idx;

	for (pit = 0, sit = pack->pack_index * NMEA_SATINPACK; pit < NMEA_SATINPACK; ++pit, ++sit)
		pack->sat_data[pit] = info->satinfo.sat[sit];
}

/**
 * Fill nmeaINFO structure from RMC packet structure
 *
 * @param pack a pointer to the packet structure
 * @param info a pointer to the nmeaINFO structure
 */
void nmea_GPRMC2info(const nmeaGPRMC *pack, nmeaINFO *info) {
	assert(pack);
	assert(info);

	info->present |= pack->present;
	nmea_INFO_set_present(info, SMASK);
	info->smask |= GPRMC;
	if (nmea_INFO_is_present(pack, UTCDATE)) {
		info->utc.year = pack->utc.year;
		info->utc.mon = pack->utc.mon;
		info->utc.day = pack->utc.day;
	}
	if (nmea_INFO_is_present(pack, UTCTIME)) {
		info->utc.hour = pack->utc.hour;
		info->utc.min = pack->utc.min;
		info->utc.sec = pack->utc.sec;
		info->utc.hsec = pack->utc.hsec;
	}
	if (pack->status == 'A') {
		if (info->sig == NMEA_SIG_BAD) {
			info->sig = NMEA_SIG_MID;
		}
		if (info->fix == NMEA_FIX_BAD) {
			info->fix = NMEA_FIX_2D;
		}
	} else {
		info->sig = NMEA_SIG_BAD;
		info->fix = NMEA_FIX_BAD;
	}
	if (nmea_INFO_is_present(pack, LAT)) {
		info->lat = ((pack->ns == 'N') ? pack->lat : -pack->lat);
	}
	if (nmea_INFO_is_present(pack, LON)) {
		info->lon = ((pack->ew == 'E') ? pack->lon : -pack->lon);
	}
	if (nmea_INFO_is_present(pack, SPEED)) {
		info->speed = pack->speed * NMEA_TUD_KNOTS;
	}
	if (nmea_INFO_is_present(pack, TRACK)) {
		info->track = pack->track;
	}
	if (nmea_INFO_is_present(pack, MAGVAR)) {
		info->magvar = ((pack->magvar_ew == 'E') ? pack->magvar : -pack->magvar);
	}
}

/**
 * Convert an nmeaINFO structure into an nmeaGPRMC structure
 *
 * @param info a pointer to the nmeaINFO structure
 * @param pack a pointer to the nmeaGPRMC structure
 */
void nmea_info2GPRMC(const nmeaINFO *info, nmeaGPRMC *pack) {
	nmea_zero_GPRMC(pack);

	pack->present = info->present;
	nmea_INFO_unset_present(pack, SMASK);
	pack->utc = info->utc;
	pack->status = ((info->sig > 0) ? 'A' : 'V');
	pack->lat = fabs(info->lat);
	pack->ns = ((info->lat > 0) ? 'N' : 'S');
	pack->lon = fabs(info->lon);
	pack->ew = ((info->lon > 0) ? 'E' : 'W');
	pack->speed = info->speed / NMEA_TUD_KNOTS;
	pack->track = info->track;
	pack->magvar = fabs(info->magvar);
	pack->magvar_ew = ((info->magvar > 0) ? 'E' : 'W');
	pack->mode = ((info->sig > 0) ? 'A' : 'N');
}

/**
 * Fill nmeaINFO structure from VTG packet structure
 *
 * @param pack a pointer to the packet structure
 * @param info a pointer to the nmeaINFO structure
 */
void nmea_GPVTG2info(const nmeaGPVTG *pack, nmeaINFO *info) {
	assert(pack);
	assert(info);

	info->present |= pack->present;
	nmea_INFO_set_present(info, SMASK);
	info->smask |= GPVTG;
	if (nmea_INFO_is_present(pack, SPEED)) {
		info->speed = pack->spk;
	}
	if (nmea_INFO_is_present(pack, TRACK)) {
		info->track = pack->track;
	}
	if (nmea_INFO_is_present(pack, MTRACK)) {
		info->mtrack = pack->mtrack;
	}
}

/**
 * Convert an nmeaINFO structure into an nmeaGPVTG structure
 *
 * @param info a pointer to the nmeaINFO structure
 * @param pack a pointer to the nmeaGPRMC structure
 */
void nmea_info2GPVTG(const nmeaINFO *info, nmeaGPVTG *pack) {
	nmea_zero_GPVTG(pack);

	pack->present = info->present;
	nmea_INFO_unset_present(pack, SMASK);
	pack->track = info->track;
	pack->track_t = 'T';
	pack->mtrack = info->mtrack;
	pack->mtrack_m = 'M';
	pack->spn = info->speed / NMEA_TUD_KNOTS;
	pack->spn_n = 'N';
	pack->spk = info->speed;
	pack->spk_k = 'K';
}

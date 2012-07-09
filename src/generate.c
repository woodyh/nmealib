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

#include <nmea/generate.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <nmea/tok.h>
#include <nmea/units.h>

/**
 * Generate a GPGGA sentence from an nmeaGPGGA structure
 *
 * @param s a pointer to the buffer to generate the string in
 * @param len the size of the buffer
 * @param pack the structure
 * @return the length of the generated sentence
 */
int nmea_gen_GPGGA(char *s, int len, nmeaGPGGA *pack) {
	char sTime[16];
	char sLat[16];
	char sNs[2];
	char sLon[16];
	char sEw[2];
	char sSig[4];
	char sHdop[16];
	char sElv[16];
	char sEmpty[2];

	sTime[0] = 0;
	sLat[0] = 0;
	sNs[0] = sNs[1] = 0;
	sLon[0] = 0;
	sEw[0] = sEw[1] = 0;
	sSig[0] = 0;
	sHdop[0] = 0;
	sElv[0] = 0;
	sEmpty[0] = '0';
	sEmpty[1] = 0;

	if (nmea_INFO_is_present(pack, UTCTIME)) {
		snprintf(&sTime[0], sizeof(sTime), "%02d%02d%02d.%02d", pack->utc.hour, pack->utc.min, pack->utc.sec,
				pack->utc.hsec);
	}
	if (nmea_INFO_is_present(pack, LAT)) {
		snprintf(&sLat[0], sizeof(sLat), "%09.4f", pack->lat);
		sNs[0] = pack->ns;
	}
	if (nmea_INFO_is_present(pack, LON)) {
		snprintf(&sLon[0], sizeof(sLon), "%010.4f", pack->lon);
		sEw[0] = pack->ew;
	}
	if (nmea_INFO_is_present(pack, SIG)) {
		snprintf(&sSig[0], sizeof(sSig), "%1d", pack->sig);
	}
	if (nmea_INFO_is_present(pack, HDOP)) {
		snprintf(&sHdop[0], sizeof(sHdop), "%03.1f", pack->HDOP);
	}
	if (nmea_INFO_is_present(pack, ELV)) {
		snprintf(&sElv[0], sizeof(sElv), "%03.1f", pack->elv);
	}

	return nmea_printf(s, len, "$GPGGA,%s,%s,%s,%s,%s,%s,%02d,%s,%s,%C,%s,%C,%s,%s", &sTime[0], &sLat[0], &sNs[0],
			&sLon[0], &sEw[0], &sSig[0], pack->satinuse, &sHdop[0], &sElv[0], pack->elv_units, &sEmpty[0], 'M',
			&sEmpty[0], &sEmpty[0]);
}

/**
 * Generate a GPGSA sentence from an nmeaGPGSA structure
 *
 * @param s a pointer to the buffer to generate the string in
 * @param len the size of the buffer
 * @param pack the structure
 * @return the length of the generated sentence
 */
int nmea_gen_GPGSA(char *s, int len, nmeaGPGSA *pack) {
	int i;
	char sFixMode[2];
	char sFixType[2];
	char sSatPrn[64];
	char sPdop[16];
	char sHdop[16];
	char sVdop[16];
	char * psSatPrn = &sSatPrn[0];
	int ssSatPrn = sizeof(sSatPrn);
	bool satinuse = nmea_INFO_is_present(pack, SATINUSE);

	sFixMode[0] = sFixMode[1] = 0;
	sFixType[0] = sFixType[1] = 0;
	sSatPrn[0] = 0;
	sPdop[0] = 0;
	sHdop[0] = 0;
	sVdop[0] = 0;

	if (nmea_INFO_is_present(pack, FIX)) {
		sFixMode[0] = pack->fix_mode;
		snprintf(&sFixType[0], sizeof(sFixType), "%1d", pack->fix_type);
	}

	for (i = 0; i < NMEA_MAXSAT; i++) {
		int cnt = 0;
		if (satinuse && pack->sat_prn[i]) {
			cnt = snprintf(psSatPrn, ssSatPrn, "%d", pack->sat_prn[i]);
			ssSatPrn -= cnt;
			psSatPrn += cnt;
		}
		if (i < (NMEA_MAXSAT - 1)) {
			*psSatPrn = ',';
			psSatPrn++;
			ssSatPrn--;
			*psSatPrn = '\0';
		}
	}

	if (nmea_INFO_is_present(pack, PDOP)) {
		snprintf(&sPdop[0], sizeof(sPdop), "%03.1f", pack->PDOP);
	}
	if (nmea_INFO_is_present(pack, HDOP)) {
		snprintf(&sHdop[0], sizeof(sHdop), "%03.1f", pack->HDOP);
	}
	if (nmea_INFO_is_present(pack, VDOP)) {
		snprintf(&sVdop[0], sizeof(sVdop), "%03.1f", pack->VDOP);
	}

	return nmea_printf(s, len, "$GPGSA,%s,%s,%s,%s,%s,%s", &sFixMode[0], &sFixType[0], &sSatPrn[0], &sPdop[0],
			&sHdop[0], &sVdop[0]);
}

/**
 * Generate a GPGSV sentence from an nmeaGPGSV structure
 *
 * @param s a pointer to the buffer to generate the string in
 * @param len the size of the buffer
 * @param pack the structure
 * @return the length of the generated sentence
 */
int nmea_gen_GPGSV(char *s, int len, nmeaGPGSV *pack) {
	return nmea_printf(s, len, "$GPGSV,%1d,%1d,%02d,"
			"%02d,%02d,%03d,%02d,"
			"%02d,%02d,%03d,%02d,"
			"%02d,%02d,%03d,%02d,"
			"%02d,%02d,%03d,%02d", pack->pack_count, pack->pack_index + 1, pack->sat_count, pack->sat_data[0].id,
			pack->sat_data[0].elv, pack->sat_data[0].azimuth, pack->sat_data[0].sig, pack->sat_data[1].id,
			pack->sat_data[1].elv, pack->sat_data[1].azimuth, pack->sat_data[1].sig, pack->sat_data[2].id,
			pack->sat_data[2].elv, pack->sat_data[2].azimuth, pack->sat_data[2].sig, pack->sat_data[3].id,
			pack->sat_data[3].elv, pack->sat_data[3].azimuth, pack->sat_data[3].sig);
}

/**
 * Generate a GPRMC sentence from an nmeaGPRMC structure
 *
 * @param s a pointer to the buffer to generate the string in
 * @param len the size of the buffer
 * @param pack the structure
 * @return the length of the generated sentence
 */
int nmea_gen_GPRMC(char *s, int len, nmeaGPRMC *pack) {
	char sTime[16];
	char sDate[16];
	char sLat[16];
	char sNs[2];
	char sLon[16];
	char sEw[2];
	char sSpeed[16];
	char sTrack[16];
	char sMagvar[16];
	char sMagvar_ew[2];
	char sMode[2];

	sTime[0] = 0;
	sDate[0] = 0;
	sLat[0] = 0;
	sNs[0] = sNs[1] = 0;
	sLon[0] = 0;
	sEw[0] = sEw[1] = 0;
	sSpeed[0] = 0;
	sTrack[0] = 0;
	sMagvar[0] = 0;
	sMagvar_ew[0] = sMagvar_ew[1] = 0;
	sMode[0] = sMode[1] = 0;

	if (nmea_INFO_is_present(pack, UTCDATE)) {
		snprintf(&sDate[0], sizeof(sDate), "%02d%02d%02d", pack->utc.day, pack->utc.mon + 1, pack->utc.year - 100);
	}
	if (nmea_INFO_is_present(pack, UTCTIME)) {
		snprintf(&sTime[0], sizeof(sTime), "%02d%02d%02d.%02d", pack->utc.hour, pack->utc.min, pack->utc.sec,
				pack->utc.hsec);
	}
	if (nmea_INFO_is_present(pack, LAT)) {
		snprintf(&sLat[0], sizeof(sLat), "%09.4f", pack->lat);
		sNs[0] = pack->ns;
	}
	if (nmea_INFO_is_present(pack, LON)) {
		snprintf(&sLon[0], sizeof(sLon), "%010.4f", pack->lon);
		sEw[0] = pack->ew;
	}
	if (nmea_INFO_is_present(pack, SPEED)) {
		snprintf(&sSpeed[0], sizeof(sSpeed), "%03.1f", pack->speed);
	}
	if (nmea_INFO_is_present(pack, TRACK)) {
		snprintf(&sTrack[0], sizeof(sTrack), "%03.1f", pack->track);
	}
	if (nmea_INFO_is_present(pack, MAGVAR)) {
		snprintf(&sMagvar[0], sizeof(sMagvar), "%03.1f", pack->magvar);
		sMagvar_ew[0] = pack->magvar_ew;
	}
	if (nmea_INFO_is_present(pack, SIG)) {
		sMode[0] = pack->mode;
	}

	return nmea_printf(s, len, "$GPRMC,%s,%C,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", &sTime[0], pack->status, &sLat[0], &sNs[0],
			&sLon[0], &sEw[0], &sSpeed[0], &sTrack[0], &sDate[0], &sMagvar[0], &sMagvar_ew[0], &sMode[0]);
}

/**
 * Generate a GPVTG sentence from an nmeaGPVTG structure
 *
 * @param s a pointer to the buffer to generate the string in
 * @param len the size of the buffer
 * @param pack the structure
 * @return the length of the generated sentence
 */
int nmea_gen_GPVTG(char *s, int len, nmeaGPVTG *pack) {
	char sTrackT[16];
	char sTrackM[16];
	char sSpeedN[16];
	char sSpeedK[16];
	char sUnitT[2];
	char sUnitM[2];
	char sUnitN[2];
	char sUnitK[2];

	sTrackT[0] = 0;
	sTrackM[0] = 0;
	sSpeedN[0] = 0;
	sSpeedK[0] = 0;
	sUnitT[0] = sUnitT[1] = 0;
	sUnitM[0] = sUnitM[1] = 0;
	sUnitN[0] = sUnitN[1] = 0;
	sUnitK[0] = sUnitK[1] = 0;

	if (nmea_INFO_is_present(pack, TRACK)) {
		snprintf(&sTrackT[0], sizeof(sTrackT), "%03.1f", pack->track);
		sUnitT[0] = 'T';
	}
	if (nmea_INFO_is_present(pack, MTRACK)) {
		snprintf(&sTrackM[0], sizeof(sTrackM), "%03.1f", pack->mtrack);
		sUnitM[0] = 'M';
	}
	if (nmea_INFO_is_present(pack, SPEED)) {
		snprintf(&sSpeedN[0], sizeof(sSpeedN), "%03.1f", pack->spn);
		sUnitN[0] = 'N';
		snprintf(&sSpeedK[0], sizeof(sSpeedK), "%03.1f", pack->spk);
		sUnitK[0] = 'K';
	}

	return nmea_printf(s, len, "$GPVTG,%s,%s,%s,%s,%s,%s,%s,%s", &sTrackT[0], &sUnitT[0], &sTrackM[0],
			&sUnitM[0], &sSpeedN[0], &sUnitN[0], &sSpeedK[0], &sUnitK[0]);
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
	pack->utc.hour = info->utc.hour;
	pack->utc.min = info->utc.min;
	pack->utc.sec = info->utc.sec;
	pack->utc.hsec = info->utc.hsec;
	pack->lat = fabs(info->lat);
	pack->ns = ((info->lat > 0) ? 'N' : 'S');
	pack->lon = fabs(info->lon);
	pack->ew = ((info->lon > 0) ? 'E' : 'W');
	pack->sig = info->sig;
	pack->satinuse = info->satinfo.inuse;
	pack->HDOP = info->HDOP;
	pack->elv = info->elv;
	pack->elv_units = 'M';
	pack->diff = 0;
	pack->diff_units = 'M';
	pack->dgps_age = 0;
	pack->dgps_sid = 0;
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

int nmea_gsv_npack(int sat_count) {
	int pack_count = lrint(ceil(((double) sat_count) / (double) NMEA_SATINPACK));

	if (0 == pack_count)
		pack_count = 1;

	return pack_count;
}

/**
 * Convert an nmeaINFO structure into an nmeaGPGSV structure
 *
 * @param info a pointer to the nmeaINFO structure
 * @param pack a pointer to the nmeaGPGSV structure
 * @param pack_idx pack index
 */
void nmea_info2GPGSV(const nmeaINFO *info, nmeaGPGSV *pack, int pack_idx) {
	int sit, pit;

	nmea_zero_GPGSV(pack);

	pack->sat_count = (info->satinfo.inview <= NMEA_MAXSAT) ? info->satinfo.inview : NMEA_MAXSAT;
	pack->pack_count = nmea_gsv_npack(pack->sat_count);

	if (pack->pack_count == 0)
		pack->pack_count = 1;

	if (pack_idx >= pack->pack_count)
		pack->pack_index = pack_idx % pack->pack_count;
	else
		pack->pack_index = pack_idx;

	for (pit = 0, sit = pack->pack_index * NMEA_SATINPACK; pit < NMEA_SATINPACK; ++pit, ++sit)
		pack->sat_data[pit] = info->satinfo.sat[sit];
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

int nmea_generate(char *buff, int buff_sz, const nmeaINFO *info, int generate_mask) {
	int gen_count = 0, gsv_it, gsv_count;
	int pack_mask = generate_mask;

	nmeaGPGGA gga;
	nmeaGPGSA gsa;
	nmeaGPGSV gsv;
	nmeaGPRMC rmc;
	nmeaGPVTG vtg;

	if (!buff)
		return 0;

	while (pack_mask) {
		if (pack_mask & GPGGA) {
			nmea_info2GPGGA(info, &gga);
			gen_count += nmea_gen_GPGGA(buff + gen_count, buff_sz - gen_count, &gga);
			pack_mask &= ~GPGGA;
		} else if (pack_mask & GPGSA) {
			nmea_info2GPGSA(info, &gsa);
			gen_count += nmea_gen_GPGSA(buff + gen_count, buff_sz - gen_count, &gsa);
			pack_mask &= ~GPGSA;
		} else if (pack_mask & GPGSV) {
			gsv_count = nmea_gsv_npack(info->satinfo.inview);
			for (gsv_it = 0; gsv_it < gsv_count && buff_sz - gen_count > 0; ++gsv_it) {
				nmea_info2GPGSV(info, &gsv, gsv_it);
				gen_count += nmea_gen_GPGSV(buff + gen_count, buff_sz - gen_count, &gsv);
			}
			pack_mask &= ~GPGSV;
		} else if (pack_mask & GPRMC) {
			nmea_info2GPRMC(info, &rmc);
			gen_count += nmea_gen_GPRMC(buff + gen_count, buff_sz - gen_count, &rmc);
			pack_mask &= ~GPRMC;
		} else if (pack_mask & GPVTG) {
			nmea_info2GPVTG(info, &vtg);
			gen_count += nmea_gen_GPVTG(buff + gen_count, buff_sz - gen_count, &vtg);
			pack_mask &= ~GPVTG;
		} else
			break;

		if (buff_sz - gen_count <= 0)
			break;
	}

	return gen_count;
}

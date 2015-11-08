/*
 * conf.h
 *
 *  Created on: 2 Nov 2015
 *      Author: woody
 */

#ifndef NMEALIB_INCLUDE_NMEA_NMEACONF_H_
#define NMEALIB_INCLUDE_NMEA_NMEACONF_H_

#define NMEA_DEBUG 1

#define NMEA_VALIDATE       1
#define NMEA_ERROR          0

#define NMEA_TIME_FORMAT    4

/** the default size for the temporary buffers */
#define NMEA_DEF_PARSEBUFF  128

/** the minimum size for the temporary buffers */
#define NMEA_MIN_PARSEBUFF  128

/** the size of the buffer to put time string (that is to be parsed) into */
#define NMEA_TIMEPARSE_BUF  32

/** number conversion buffer size */
#define NMEA_CONVSTR_BUF    32

#define SENTENCE_SIZE (128)

#if NMEA_DEBUG
  #include "ch.h"
  #define NMEA_ASSERT(x) chDbgAssert(x, "nmealib")
#else
  #define NMEA_ASSERT(x)
#endif

#endif /* NMEALIB_INCLUDE_NMEA_NMEACONF_H_ */

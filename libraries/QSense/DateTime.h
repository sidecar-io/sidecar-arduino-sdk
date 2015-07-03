#ifndef QSENSE_NET_DATETIME_H
#define QSENSE_NET_DATETIME_H

#if defined( ARDUINO )
#include "QSense.h"
#include "../StandardCplusplus/serstream"
#else
#include <QSense.h>
#include <iostream>
#endif

namespace qsense
{
  namespace net
  {
    /**
     * @brief Represents current date/time.  Seeds initially (and daily)
     * from a network time service, and uses internal timer to represent
     * a real-time clock.
     */
    class DateTime
    {
    public:
      /// Default constructor.  Use {@link #singleton} in general.
      DateTime();

      /// Return the current date/time in ISO 8601 format
      const qsense::QString currentTime();

      /// Return the current date in ISO 8601 format
      const qsense::QString date();

      /// Return the milli seconds since UNIX epoch.
      int64_t currentTimeMillis();

      /// Return a singleton instance to use.  This is the preferred way
      /// of using this class.
      static DateTime& singleton()
      {
        static DateTime dt;
        return dt;
      }

    private:
      void init();
      const qsense::QString serverTime();
      bool isLeapYear( int16_t year ) const;
      int64_t epochMilliSeconds( const qsense::QString& iso8601 ) const;
      qsense::QString isoTime( int64_t epoch ) const;

    private:
      static const int64_t milliSecondsPerHour;
      static const int64_t minEpoch;

      uint32_t startTimeMillis;
      uint32_t lastUpdateMillis;
      int64_t milliSecondsSinceEpoch;
    };
  }
}

#endif // QSENSE_NET_DATETIME_H

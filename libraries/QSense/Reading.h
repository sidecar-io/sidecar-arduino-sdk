/*
Copyright 2015 Sidecar
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef QSENSE_READING_H
#define QSENSE_READING_H

#if defined( ARDUINO )
#include "QSense.h"
#include "DateTime.h"
#include "../StandardCplusplus/ostream"
#else
#include <QSense.h>
#include <net/DateTime.h>
#include <ostream>
#endif

namespace qsense
{
  /**
   * @brief A class that represents a single reading.  Readings are
   * added to an {@link Event}.
   */
  class Reading
  {
  public:
    /**
     * @brief Create a new reading with specified values.
     * @param k The key to associate with the reading
     * @param v The value of the reading
     * @param ts The timestamp (optional) at which reading was taken.
     */
    Reading( const qsense::QString& k, const qsense::QString& v,
        const qsense::QString& ts = qsense::net::DateTime::singleton().currentTime() ) :
      key( k ), value( v ), timestamp( ts ) {}

    /**
     * @brief Create a new reading with specified values.
     * @param k The key to associate with the reading
     * @param v The float value of the reading
     * @param ts The timestamp (optional) at which reading was taken.
     */
    Reading( const qsense::QString& k, float v,
        const qsense::QString& ts = qsense::net::DateTime::singleton().currentTime() );

    /// Destructor.  No actions required.
    ~Reading() {}

    /// Return the key for the reading
    const qsense::QString& getKey() const { return key; }

    /// Return the value of the reading.
    const qsense::QString& getValue() const { return value; }

    /// Return the time at which the reading was taken.
    const qsense::QString& getTimestamp() const { return timestamp; }

    /// Return a JSON representation of the reading
    const qsense::QString toString() const;

  private:
    qsense::QString key;
    qsense::QString value;
    qsense::QString timestamp;
  };

  /// Serialise the reading as JSON to the output stream.
  std::ostream& operator << ( std::ostream& os, const qsense::Reading& reading );
}

#endif // QSENSE_READING_H

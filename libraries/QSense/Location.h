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
#ifndef QSENSE_LOCATION_H
#define QSENSE_LOCATION_H

#if defined( ARDUINO )
#include "QSense.h"
#include "../StandardCplusplus/ostream"
#else
#include <QSense.h>
#include <ostream>
#endif

namespace qsense
{
  /**
   * @brief A simple representation of geographical location.
   */
  class Location
  {
  public:
    /// Default constructor
    Location() : latitude( 0.0 ), longitude( 0.0 ) {}

    /**
     * @brief Create a new instance with the specified co-ordinates
     * @param lat The latitude
     * @param lon The longitude
     */
    Location( float lat, float lon ) : latitude( lat ), longitude( lon ) {}

    /// Copy constructor
    Location( const Location& location ) : latitude( location.latitude ),
        longitude( location.longitude ) {}

    /// Destructor.  No action required.
    ~Location() {}

    /// Copy assignment operator
    Location& operator = ( const Location& location );

    /// Serialise this instance to a JSON representation
    const qsense::QString toString() const;

    /// @return Return the latitude value
    float getLatitude() const { return latitude; }

    /// @return Return the longitude value
    float getLongitude() const { return longitude; }

  private:
    float latitude;
    float longitude;
  };

  /// Serialise the specified location to the output stream
  std::ostream& operator << ( std::ostream& os, const qsense::Location& location );
}

#endif // QSENSE_LOCATION_H

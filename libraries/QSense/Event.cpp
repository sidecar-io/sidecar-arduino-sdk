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
#include "Event.h"

#if defined( ARDUINO )
#include "UUID.h"
#include "DateTime.h"
#include "../StandardCplusplus/sstream"
#else
#include <UUID.h>
#include <net/DateTime.h>
#include <sstream>
#endif

namespace qsense
{
  namespace data
  {
    static qsense::QString deviceId;
    static qsense::QString stream;
    static qsense::Location location;
  }
}

using qsense::Event;
using qsense::Location;
using qsense::Reading;
using qsense::QString;


Event::Event() : readings(), tags(), location( qsense::data::location ) {}


Event::Event( const Location& loc ) : readings(), tags(), location( loc ) {}


Event& Event::add( const Reading& reading )
{
  readings.push_back( reading );
  return *this;
}


Event& Event::add( const QString& tag )
{
  tags.push_back( tag );
  return *this;
}


Event& Event::add( const QString& key, const QString& tag )
{
  keyTags[key].push_back( tag );
  return *this;
}


const QString Event::toString() const
{
  std::stringstream ss;
  ss << *this;
  return ss.str();
}


void Event::init( const QString& deviceId, const QString& stream,
    const qsense::Location& location )
{
  qsense::data::deviceId = deviceId;
  qsense::data::stream = stream;
  qsense::data::location = location;
}


std::ostream& qsense::operator << ( std::ostream& os, const Event& event )
{
  using qsense::UUID;
  using qsense::net::DateTime;

  os <<
    "{\"id\": \"" << UUID::create().toString() <<
    "\", \"deviceId\": \"" << qsense::data::deviceId <<
    "\", \"ts\": \"" << DateTime::singleton().currentTime() <<
    "\", \"stream\": \"" << qsense::data::stream <<
    "\", \"location\": " << event.getLocation() <<
    ", \"readings\": [";

  bool first = true;
  for ( Event::ReadingsIterator iter = event.beginReadings(); iter != event.endReadings(); ++iter )
  {
    if ( ! first ) os << ", ";
    os << *iter;
    first = false;
  }

  os << "]";

  if ( event.numberOfTags() > 0 )
  {
    os << ", ";
    first = true;
    os << "\"tags\": [";

    for ( Event::TagsIterator iter = event.beginTags(); iter != event.endTags(); ++iter )
    {
      if ( ! first ) os << ", ";
      os << "\"" << *iter << "\"";
      first = false;
    }

    os << "]";
  }

  if ( event.numberOfKeyTags() > 0 )
  {
    os << ", \"keyTags\": [";
    first = true;

    for ( Event::KeyTagsIterator iter = event.beginKeyTags(); iter != event.endKeyTags(); ++iter )
    {
      if ( ! first ) os << ", ";
      os << "{\"key\": \"" << iter->first << "\",";
      os << "\"tags\": [";

      bool tf = true;
      for ( Event::TagsIterator ti = iter->second.begin(); ti != iter->second.end(); ++ti )
      {
        if ( ! tf ) os << ", ";
        os << "\"" << *ti << "\"";
        tf = false;
      }

      os << "]}";
      first = false;
    }

    os << "]";
  }

  os << "}";
  return os;
}

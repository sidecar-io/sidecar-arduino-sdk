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

  if ( event.numberOfTags() > 0 )
  {
    os << "], \"tags\": [";
    first = true;

    for ( Event::TagsIterator iter = event.beginTags(); iter != event.endTags(); ++iter )
    {
      if ( ! first ) os << ", ";
      os << "\"" << *iter << "\"";
      first = false;
    }
  }

  os << "]}";
  return os;
}

#include "Location.h"

#if defined( ARDUINO )
#include "../StandardCplusplus/sstream"
#else
#include <string>
#include <sstream>
#endif

using qsense::Location;
using qsense::QString;

Location& Location::operator = ( const Location& location )
{
  latitude = location.latitude;
  longitude = location.longitude;
  return *this;
}

const QString Location::toString() const
{
  std::stringstream ss;
  ss << *this;
  return ss.str();
}


std::ostream& qsense::operator << ( std::ostream& os, const Location& location )
{
  os << "{\"lat\": " << location.getLatitude() <<
    ",\"lon\": " << location.getLongitude() << "}";
  return os;
}

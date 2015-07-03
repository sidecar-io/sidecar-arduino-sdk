#include "Reading.h"

#if defined( ARDUINO )
#include "../StandardCplusplus/sstream"
#else
#include <sstream>
#endif

using qsense::Reading;
using qsense::QString;


Reading::Reading( const QString& k, float v, const QString& ts ) :
  key( k ), timestamp( ts )
{
  std::stringstream ss;
  ss << v;
  value = ss.str();
}


const QString Reading::toString() const
{
  std::stringstream ss;
  ss << *this;
  return ss.str();
}


std::ostream& qsense::operator << ( std::ostream& os, const Reading& reading )
{
  os << "{\"key\": \"" << reading.getKey() <<
    "\", \"ts\": \"" << reading.getTimestamp() <<
    "\", \"value\": \"" << reading.getValue() << "\"}";
  return os;
}

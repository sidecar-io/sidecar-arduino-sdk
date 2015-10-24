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

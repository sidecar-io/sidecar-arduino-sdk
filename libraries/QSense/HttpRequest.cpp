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
#include "HttpRequest.h"

#if defined( ARDUINO )
#include "../StandardCplusplus/cstdio"
#include "../StandardCplusplus/sstream"
#else
#include <cstdio>
#include <sstream>
#endif

using qsense::QString;
using qsense::net::HttpRequest;

const QString HttpRequest::ILLEGAL = "%<>{}|\\\"^`";

HttpRequest::HttpRequest( const QString& path ) :
  uri( path ), body(), parameters(), headers() {}


HttpRequest& HttpRequest::setBody( const QString& txt )
{
  body = txt;
  return *this;
}


HttpRequest& HttpRequest::setParameter( const QString& key, const QString& value )
{
  Map::iterator iter = parameters.find( key );
  if ( iter != parameters.end() ) parameters.erase( iter );
  parameters.insert( std::pair<QString,QString>( key, value ) );
  return *this;
}


HttpRequest& HttpRequest::setHeader( const QString& key, const QString& value )
{
  Map::iterator iter = headers.find( key );
  if ( iter != headers.end() ) headers.erase( iter );
  headers.insert( std::pair<QString,QString>( key, value ) );
  return *this;
}


const QString HttpRequest::getParamters() const
{
  std::stringstream ss;

  bool first = true;
  for ( Iterator iter = parameters.begin(); iter != parameters.end(); ++iter )
  {
    if ( ! first ) ss << "&";
    ss << iter->first << "=" << encode( iter->second );
    first = false;
  }

  return ss.str();
}


void HttpRequest::toHex( unsigned value, int width, QString& output ) const
{
  char buffer[64];
  std::sprintf( buffer, "%0*X", width, value );
  output.append( buffer );
}


QString HttpRequest::encode( const QString& str ) const
{
  QString encodedStr;

  for ( QString::const_iterator it = str.begin(); it != str.end(); ++it )
  {
    char c = *it;
    if ( ( c >= 'a' && c <= 'z' ) ||
        ( c >= 'A' && c <= 'Z' ) ||
        ( c >= '0' && c <= '9' ) ||
        c == '-' || c == '_' ||
        c == '.' || c == '~' )
    {
      encodedStr += c;
    }
    else if ( c <= 0x20 || c >= 0x7F ||
      ILLEGAL.find(c) != QString::npos || '#' == c )
    {
      encodedStr += '%';
      toHex( (unsigned) (unsigned char) c, 2, encodedStr );
    }
    else encodedStr += c;
  }

  return encodedStr;
}

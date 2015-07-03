#include "DateTime.h"
#include "QHttpClient.h"

#if defined( ARDUINO )
#include <stdlib.h>
#include "../StandardCplusplus/sstream"
#else
#include <cstdlib>
#include <sstream>
#endif

using qsense::QString;
using qsense::net::DateTime;

const int64_t DateTime::minEpoch = int64_t( 1430704319000 );
const int64_t DateTime::milliSecondsPerHour = int64_t( 3600000 );


DateTime::DateTime() : startTimeMillis( millis() ),
  lastUpdateMillis( startTimeMillis ), milliSecondsSinceEpoch( 0 )
{
  init();
}


const QString DateTime::currentTime()
{
  return isoTime( currentTimeMillis() );
}


const QString DateTime::date()
{
  const QString& dt = currentTime();
  return dt.substr( 0, dt.find( "T" ) );
}


int64_t DateTime::currentTimeMillis()
{
  uint32_t time = millis();

  if ( ( time - startTimeMillis ) > 86400000UL ) init();
  else
  {
    milliSecondsSinceEpoch += time - lastUpdateMillis;
    lastUpdateMillis = time;
  }

  return milliSecondsSinceEpoch;
}


void DateTime::init()
{
  const QString& iso = serverTime();
  milliSecondsSinceEpoch = epochMilliSeconds( iso );
  if ( milliSecondsSinceEpoch < minEpoch )
  {
    uint32_t diff = uint32_t( minEpoch - milliSecondsSinceEpoch );
    std::cout << "minEpoch greater than server time.  Difference " << diff << std::endl;
    milliSecondsSinceEpoch = minEpoch;
  }

  uint32_t time = millis();
  milliSecondsSinceEpoch += time - lastUpdateMillis;
  startTimeMillis = time;
  lastUpdateMillis = time;
}


const qsense::QString DateTime::serverTime()
{
  using qsense::net::HttpClient;

  static QString server( "www.timeapi.org" );
  static QString uri( "/utc/now" );
  static QString userAgentKey( "User-Agent" );
  static QString userAgentValue( "QSense" );

  HttpClient* client = HttpClient::create();

  if ( client->connect( server ) )
  {
    std::cout << F( "Connected to " ) << server << std::endl;

    HttpClient::Headers headers;
    headers.insert( std::pair<QString,QString>( userAgentKey, userAgentValue ) );

    uint16_t responseCode = client->get( uri, headers );
    std::cout << F( "Server returned HTTP response code: " ) << responseCode << std::endl;

    if ( responseCode == 200 && client->connected() ) return client->readBody();
  }
  else
  {
    std::cout << F( "Connection to " ) << server << F( " failed" ) << std::endl;
  }

  delete client;
  return QString();
}


bool DateTime::isLeapYear( int16_t year ) const
{
  bool result = false;

  if ( ( year % 400 ) == 0 ) result = true;
  else if ( ( year % 100 ) == 0 ) result = false;
  else if ( ( year % 4 ) == 0 ) result = true;

  return result;
}


int64_t DateTime::epochMilliSeconds( const QString& date ) const
{
  const int16_t year = atoi( date.substr( 0, 4 ).c_str() );
  const int16_t month = atoi( date.substr( 5, 2 ).c_str() );
  const int16_t day = atoi( date.substr( 8, 2 ).c_str() );
  const int16_t hour = atoi( date.substr( 11, 2 ).c_str() );
  const int16_t minute = atoi( date.substr( 14, 2 ).c_str() );
  const int16_t second = atoi( date.substr( 17, 2 ).c_str() );
  const int16_t dst = atoi( date.substr( 20, 2 ).c_str() );
  const int16_t millis = 0;

  int64_t epoch = millis;
  epoch += second * int64_t( 1000 );
  epoch += minute * int64_t( 60000 );
  epoch += hour * milliSecondsPerHour;
  epoch += ( day - 1 ) * 24 * milliSecondsPerHour;

  const int8_t isLeap = isLeapYear( year );

  for ( int i = 1; i < month; ++i )
  {
    switch ( i )
    {
      case 2:
        epoch += ( (isLeap) ? 29 : 28 ) * 24 * milliSecondsPerHour;
        break;
      case 4:
      case 6:
      case 9:
      case 11:
        epoch += 30 * 24 * milliSecondsPerHour;
        break;
      default:
        epoch += 31 * 24 * milliSecondsPerHour;
    }
  }

  for ( int i = 1970; i < year; ++i )
  {
    if ( isLeapYear( i ) ) epoch += 366 * 24 * milliSecondsPerHour;
    else epoch += 365 * 24 * milliSecondsPerHour;
  }

  if ( dst ) epoch -= dst * milliSecondsPerHour;
  return epoch;
}


QString DateTime::isoTime( int64_t epoch ) const
{
  const int millis = epoch % int64_t( 1000 );
  epoch /= int64_t( 1000 );

  const int second = epoch % 60;

  epoch /= 60;
  const int minute = epoch % 60;

  epoch /= 60;
  const int hour = epoch % 24;
  epoch /= 24;
  int year = 1970;

  {
    int32_t days = 0;
    while ( ( days += ( isLeapYear( year ) ) ? 366 : 365 ) <= epoch ) ++year;

    days -= ( isLeapYear( year ) ) ? 366 : 365;
    epoch -= days;
  }

  uint8_t isLeap = isLeapYear( year );
  int month = 1;

  for ( ; month < 13; ++month )
  {
    int8_t length = 0;

    switch ( month )
    {
      case 2:
        length = isLeap ? 29 : 28;
        break;
      case 4:
      case 6:
      case 9:
      case 11:
        length = 30;
        break;
      default:
        length = 31;
    }

    if ( epoch >= length ) epoch -= length;
    else break;
  }

  const int day = epoch + 1;
  std::stringstream ss;
  ss << year << '-';

  if ( month < 10 ) ss << 0;
  ss << month << '-';

  if ( day < 10 ) ss << 0;
  ss << day << 'T';

  if ( hour < 10 ) ss << 0;
  ss << hour << ':';

  if ( minute < 10 ) ss << 0;
  ss << minute << ':';

  if ( second < 10 ) ss << 0;
  ss << second << '.';

  if ( millis < 10 ) ss << "00";
  else if ( millis < 100 ) ss << "0";
  ss << millis << 'Z';

  return ss.str();
}


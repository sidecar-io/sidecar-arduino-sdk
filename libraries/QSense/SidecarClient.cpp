#include "SidecarClient.h"
#include "DateTime.h"
#include "QHttpClient.h"

#if defined( ARDUINO )
#include "MD5.h"
#include "Sha1.h"
#include "../StandardCplusplus/sstream"
#else
#include <hash/MD5.h>
#include <hash/Sha1.h>
#include <sstream>
#endif

namespace qsense
{
  namespace net
  {
    namespace data
    {
      static QString apiKey;
      static QString apiSecret;
      static bool SidecarClientInitialised = false;
    }
  }
}

using qsense::QString;
using qsense::net::SidecarClient;


void SidecarClient::init( const QString& apiKey, const QString& apiSecret )
{
  if ( ! qsense::net::data::SidecarClientInitialised )
  {
    qsense::net::data::apiKey = apiKey;
    qsense::net::data::apiSecret = apiSecret;
    qsense::net::data::SidecarClientInitialised = true;
  }
}


bool SidecarClient::publish( const qsense::Event& event ) const
{
  using qsense::net::DateTime;
  using qsense::net::HttpClient;

  static QString server( "api.sidecar.io" );
  static QString method( "POST" );
  static QString uri( "/rest/v1/event" );

  bool flag = false;
  const QString& currentTime = DateTime::singleton().currentTime();

  HttpClient* client = HttpClient::create();

  if ( client->connect( server ) )
  {
    std::cout << F( "Connected to " ) << server << std::endl;

    const QString& eventJson = event.toString();
    const QString& hash = md5( eventJson );

    HttpClient::Headers headers;
    headers.insert( std::pair<QString,QString>( "Date", currentTime ) );
    headers.insert( std::pair<QString,QString>( "Content-Type", "application/json" ) );
    headers.insert( std::pair<QString,QString>( "Content-MD5", hash ) );
    headers.insert( std::pair<QString,QString>( "Signature-Version", "1" ) );

    {
      std::stringstream ss;
      ss << eventJson.length();
      headers.insert( std::pair<QString,QString>( "Content-Length", ss.str() ) );
    }

    QString auth( "SIDECAR " );
    auth.
      append( qsense::net::data::apiKey ).
      append( ":" ).
      append( signature( method, uri, currentTime, hash ).c_str() );
    headers.insert( std::pair<QString,QString>( "Authorization", auth ) );

    uint16_t responseCode = client->post( uri, headers, eventJson );
    std::cout << F( "Server returned HTTP response code: " ) << responseCode << std::endl;

    if ( responseCode == 202 ) flag = true;
  }
  else
  {
    std::cout << F( "Connection to " ) << server << F( " failed" ) << std::endl;
  }

  delete client;
  return flag;
}


QString SidecarClient::md5( const QString& event ) const
{
  using qsense::hash::MD5;
  MD5 md5;
  return md5.compute( event );
}


QString SidecarClient::signature( const QString& method, const QString& uri,
    const QString& date, const QString& hash ) const
{
  using qsense::hash::Sha1;
  Sha1 sha1;
  return sha1.sign( qsense::net::data::apiSecret, method, uri, date, hash );
}

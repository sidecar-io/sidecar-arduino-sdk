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
#include "SidecarClient.h"
#include "DateTime.h"
#include "QHttpClient.h"

#if defined( ARDUINO )
#include "MD5.h"
#include "Sha1.h"
#include "../StandardCplusplus/sstream"
#include <Ethernet.h>
#else
#include <hash/MD5.h>
#include <hash/Sha1.h>
#include <iostream>
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
      static bool SidecarClientAPIInitialised = false;

      static QString userKey;
      static QString userSecret;
      static bool SidecarClientUserInitialised = false;

      static const QString server( "api.sidecar.io" );
      static const QString POST( "POST" );
      static const QString DELETE( "DELETE" );

      const QString credentials( const QString& username, const QString& password )
      {
        std::stringstream ss;
        ss << "{\"username\":\"" << username <<
          "\",\"password\":\"" << password << "\"}";
        return ss.str();
      }
    }
  }
}

using qsense::QString;
using qsense::net::SidecarClient;


void SidecarClient::initAPIKey( const QString& apiKey, const QString& apiSecret )
{
  if ( ! qsense::net::data::SidecarClientAPIInitialised )
  {
    qsense::net::data::apiKey = apiKey;
    qsense::net::data::apiSecret = apiSecret;
    qsense::net::data::SidecarClientAPIInitialised = true;
  }
}


void SidecarClient::initUserKey( const QString& userKey, const QString& userSecret )
{
  if ( ! qsense::net::data::SidecarClientUserInitialised )
  {
    qsense::net::data::userKey = userKey;
    qsense::net::data::userSecret = userSecret;
    qsense::net::data::SidecarClientUserInitialised = true;
  }
}


const SidecarClient::UserResponse SidecarClient::UserResponse::create(
  uint16_t response, const qsense::QString& body )
{
  std::size_t start = body.find( "keyId\":\"" );
  std::size_t end = body.find( "\",\"" );
  const QString& keyId = body.substr( start + 8, end - ( start + 8 ) );

  start = body.find( "secret\":\"" );
  end = body.find( '"', start + 9 );
  const QString& secret = body.substr( start + 9, end - ( start + 9 ) );

#if defined( ARDUINO )
  return UserResponse( response, keyId, secret );
#else
  return std::move( UserResponse( response, keyId, secret ) );
#endif
}


SidecarClient::UserResponse SidecarClient::createUser(
  const QString& username, const QString& password )
{
  using qsense::net::DateTime;
  using qsense::net::HttpClient;
  using qsense::net::HttpRequest;

  static QString uri( "/rest/v1/provision/application/user/" );

  uint16_t responseCode = uint16_t( 500 );
  const QString& currentTime = DateTime::singleton().currentTime();

  HttpClient::Ptr client = HttpClient::create();

  if ( client->connect( data::server ) )
  {
#if DEBUG
    std::cout << F( "Connected to " ) << data::server << std::endl;
#endif

    const QString& json = data::credentials( username, password );
    const QString& hash = md5( json );

    HttpRequest request( uri );
    request.setHeader( "Date", currentTime );
    request.setHeader( "Content-Type", "application/json" );
    request.setHeader( "Content-MD5", hash );
    request.setHeader( "Signature-Version", "1" );

    {
      std::stringstream ss;
      ss << json.length();
      request.setHeader( "Content-Length", ss.str() );
    }

    std::stringstream ss;
    ss << "SIDECAR " <<
      qsense::net::data::apiKey <<
      ':' <<
      signature( qsense::net::data::apiSecret,
        data::POST, uri, currentTime, hash );

    request.setHeader( "Authorization", ss.str() );

    request.setBody( json );

    responseCode = client->post( request );
#if DEBUG
    std::cout << F( "User Provision API returned HTTP response code: " ) << responseCode << std::endl;
#endif

    if ( responseCode == 200 )
    {
      const QString& body = client->readBody();
#if defined( ARDUINO )
      return UserResponse::create( responseCode, body );
#else
      return std::move( UserResponse::create( responseCode, body ) );
#endif
    }
    else
    {
      while ( client->connected() )
      {
        std::cout << F( "  [resp] " ) << client->readLine() << std::endl;
      }
    }
  }

  return UserResponse( responseCode );
}


SidecarClient::UserResponse SidecarClient::createOrRetrieveAccessKeys(
  const QString& username, const QString& password )
{
  using qsense::net::DateTime;
  using qsense::net::HttpClient;
  using qsense::net::HttpRequest;

  static QString uri( "/rest/v1/provision/application/accesskey/" );

  uint16_t responseCode = uint16_t( 500 );
  const QString& currentTime = DateTime::singleton().currentTime();

  HttpClient::Ptr client = HttpClient::create();

  if ( client->connect( data::server ) )
  {
#if DEBUG
    std::cout << F( "Connected to " ) << data::server << std::endl;
#endif

    const QString& json = data::credentials( username, password );
    const QString& hash = md5( json );

    HttpRequest request( uri );
    request.setHeader( "Date", currentTime );
    request.setHeader( "Content-Type", "application/json" );
    request.setHeader( "Content-MD5", hash );
    request.setHeader( "Signature-Version", "1" );

    {
      std::stringstream ss;
      ss << json.length();
      request.setHeader( "Content-Length", ss.str() );
    }

    std::stringstream ss;
    ss << "SIDECAR " <<
      qsense::net::data::apiKey <<
      ':' <<
      signature( qsense::net::data::apiSecret, data::POST, uri, currentTime, hash );

    request.setHeader( "Authorization", ss.str() );

    request.setBody( json );

    responseCode = client->post( request );
#if DEBUG
    std::cout << F( "Create or Retrieve API returned HTTP response code: " ) << responseCode << std::endl;
#endif

    if ( responseCode == 200 )
    {
      const QString& body = client->readBody();
#if defined ( ARDUINO )
      return UserResponse::create( responseCode, body );
#else
      return std::move( UserResponse::create( responseCode, body ) );
#endif
    }
    else
    {
      while ( client->connected() )
      {
        std::cout << F( "  [resp] " ) << client->readLine() << std::endl;
      }
    }
  }

  return UserResponse( responseCode );
}


SidecarClient::UserResponse SidecarClient::authenticate(
  const QString& username, const QString& password )
{
  using qsense::net::DateTime;
  using qsense::net::HttpClient;
  using qsense::net::HttpRequest;

  static QString uri( "/rest/v1/provision/application/auth/" );

  uint16_t responseCode = uint16_t( 500 );
  const QString& currentTime = DateTime::singleton().currentTime();

  HttpClient::Ptr client = HttpClient::create();

  if ( client->connect( data::server ) )
  {
#if DEBUG
    std::cout << F( "Connected to " ) << data::server << std::endl;
#endif

    const QString& json = data::credentials( username, password );
    const QString& hash = md5( json );

    HttpRequest request( uri );
    request.setHeader( "Date", currentTime );
    request.setHeader( "Content-Type", "application/json" );
    request.setHeader( "Content-MD5", hash );
    request.setHeader( "Signature-Version", "1" );

    {
      std::stringstream ss;
      ss << json.length();
      request.setHeader( "Content-Length", ss.str() );
    }

    std::stringstream ss;
    ss << "SIDECAR " <<
      qsense::net::data::apiKey <<
      ':' <<
      signature( qsense::net::data::apiSecret, data::POST, uri, currentTime, hash );
    request.setHeader( "Authorization", ss.str() );

    request.setBody( json );

    responseCode = client->post( request );
#if DEBUG
    std::cout << F( "User Provision API returned HTTP response code: " ) << responseCode << std::endl;
#endif

    if ( responseCode == 200 )
    {
      const QString& body = client->readBody();
#if defined( ARDUINO )
      return UserResponse::create( responseCode, body );
#else
      return std::move( UserResponse::create( responseCode, body ) );
#endif
    }
    else
    {
      while ( client->connected() )
      {
        std::cout << F( "  [resp] " ) << client->readLine() << std::endl;
      }
    }
  }

  return UserResponse( responseCode );
}


int16_t SidecarClient::deleteUser( const QString& username, const QString& password )
{
  using qsense::net::DateTime;
  using qsense::net::HttpClient;
  using qsense::net::HttpRequest;

  static QString uri( "/rest/v1/provision/application/user/" );

  uint16_t responseCode = uint16_t( 500 );
  const QString& currentTime = DateTime::singleton().currentTime();

  HttpClient::Ptr client = HttpClient::create();

  if ( client->connect( data::server ) )
  {
#if DEBUG
    std::cout << F( "Connected to " ) << data::server << std::endl;
#endif

    const QString& json = data::credentials( username, password );
    const QString& hash = md5( json );

    HttpRequest request( uri );
    request.setHeader( "Date", currentTime );
    request.setHeader( "Content-Type", "application/json" );
    request.setHeader( "Content-MD5", hash );
    request.setHeader( "Signature-Version", "1" );

    {
      std::stringstream ss;
      ss << json.length();
      request.setHeader( "Content-Length", ss.str() );
    }

    std::stringstream ss;
    ss << "SIDECAR " <<
      qsense::net::data::apiKey <<
      ':' <<
      signature( qsense::net::data::apiSecret, data::DELETE, uri, currentTime, hash );
    request.setHeader( "Authorization", ss.str() );

    request.setBody( json );

    responseCode = client->remove( request );
#if DEBUG
    std::cout << F( "User Provision API returned HTTP response code: " ) << responseCode << std::endl;
#endif
  }

  return responseCode;
}


bool SidecarClient::publish( const qsense::Event& event ) const
{
  using qsense::net::DateTime;
  using qsense::net::HttpClient;
  using qsense::net::HttpRequest;

  static QString uri( "/rest/v1/event" );

  bool flag = false;
  const QString& currentTime = DateTime::singleton().currentTime();

  HttpClient::Ptr client = HttpClient::create();

  if ( client->connect( data::server ) )
  {
#if DEBUG
    std::cout << F( "Connected to " ) << data::server << std::endl;
#endif

    const QString& eventJson = event.toString();
    const QString& hash = md5( eventJson );

    HttpRequest request( uri );
    request.setHeader( "Date", currentTime );
    request.setHeader( "Content-Type", "application/json" );
    request.setHeader( "Content-MD5", hash );
    request.setHeader( "Signature-Version", "1" );

    {
      std::stringstream ss;
      ss << eventJson.length();
      request.setHeader( "Content-Length", ss.str() );
    }

    std::stringstream ss;
    ss << "SIDECAR " <<
      qsense::net::data::userKey <<
      ':' <<
      signature( qsense::net::data::userSecret, data::POST, uri, currentTime, hash );
    request.setHeader( "Authorization", ss.str() );

    request.setBody( eventJson );

    uint16_t responseCode = client->post( request );
#if DEBUG
    std::cout << F( "Event API returned HTTP response code: " ) << responseCode << std::endl;
#endif

    if ( responseCode == 202 ) flag = true;
    else
    {
      while ( client->connected() )
      {
        std::cout << F( "  [resp] " ) << client->readLine() << std::endl;
      }
    }
  }
  else
  {
    std::cout << F( "Connection to " ) << data::server << F( " failed" ) << std::endl;
  }

  return flag;
}


QString SidecarClient::md5( const QString& event ) const
{
  using qsense::hash::MD5;
  MD5 md5;
  return md5.compute( event );
}


QString SidecarClient::signature( const QString& secret,
    const QString& method, const QString& uri,
    const QString& date, const QString& hash ) const
{
  using qsense::hash::Sha1;
  Sha1 sha1;
  return sha1.sign( secret, method, uri, date, hash );
}

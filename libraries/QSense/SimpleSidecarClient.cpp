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
#include <SimpleSidecarClient.h>
#include <SidecarClient.h>
#include <DateTime.h>
#include <QHttpClient.h>
#include <Event.h>
#include <UUID.h>

namespace qsense
{
  struct SimpleSidecarClient
  {
    static SimpleSidecarClient& getInstance()
    {
      static SimpleSidecarClient client;
      return client;
    }

    SimpleSidecarClient() : client(), event() {}
    void reset() { event = qsense::Event(); }

    qsense::net::SidecarClient client;
    qsense::Event event;
  };
}


void SimpleSidecarClient::initNetworkType( NetworkType type )
{
  switch ( type )
  {
    case Ethernet:
      qsense::net::initNetworkType( qsense::net::Ethernet );
      break;
    case WiFi:
      qsense::net::initNetworkType( qsense::net::WiFi );
      break;
  }
}


void SimpleSidecarClient::initUUID( byte mac[6] )
{
  qsense::UUID::init( mac );
}


void SimpleSidecarClient::initUUID()
{
  randomSeed( analogRead( 0 ) );
  const byte v1 = random( 256 );
  const byte v2 = random( 256 );
  const byte v3 = random( 256 );
  const byte v4 = random( 256 );
  const byte v5 = random( 256 );
  const byte v6 = random( 256 );
  byte mac[] = { v1, v2, v3, v4, v5, v6 };
  qsense::UUID::init( mac );
}


void SimpleSidecarClient::initAPIKey( const String& apiKey, const String& apiSecret )
{
  qsense::net::SidecarClient::initAPIKey( apiKey.c_str(), apiSecret.c_str() );
}


SimpleSidecarClient::UserResponse SimpleSidecarClient::authenticate(
    const String& username, const String& password )
{
  const qsense::net::SidecarClient::UserResponse& response =
      qsense::SimpleSidecarClient::getInstance().client.authenticate(
        username.c_str(), password.c_str() );
  return UserResponse( response.responseCode, response.keyId.c_str(), response.secret.c_str() );
}


SimpleSidecarClient::UserResponse SimpleSidecarClient::createUser(
    const String& username, const String& password )
{
  const qsense::net::SidecarClient::UserResponse& response =
      qsense::SimpleSidecarClient::getInstance().client.createUser(
        username.c_str(), password.c_str() );
  return UserResponse( response.responseCode, response.keyId.c_str(), response.secret.c_str() );
}


SimpleSidecarClient::UserResponse SimpleSidecarClient::createOrRetrieveAccessKeys(
    const String& username, const String& password )
{
  const qsense::net::SidecarClient::UserResponse& response =
      qsense::SimpleSidecarClient::getInstance().client.createOrRetrieveAccessKeys(
        username.c_str(), password.c_str() );
  return UserResponse( response.responseCode, response.keyId.c_str(), response.secret.c_str() );
}


void SimpleSidecarClient::initUserKey( const String& userKey, const String& userSecret )
{
  qsense::net::SidecarClient::initUserKey( userKey.c_str(), userSecret.c_str() );
}


int16_t SimpleSidecarClient::deleteUser( const String& username, const String& password )
{
  return qsense::SimpleSidecarClient::getInstance().client.deleteUser(
        username.c_str(), password.c_str() );
}


void SimpleSidecarClient::initEventAPI( const EventAPIData& data )
{
  qsense::Location location( data.latitude, data.longitude );
  qsense::Event::init( qsense::UUID( data.deviceUUID.c_str() ),
      data.stream.c_str(), location );
}


void SimpleSidecarClient::addReading( const String& key, const float value )
{
  qsense::SimpleSidecarClient::getInstance().event += qsense::Reading( key.c_str(), value );
}


void SimpleSidecarClient::addTag( const String& value )
{
  qsense::SimpleSidecarClient::getInstance().event += value.c_str();
}


void SimpleSidecarClient::addKeyTag( const String& key, const String& tag )
{
  qsense::SimpleSidecarClient::getInstance().event.add( key.c_str(), tag.c_str() );
}


bool SimpleSidecarClient::publish()
{
  using qsense::SimpleSidecarClient;

  const bool result = SimpleSidecarClient::getInstance().client.publish(
        SimpleSidecarClient::getInstance().event );
  SimpleSidecarClient::getInstance().reset();
  return result;
}


const String SimpleSidecarClient::currentTime()
{
  const qsense::QString& ct = qsense::net::DateTime::singleton().currentTime();
  return String( ct.c_str() );
}


const String SimpleSidecarClient::date()
{
  const qsense::QString& dt = qsense::net::DateTime::singleton().date();
  return String( dt.c_str() );
}


int64_t SimpleSidecarClient::currentTimeMillis()
{
  return qsense::net::DateTime::singleton().currentTimeMillis();
}

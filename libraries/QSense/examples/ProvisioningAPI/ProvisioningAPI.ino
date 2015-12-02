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
#include <SPI.h>
//#include <Ethernet.h>
#include <EthernetV2_0.h>
#include <WiFi.h>

#define W5200_CS  10
#define SDCARD_CS 4

#include <StandardCplusplus.h>
#include <SimpleSidecarClient.h>
#include <serstream>

namespace std
{
  ohserialstream cout(Serial);
}

//using qsense::QString;

const int ledPower = 12;

// Sidecar API key/secret pair for creating user account(s).
// Will be read in from console
String apiKey;
String apiSecret;

// Sidecar user account to create to retrieve access keys for events API
String username;
String password;


// A proper unique mac address is critical to having a stable UUID generated
// for events published to Sidecar service.  Please use the MAC address
// printed on the sticker on the sheild
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip( 10, 0, 1, 200 );

SimpleSidecarClient client;

void initUUID()
{
  // Initialise UUID engine
  // If using WiFi, the WiFi api provides a way to look up current MAC
  // address.  That would be better to get proper UUID values.
  //qsense::UUID::init( mac );
  SimpleSidecarClient::initUUID();
}


void initSidecar()
{
  Serial.setTimeout( 30000 );

  Serial.println( F( "Please enter your API key:" ) );
  apiKey = Serial.readStringUntil( '\n' );

  Serial.println( F( "Please enter your API secret:" ) );
  apiSecret = Serial.readStringUntil( '\n' ).c_str();

  Serial.print( F( "Initialising Sidecar API with API key: " ) );
  Serial.print( apiKey );
  Serial.print( F( " and secret: " ) );
  Serial.println( apiSecret );

  // Initialise SidecarClient API
  SimpleSidecarClient::initAPIKey( apiKey, apiSecret );
  Serial.setTimeout( 1000 );
}


// Invoke this in setup if you are using ethernet shield for networking
void initEthernet()
{
  if ( Ethernet.begin( mac ) == 0 )
  {
    Serial.println( F( "Failed to configure Ethernet using DHCP" ) );
    Ethernet.begin( mac, ip );
  }

  // give the Ethernet shield a second to initialize:
  Serial.println( F( "Connecting to network..." ) );
  delay( 1000 );

  // Initialise network API to use Ethernet
  SimpleSidecarClient::initNetworkType( SimpleSidecarClient::Ethernet );
}


void initEthernetStatic()
{
  IPAddress dnsServer( 208, 67, 222, 222 ); // OpenDNS server
  Ethernet.begin( mac, ip, dnsServer );

  // give the Ethernet shield a second to initialize:
  Serial.println( F( "Connecting to network..." ) );
  delay( 1000 );

  // Initialise network API to use Ethernet
  SimpleSidecarClient::initNetworkType( SimpleSidecarClient::Ethernet );
}


// Invoke this in setup if you are using ethernet shield for networking
void initWiFi()
{
  // Perform normal WiFi setup

  // Initialise network API to use WiFi
  SimpleSidecarClient::initNetworkType( SimpleSidecarClient::WiFi );
}


// Create a test user for using the Event API
void createUser()
{
  Serial.setTimeout( 30000 );

  Serial.println( F( "Enter email address you wish to use as username:" ) );
  username = Serial.readStringUntil( '\n' );

  Serial.println( F( "Enter password you wish to use with account (8-20 chars):" ) );
  password = Serial.readStringUntil( '\n' );

  Serial.print( F( "Accessing or creating account " ) );
  Serial.print( username );
  Serial.print( F( "/" ) );
  Serial.println( password );

  const SimpleSidecarClient::UserResponse& response = client.createUser( username, password );

  if ( response.responseCode == 200 )
  {
    Serial.print( F( "Created user with access key: " ) );
    Serial.print( response.keyId );
    Serial.print( F( " and secret: " ) );
    Serial.println( response.secret );
    Serial.println( F(  "Please use this key/secret combination to initialise the Sidecar API using qsense::net::SidecarClient::initUserKey method." ) );
  }
  else
  {
    Serial.print( F( "Error creating user.  Response code: " ) );
    Serial.println( response.responseCode );
  }

  Serial.setTimeout( 1000 );
}


// Create or retrieve test user for using the Event API
void createOrRetrieveAccessKeys()
{
  Serial.setTimeout( 30000 );

  Serial.println( F( "Enter username (email address).  New access keys will be created if non-existing." ) );
  Serial.println( F( "Username: " ) );
  username = Serial.readStringUntil( '\n' );

  Serial.println( F( "Enter password for your account (8-20 chars):" ) );
  password = Serial.readStringUntil( '\n' );

  Serial.print( F( "Accessing or creating account " ) );
  Serial.print( username );
  Serial.print( F( "/" ) );
  Serial.println( password );

  const SimpleSidecarClient::UserResponse& response = client.createOrRetrieveAccessKeys( username, password );

  if ( response.responseCode == 200 )
  {
    Serial.print( F( "Created user with access key: " ) );
    Serial.print( response.keyId );
    Serial.print( F( " and secret: " ) );
    Serial.println( response.secret );
    Serial.println( F( "Please use this key/secret combination to initialise the Sidecar API using qsense::net::SidecarClient::initUserKey method." ) );
  }
  else
  {
    Serial.print( F( "Error creating user.  Response code: " ) );
    Serial.println( response.responseCode );
  }

  Serial.setTimeout( 1000 );
}


// Just display the user access keys
void displayKeys()
{
  const SimpleSidecarClient::UserResponse& response = client.authenticate( username, password );

  if ( response.responseCode == 200 )
  {
    Serial.print( F( "Access key: " ) );
    Serial.print( response.keyId );
    Serial.print( F( " and secret: " ) );
    Serial.println( response.secret );
  }
  else
  {
    Serial.print( F( "Error authenticating user.  Response code: " ) );
    Serial.println( response.responseCode );
  }
}


int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


void setup()
{
  Serial.begin( 57600 );

  initEthernet();
  //initEthernetStatic();

  // Set up sensor pin
  pinMode( ledPower, OUTPUT );

  initUUID();
  initSidecar();
  createUser();
  //createOrRetrieveUser();
}

void loop()
{
  Serial.print( F( "Current time: " ) );
  Serial.println( client.currentTime() );
  
  displayKeys();
  Serial.print( F( "Free SRAM: " ) );
  Serial.println( freeRam() );
  delay( 60000 );
}


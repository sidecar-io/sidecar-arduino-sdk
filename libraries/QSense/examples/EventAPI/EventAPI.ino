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
#include <Ethernet.h>
#include <WiFi.h>

#include <StandardCplusplus.h>
#include <SimpleSidecarClient.h>
#include <serstream>

namespace std
{
  ohserialstream cout(Serial);
}

const int ledPower = 12;

// A proper unique mac address is critical to having a stable UUID generated
// for events published to Sidecar service.  Please use the MAC address
// printed on the sticker on the sheild
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip( 192, 168, 0, 177 );

SimpleSidecarClient client;

void initUUID()
{
  // Initialise UUID engine
  // If using WiFi, the WiFi api provides a way to look up current MAC
  // address.  That would be better to get proper UUID values.
  SimpleSidecarClient::initUUID( mac );
}


void initSidecar()
{
  Serial.setTimeout( 30000 );

  // Sidecar API key/secret for initialising SidecarClient API.
  Serial.println( F( "Please enter your user access key:" ) );
  const String userKey = Serial.readStringUntil( '\n' );
  Serial.println( F( "Please enter your user access secret:" ) );
  const String userSecret = Serial.readStringUntil( '\n' );

  // Initialise SidecarClient API
  SimpleSidecarClient::initUserKey( userKey, userSecret );
  Serial.setTimeout( 1000 );
}


void initEventAPI()
{
  SimpleSidecarClient::EventAPIData data;
  // Sidecar stream id to use
  data.stream = "airQuality";

  // Unique device UUID value.  Change per device deployment
  data.deviceUUID = "47c247bb-493d-482d-be8d-ebaeb2813170";

  // Default location to use.  At present there is no way to dynamically
  // retrieve location for the device.
  data.latitude = 47.60621;
  data.longitude = -122.33207;

  // Initialise the Event API
  SimpleSidecarClient::initEventAPI( data );
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


// Invoke this in setup if you are using ethernet shield for networking
void initWiFi()
{
  // Perform normal WiFi setup

  // Initialise network API to use WiFi
  SimpleSidecarClient::initNetworkType( SimpleSidecarClient::WiFi );
}


// Measure air quality, and publish data to Sidecar Event API
void measure()
{
  const int samplingTime = 280;
  digitalWrite( ledPower, LOW ); // power on the LED
  delayMicroseconds( samplingTime );

  const int measurePin = 6;
  float voMeasured = analogRead( measurePin ); // read the dust value
  client.addReading( "SV", voMeasured );
  
  const int deltaTime = 40;
  delayMicroseconds( deltaTime );
  digitalWrite( ledPower, HIGH ); // turn the LED off

  const int sleepTime = 9680;
  delayMicroseconds( sleepTime );
 
  // 0 - 5.0V mapped to 0 - 1023 integer values
  // recover voltage
  const float calcVoltage = voMeasured * ( 5.0 / 1024 );
  const float dustDensity = 0.17 * calcVoltage - 0.1;

  // Add the readings
  client.addReading( "V", calcVoltage );
  client.addReading( "DD", dustDensity );

  // Add tags to the event.  To demonstrate we add current date to help
  // easily retrieve readings by day
  client.addTag( client.date() );
  
  if ( client.publish() )
  {
    Serial.println( F( "Published event to sidecar" ) );
  }
  else
  {
    Serial.println( F( "Error publishing event to sidecar" ) );
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

  // Set up sensor pin
  pinMode( ledPower, OUTPUT );

  initUUID();
  initEventAPI();
  initSidecar();
}

void loop()
{
  Serial.print( F( "Current time: " ) );
  Serial.println( client.currentTime() );
  measure();
  
  Serial.print( F( "Free SRAM: " ) );
  Serial.println( freeRam() );
  delay( 5000 );
}


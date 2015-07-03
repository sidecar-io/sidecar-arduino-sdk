#include <SPI.h>
#include <Ethernet.h>
#include <WiFi.h>

#include <StandardCplusplus.h>

#include <QHttpClient.h>
#include <DateTime.h>
#include <SidecarClient.h>
#include <UUID.h>

namespace std
{
  ohserialstream cout(Serial);
}

using qsense::QString;

const int ledPower = 12;


// A proper unique mac address is critical to having a stable UUID generated
// for events published to Sidecar service.  Please use the MAC address
// printed on the sticker on the sheild
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip( 192, 168, 0, 177 );


void initUUID()
{
  // Initialise UUID engine
  // If using WiFi, the WiFi api provides a way to look up current MAC
  // address.  That would be better to get proper UUID values.
  qsense::UUID::init( mac );
}


void initSidecar()
{
  // Sidecar API key/secret for initialising SidecarClient API.
  const QString apiKey = "";
  const QString apiSecret = "";

  // Initialise SidecarClient API
  qsense::net::SidecarClient::init( apiKey, apiSecret );
}


void initEventAPI()
{
  // Sidecar stream id to use
  const QString streamId = "airQuality";

  // Unique device UUID value.  Change per device deployment
  const QString deviceUUID = "47c247bb-493d-482d-be8d-ebaeb2813170";

  // Default location to use.  At present there is no way to dynamically
  // retrieve location for the device.
  const qsense::Location location( 47.60621, -122.33207 );

  // Initialise the Event API
  qsense::Event::init( deviceUUID, streamId, location );
}


// Invoke this in setup if you are using ethernet shield for networking
void initEthernet()
{
  if ( Ethernet.begin( mac ) == 0 )
  {
    std::cout << F( "Failed to configure Ethernet using DHCP" ) << std::endl;
    Ethernet.begin( mac, ip );
  }

  // give the Ethernet shield a second to initialize:
  std::cout << F( "Connecting to network..." ) << std::endl;
  delay( 1000 );

  // Initialise network API to use Ethernet
  qsense::net::initNetworkType( qsense::net::Ethernet );
}


// Invoke this in setup if you are using ethernet shield for networking
void initWiFi()
{
  // Perform normal WiFi setup

  // Initialise network API to use WiFi
  qsense::net::initNetworkType( qsense::net::WiFi );
}


// Measure air quality, and publish data to Sidecar Event API
void measure()
{
  const int samplingTime = 280;
  digitalWrite( ledPower, LOW ); // power on the LED
  delayMicroseconds( samplingTime );

  const int measurePin = 6;
  float voMeasured = analogRead( measurePin ); // read the dust value

  const int deltaTime = 40;
  delayMicroseconds( deltaTime );
  digitalWrite( ledPower, HIGH ); // turn the LED off

  const int sleepTime = 9680;
  delayMicroseconds( sleepTime );

  // 0 - 5.0V mapped to 0 - 1023 integer values
  // recover voltage
  const float calcVoltage = voMeasured * ( 5.0 / 1024 );
  const float dustDensity = 0.17 * calcVoltage - 0.1;

  qsense::net::DateTime& dt = qsense::net::DateTime::singleton();
  const QString& date = dt.date();
  const QString& time = dt.currentTime();

  qsense::Event event;

  // Add the readings
  event += qsense::Reading( "SV", voMeasured, time );
  event += qsense::Reading( "V", calcVoltage, time );
  event += qsense::Reading( "DD", dustDensity, time );

  // Add tags to the event.  To demonstrate we add current date to help
  // easily retrieve readings by day
  event += date;

  qsense::net::SidecarClient client;
  if ( client.publish( event ) )
  {
    std::cout << "Published " << event << " to sidecar" << std::endl;
  }
  else
  {
    std::cout << "Error publishing " << event << " to sidecar" << std::endl;
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
  std::cout << F( "Current time: " ) << qsense::net::DateTime::singleton().currentTime() << std::endl;
  measure();

  std::cout << F( "Free SRAM: " ) << freeRam() << std::endl;
  delay( 60000 );
}

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
  const QString apiKey = "DEROHOVJQ85UITACAGPC";
  const QString apiSecret = "WEOxAgLUfYd6UhsT88x2j+2+yX6tn00dTwf+fdwV";

  // Initialise SidecarClient API
  qsense::net::SidecarClient::initAPIKey( apiKey, apiSecret );
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


// Create a test user for using the Event API
void createUser()
{
  using qsense::net::SidecarClient;

  const QString username( "test" );
  const QString password( "test" );
  SidecarClient client;

  const SidecarClient::UserResponse& response = client.createUser( username, password );

  if ( response.responseCode == 200 )
  {
    std::cout << "Created user with userKey: " << response.keyId << " secret: " << response.secret << std::endl;
  }
  else
  {
    std::cout << "Error creating user.  Response code: " << response.responseCode << std::endl;
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
  initSidecar();
  createUser();
}

void loop()
{
  std::cout << F( "Current time: " ) << qsense::net::DateTime::singleton().currentTime() << std::endl;
  
  std::cout << F( "Free SRAM: " ) << freeRam() << std::endl;
  delay( 60000 );
}


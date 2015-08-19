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

// Sidecar API key/secret pair for creating user account(s).
// Will be read in from console
QString apiKey;
QString apiSecret;

// Sidecar user account to create to retrieve access keys for events API
QString username;
QString password;


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
  Serial.setTimeout( 30000 );

  std::cout << F( "Please enter your API key:" ) << std::endl;
  apiKey = Serial.readStringUntil( '\n' ).c_str();

  std::cout << F( "Please enter your API secret:" ) << std::endl;
  apiSecret = Serial.readStringUntil( '\n' ).c_str();

  std::cout << F( "Initialising Sidecar API with API key: " ) <<
    apiKey << F( " and secret: " ) << apiSecret << std::endl;

  // Initialise SidecarClient API
  qsense::net::SidecarClient::initAPIKey( apiKey, apiSecret );
  Serial.setTimeout( 1000 );
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

  Serial.setTimeout( 30000 );

  std::cout << "Enter email address you wish to use as username:" << std::endl;
  username = Serial.readStringUntil( '\n' ).c_str();

  std::cout << "Enter password you wish to use with account (8-20 chars):" << std::endl;
  password = Serial.readStringUntil( '\n' ).c_str();

  std::cout << "Accessing or creating account " << username << "/" << password << std::endl;

  SidecarClient client;

  const SidecarClient::UserResponse& response = client.createUser( username, password );

  if ( response.responseCode == 200 )
  {
    std::cout << "Created user with access key: " << response.keyId << " and secret: " << response.secret << std::endl;
    std::cout << "Please use this key/secret combination to initialise the Sidecar API using qsense::net::SidecarClient::initUserKey method." << std::endl;
  }
  else
  {
    std::cout << "Error creating user.  Response code: " << response.responseCode << std::endl;
  }

  Serial.setTimeout( 1000 );
}


// Create or retrieve test user for using the Event API
void createOrRetrieveUser()
{
  using qsense::net::SidecarClient;

  Serial.setTimeout( 30000 );

  std::cout << "Enter username (email address).  New account will be created if non-existing." << std::endl;
  std::cout << "Username: " << std::endl;
  username = Serial.readStringUntil( '\n' ).c_str();

  std::cout << "Enter password for your account (8-20 chars):" << std::endl;
  password = Serial.readStringUntil( '\n' ).c_str();

  std::cout << "Accessing or creating account " << username << "/" << password << std::endl;

  SidecarClient client;

  const SidecarClient::UserResponse& response = client.createOrAuthenticateUser( username, password );

  if ( response.responseCode == 200 )
  {
    std::cout << "Created user with access key: " << response.keyId << " and secret: " << response.secret << std::endl;
    std::cout << "Please use this key/secret combination to initialise the Sidecar API using qsense::net::SidecarClient::initUserKey method." << std::endl;
  }
  else
  {
    std::cout << "Error creating user.  Response code: " << response.responseCode << std::endl;
  }

  Serial.setTimeout( 1000 );
}


// Just display the user access keys
void displayKeys()
{
  using qsense::net::SidecarClient;
  SidecarClient client;

  const SidecarClient::UserResponse& response = client.authenticate( username, password );

  if ( response.responseCode == 200 )
  {
    std::cout << "Access key: " << response.keyId << " and secret: " << response.secret << std::endl;
  }
  else
  {
    std::cout << "Error authenticating user.  Response code: " << response.responseCode << std::endl;
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
  //createUser();
  createOrRetrieveUser();
}

void loop()
{
  std::cout << F( "Current time: " ) << qsense::net::DateTime::singleton().currentTime() << std::endl;
  
  displayKeys();
  std::cout << F( "Free SRAM: " ) << freeRam() << std::endl;
  delay( 60000 );
}


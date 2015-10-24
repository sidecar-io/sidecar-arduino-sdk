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
#ifndef SIMPLESIDECARCLIENT_H
#define SIMPLESIDECARCLIENT_H

#include <Arduino.h>

/**
 * @brief A simple client implementation that hides the low-level API.
 *
 * The low-level API is a cross-platform standard C++ API that has been
 * tested on Mac OS X and Windows in addition to Arduino Mega.  The simple
 * client is specific to Arduino and is intended to provide a single class
 * that Arduino applications may use to provision devices and publish event data.
 *
 * The methods in this class are declared in the same order that a typical
 * calling application will need to make to interact with Sidecar.
 */
class SimpleSidecarClient
{
public:
  /// A simple data structure that represents the response from Sidecar
  /// Provisioning API.
  struct UserResponse
  {
    /// Create a new instance of the data structure.
    UserResponse( uint16_t response, const String& key, const String& sec ) :
      responseCode( response ), keyId( key ), secret( sec ) {}

    /// The HTTP response code returned by the Sidecar Provisioning API.
    const uint16_t responseCode;

    /// The user access key value.
    const String keyId;

    /// The user access secret value.
    const String secret;
  };

  SimpleSidecarClient() {}

  /// Enumeration of network connection types for device
  enum NetworkType { Ethernet = 0, WiFi = 1 };

   /// Initialise the API to use the specified type.
  static void initNetworkType( NetworkType type );

  /**
   * Initialise UUID engine.
   * If using WiFi, the WiFi api provides a way to look up current MAC
   * address.  That would be better to get proper UUID values.
   */
  static void initUUID( byte mac[6] );

  /// Initialise UUID engine using a random seed.
  static void initUUID();

  /**
   * Initialise the API with the API key and secret used to sign provisioning requests.
   * This is usually the first step in initialising the API for a device.
   * Users must register their application with Sidecar and specify the
   * generated API access key and secret.
   *
   * Calling applications will generally store the API key and secret in
   * flash storage using \c PROGMEM and use that to initialise the API
   * on each run of the application.
   */
  static void initAPIKey( const String& apiKey, const String& apiSecret );

  /**
   * @brief Authenticate the user against Sidecar.  Return the existing
   * user key/secret pair to use with the events API.
   *
   * This is generally the first call to use on first run of the application.
   * Use \c authenticate before {@link #createUser} to ensure that the
   * account name (email id) and password are not already in use.
   *
   * If the response contains a valid (non-empty) keyId and secret,
   * calling applications may chose to save it to flash storage using
   * \c PROGMEM.  Once stored the provisioning process is complete, and
   * the application may interact directly with the Event API methods.
   * On each run of the application invoke {@link #initUserKey} with the
   * stored user key and secret.
   *
   * @param username The username of the user to authenticate as
   * @param password The password to use to authenticate
   * @return The response which on success will contain the key/secret pair.
   *   In case of failure the \c responseCode value will be other than \c 200.
   */
  UserResponse authenticate( const String& username, const String& password );

  /**
   * @brief Create a new user account with Sidecar.  Use this on first
   * run of application if the {@link #authenticate} method returned
   * an invalid (not 200) \c responseCode.
   *
   * If the method returns with empty key/secret pair, it generally
   * indicates that the username is taken or username/password failed
   * format/length rules.
   *
   * @param username The username preferred by user (email format)
   * @param password The password to associate with user account (8-20 char length)
   * @return  Return a UserResponse struct with key/secret pair populated
   *   on success.
   */
  UserResponse createUser( const String& username, const String& password );

  /**
   * @brief Create or retrieve application access keys for the specified user.
   *
   * @param username The username preferred by user (email format)
   * @param password The password to associate with user account (8-20 char length)
   * @return  Return a UserResponse struct with key/secret pair populated
   *   on success.  The \c responseCode value will be \c 200 on success.
   */
  UserResponse createOrRetrieveAccessKeys( const String& username, const String& password );

  /**
   * Initialise the API with the user key and secret used to sign event requests.
   * This step can be performed after provisioning a user account for
   * a device.  Use the provisioning API
   *
   * Calling applications will generally store the user access key and
   * secret in flash storage using \c PROGMEM and use that to initialise
   * the API on each run of the application.
   */
  static void initUserKey( const String& userKey, const String& userSecret );

  /**
   * @brief deleteUser Deprovision a user from the system.  Removes
   * the user account, access key/secret and devices associated with
   * the user.
   *
   * @param username The username of the user account
   * @param password The password for the user account.
   * @return The HTTP response code returned by Sidecar.  Response
   *   code \c 204 indicates success.
   */
  int16_t deleteUser( const String& username, const String& password );

  /**
   * @brief Initialise the Sidecar Event API.
   *
   * @param streamId The user defined stream name
   * @param deviceUUID The device identifier for the hardware, this value
   *   should be considered static (e.g., a MAC Address).
   * @param latitude Latitude for current device location
   * @param longitude Longitude for current device location
   */
  static void initEventAPI( const String& streamId, const String& deviceUUID,
      const float latitude, const float longitude );

  /**
   * @brief Add the specified reading key-value pair to an event.  Add
   * as many readings as desired before publishing the event to Sidecar
   * ({@link #publish}).  On publish, the event is re-initialised for
   * publishing subsequent readings.
   *
   * @param key A user defined key for the reading
   * @param value The value for the reading.
   */
  void addReading( const String& key, const float value );

  /**
   * @brief Add optional tag values to help analyse the event after
   * publishing to Sidecar.
   *
   * \b NOTE: Tags should be single words without spaces.
   *
   * @param value A tag value.
   */
  void addTag( const String& value );

  /**
   * @brief Add option key-tag values to help identify/analyse the event
   * after publishing to Sidecar.  Use the same key to assign multiple
   * values to a key.
   *
   * \b NOTE: Tags should be single words without spaces.
   *
   * @param key The key for the key-tag pair.
   * @param tag A tag value to associate with the key.
   */
  void addKeyTag( const String& key, const String& tag );

  /**
   * @brief publish Publish the built up event to the Sidecar Event API.
   * Invoke {@link #addReading} with the individual readings that are
   * part of the current event, and {@link #addTag} as needed to build
   * up a complete event before publishing to Sidecar.
   *
   * @return Returns \c true if publish succeeded.
   */
  bool publish();

  /// Return the current date/time in ISO 8601 format
  const String currentTime();

  /// Return the current date in ISO 8601 format
  const String date();

  /// Return the milli seconds since UNIX epoch.
  int64_t currentTimeMillis();
};

#endif // SIMPLESIDECARCLIENT_H

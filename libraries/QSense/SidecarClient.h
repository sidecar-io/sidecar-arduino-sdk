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
#ifndef QSENSE_NET_SIDECARCLIENT_H
#define QSENSE_NET_SIDECARCLIENT_H

#if defined( ARDUINO )
#include "QSense.h"
#include "Event.h"
#else
#include <QSense.h>
#include <Event.h>
#endif

namespace qsense
{
  namespace net
  {
    /**
     * @brief Class that encapsulates interactions with the Sidecar REST API.
     */
    class SidecarClient
    {
    public:
      /// A simple structure that represents the result of a user 
      /// provisioning request.
      struct UserResponse
      {
        /// Create a new UserResponse instance
        UserResponse( uint16_t response, const QString& key = QString(),
          const QString& sec = QString() ) :
          responseCode( response ), keyId( key ), secret( sec ) {}

        /// Create a new UserResponse instance and populate the key and
        /// secret by parsing the response body
        static const UserResponse create( uint16_t response, const QString& body );

        uint16_t responseCode;
        const QString keyId;
        const QString secret;
      };

      /**
       * @brief Create a new user account with Sidecar.  This is usually
       * the first interaction with the Sidecar service.  Needed only
       * once per application.
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
      UserResponse createUser( const QString& username, const QString& password );

      /**
       * @brief Create or retrieve application access keys for the
       * specified user.
       *
       * @param username The username preferred by user (email format)
       * @param password The password to associate with user account (8-20 char length)
       * @return  Return a UserResponse struct with key/secret pair populated
       *   on success.
       */
      UserResponse createOrRetrieveAccessKeys( const QString& username, const QString& password );

      /**
       * @brief Authenticate the user against Sidecar.  Return the existing
       * user key/secret pair to use with the events API.
       *
       * @param username The username of the user to authenticate as
       * @param password The password to use to authenticate
       * @return The response which on success will contain the key/secret pair
       */
      UserResponse authenticate( const QString& username, const QString& password );

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
      int16_t deleteUser( const QString& username, const QString& password );

      /// Publish the specified event to the Sidecar Event API
      bool publish( const Event& event ) const;

      /// Initialise the API with the API key and secret used to sign provisioning requests.
      static void initAPIKey( const QString& apiKey, const QString& apiSecret );

      /// Initialise the API with the user key and secret used to sign event requests.
      static void initUserKey( const QString& userKey, const QString& userSecret );

    private:
      QString md5( const QString& event ) const;

      QString signature(
        const QString& secret,
        const QString& method,
        const QString& uri,
        const QString& date,
        const QString& hash ) const;
    };

  } // namespace net
} // namespace qsense

#endif // QSENSE_NET_SIDECARCLIENT_H

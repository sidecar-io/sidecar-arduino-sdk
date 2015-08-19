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
       * @brief Create or authenticate the specified user with Sidecar.
       * Sidecar will automatically create the user account if account
       * does not exist.
       *
       * @param username The username preferred by user (email format)
       * @param password The password to associate with user account (8-20 char length)
       * @return  Return a UserResponse struct with key/secret pair populated
       *   on success.
       */
      UserResponse createOrAuthenticateUser( const QString& username, const QString& password );

      /**
       * @brief Authenticate the user against Sidecar.  Return the existing
       * user key/secret pair to use with the events API.
       *
       * @param username The username of the user to authenticate as
       * @param password The password to use to authenticate
       * @return The response which on success will contain the key/secret pair
       */
      UserResponse authenticate( const QString& username, const QString& password );

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

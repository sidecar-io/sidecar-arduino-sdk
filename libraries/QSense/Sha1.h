/*
*/

#ifndef QSENSE_HASH_SHA1_h
#define QSENSE_HASH_SHA1_h

#include <inttypes.h>
#if defined( ARDUINO )
#include "QSense.h"
#else
#include <QSense.h>
#endif

// SHA1 related functions
namespace qsense
{
  namespace hash
  {
    /**
     * @brief Class for hashing using SHA1 algorithm.
     */
    class Sha1
    {
    public:
      /// Default constructor.
      Sha1();

      /// Destructor.  No actions required.
      ~Sha1() {}

      /**
       * @brief Generate SHA1 hash for specified input into output array
       * @param input The input char array that is to be hashed.
       * @param ilen The length of the input char array.
       * @param output The output char array into which hash value is written.
       */
      void hash( unsigned char* input, int ilen, unsigned char output[20] );

      /// Generate SHA1 hash for the specified input string.
      qsense::QString hash( const qsense::QString& input );

      /**
       * @brief Generate SHA1 HMAC for the specified input using specified key.
       * @param key The key to use to generate the HMAC
       * @param keylen The length of the key
       * @param input The input char array to hash
       * @param ilen The length of the input char array
       * @param output The output char array into which hash value is written.
       */
      void hmac( unsigned char* key, int keylen,
                    unsigned char* input, int ilen,
                    unsigned char output[20] );

      /// Generate SHA1 HMAC using the specified key for the input string
      qsense::QString hmac( const qsense::QString &key, const qsense::QString& input );

      /**
       * @brief Generate the signature for the Sidecar Authorization header
       * @param privateKey The api secret to use to sign
       * @param httpMethod The HTTP method used for the Sidecar API interaction
       * @param uriPath The URI path with which to interact
       * @param date The current timestamp
       * @param contentMd5 The MD5 hash for the content to be submitted to Sidecar
       * @param signatureVersion The signature version to specify in header
       * @return The Base64 encoded authorisation signature
       */
      qsense::QString sign( const qsense::QString& privateKey,
        const qsense::QString& httpMethod,
        const qsense::QString& uriPath,
        const qsense::QString& date,
        const qsense::QString& contentMd5,
        const qsense::QString& signatureVersion = qsense::QString( "1" ) );

      /// SHA1 context representation
      struct Context
      {
        unsigned long total[2];     /*!< number of bytes processed  */
        unsigned long state[5];     /*!< intermediate digest state  */
        unsigned char buffer[64];   /*!< data block being processed */

        unsigned char ipad[64];     /*!< HMAC: inner padding        */
        unsigned char opad[64];     /*!< HMAC: outer padding        */
      };

    private:
      void startHash( Context* ctx );
      void updateHash( Context* ctx, unsigned char* input, int ilen );
      void finishHash( Context* ctx, unsigned char output[20] );

      void startHmac( Context* ctx, unsigned char* key, int keylen );
      void updateHmac( Context* ctx, unsigned char* input, int ilen );
      void finishHmac( Context* ctx, unsigned char output[20] );
    };
  }
}

#endif // QSENSE_HASH_SHA1_h

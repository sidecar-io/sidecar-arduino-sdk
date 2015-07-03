#ifndef QSENSE_NET_HTTPCLIENT_H
#define QSENSE_NET_HTTPCLIENT_H

#if defined( ARDUINO )
#include "QSense.h"
#include "../StandardCplusplus/map"
#else
#include <QSense.h>
#include <map>
#endif

namespace qsense
{
  /**
   * Namespace for classes that provide network services and require
   * a network connection to work.
   */
  namespace net
  {
    /**
     * @brief A HTTP Client class for use with either ethernet or wifi.
     * Before use, the client should be initialised with the network
     * type used by the device ({@link qsense::net::initNetworkType}.
     */
    class HttpClient
    {
    public:
      /// Map used to define request headers
      typedef std::map<qsense::QString,qsense::QString> Headers;

      /// Destructor for sub-classes
      virtual ~HttpClient() {}

      /**
       * @brief Factory method for creating concrete instances based on initialisation.
       *
       * <b>Note:</b>Callers must delete the returned instance.
       * @return An instance that uses either ethernet or wifi to connect
       *   to the network.  Callers must delete the returned instance.
       */
      static HttpClient* create();

      /// Make a socket connection to the specified server on specified port (default 80)
      virtual int16_t connect( const qsense::QString& server, uint16_t port = 80 ) = 0;

      /// Check to see if the client is connected to the server
      virtual uint8_t connected() = 0;

      /**
       * @brief Perform a GET request for the specified url.  Optionally
       * specify a map of custom headers to send to server.
       * @param uri The remote URI path to request from server
       * @param headers Optional map of request headers to specify to server.
       * @return The HTTP response code from server.
       */
      virtual uint16_t get( const qsense::QString& uri, const Headers& headers = Headers() ) = 0;

      /**
       * @brief Perform a POST request to the specified url.  Optionally
       * specify a map of custom headers and a string body to send to server.
       * @param uri The remote URI path to request from server
       * @param headers Optional map of request headers to specify to server.
       * @param body Optional body to post to server.
       * @return The HTTP response code from server.
       */
      virtual uint16_t post( const qsense::QString& uri,
        const Headers& headers = Headers(),
        const qsense::QString& body = qsense::QString() ) = 0;

      /**
       * @brief Read a line from the HTTP response.
       *
       * A line can be either a header or content.  Use to process raw
       * HTTP response line by line.
       * @return A line (content until newline character) of text from raw response.
       */
      virtual const qsense::QString readLine() = 0;

      /// Return a map of the HTTP response headers
      virtual Headers readHeaders() = 0;

      /**
       * @brief Read the entire contents of the server response body.
       * Note: This method also reads headers.  If headers have already
       * been read, it may end up losing some of the response body.
       *
       * WARNING: Use with caution.  Can run embedded devices
       * out of memory very easily.
       *
       * @return The entire http response body content.
       */
      virtual const qsense::QString readBody() = 0;

    protected:
      /**
       * @brief Send the specified request headers to the HTTP server.
       *
       * @param headers The map of headers to send to the server.
       * @param close Flag indicating whether HTTP keep-alive is NOT to be used.
       */
      virtual void writeHeaders( const Headers& headers, bool close = true ) = 0;
    };

    /// Enumeration of network connection types for device
    enum NetworkType { Ethernet = 0, WiFi = 1 };

    /**
     * Initialise the API to use the specified type.
     * {@link HttpClient::create} uses this type to create appropriate
     * implementation.
     */
    void initNetworkType( NetworkType type );

  } // namespace net
} // namespace qsense

#endif // QSENSE_NET_HTTPCLIENT_H

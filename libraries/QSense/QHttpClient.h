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
#ifndef QSENSE_NET_HTTPCLIENT_H
#define QSENSE_NET_HTTPCLIENT_H

#include "QSense.h"

#if defined( ARDUINO )
#if USE_Ethernet_Shield_V2
#include <EthernetV2_0.h>
#else
#include <Ethernet.h>
#endif
#include "AutoPtr.h"
#include "RefCountedObject.h"
#include "HttpRequest.h"
#include "../StandardCplusplus/map"
#else
#include <AutoPtr.h>
#include <RefCountedObject.h>
#include <net/HttpRequest.h>
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
    class HttpClient : public RefCountedObject
    {
    public:
      /// Type for auto pointer to a http client instance.
      typedef AutoPtr<HttpClient> Ptr;

      /// Default constructor.
      HttpClient() : RefCountedObject() {}

      /// Destructor for sub-classes
      virtual ~HttpClient() {}

      /**
       * @brief Factory method for creating concrete instances based on initialisation.
       * @return An instance that uses either ethernet or wifi to connect
       *   to the network.  Callers must delete the returned instance.
       */
      static Ptr create();

      /// Make a socket connection to the specified server on specified port (default 80)
      virtual int16_t connect( const qsense::QString& server, uint16_t port = 80 ) = 0;

#if defined( ARDUINO )
      /// Make a socket connection to the specified ip address on specified port (default 80)
      virtual int16_t connect( const IPAddress& server, uint16_t port = 80 ) = 0;

      /// Check to see if the client is connected to the server
      virtual uint8_t connected() = 0;
#else
      /// Check to see if the client is connected to the server
      virtual bool connected() = 0;
#endif

      /**
       * @brief Perform a GET request using information in the request object.
       * @param request The request object that encapsulates the uri and
       *   other relevant information
       * @return The HTTP response code from server.
       */
      virtual uint16_t get( const HttpRequest& request ) = 0;

      /**
       * @brief Perform a POST request using information in the request object.
       * @param request The request object that encapsulates the uri and
       *   other relevant information
       * @return The HTTP response code from server.
       */
      virtual uint16_t post( const HttpRequest& request ) = 0;

      /**
       * @brief Perform a DELETE request using information in the request object.
       * Named remove to get around delete being keyword.
       * @param request The request object that encapsulates the uri and
       *   other relevant information
       * @return The HTTP response code from server.
       */
      virtual uint16_t remove( const HttpRequest& request ) = 0;

      /**
       * @brief Read a line from the HTTP response.
       *
       * A line can be either a header or content.  Use to process raw
       * HTTP response line by line.
       * @return A line (content until newline character) of text from raw response.
       */
      virtual const qsense::QString readLine() = 0;

      /// Return a map of the HTTP response headers
      virtual HttpRequest::Map readHeaders() = 0;

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
      virtual void writeHeaders( const HttpRequest& request, bool close = true ) = 0;
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

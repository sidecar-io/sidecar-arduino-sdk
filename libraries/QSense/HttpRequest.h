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
#ifndef QSENSE_NET_HTTPREQUEST_H
#define QSENSE_NET_HTTPREQUEST_H

#if defined( ARDUINO )
#include "QSense.h"
#include "../StandardCplusplus/map"
#else
#include <QSense.h>
#include <map>
#endif

namespace qsense
{
  namespace net
  {
    /**
     * @brief A simple class that represents a HTTP request.  Request
     * encapsulates the URI path, any request parameters, header attributes,
     * body etc. as appropriate.
     */
    class HttpRequest
    {
    public:
      /// Map used to represent request parameters and headers.
      typedef std::map<QString,QString> Map;

      /// Constant iterator to access contents of the parameters and headers
      typedef Map::const_iterator Iterator;

      /// Constructor.  Create a request for the specified server resource
      HttpRequest( const QString& uri );

      /// Destructor.  No actions required.
      ~HttpRequest() {}

      /// Return the uri for which this request was created.
      const QString& getUri() const { return uri; }

      /// Return the body to send as part of the request.
      /// For GET requests, this will be empty.
      const QString& getBody() const { return body; }

      /// Set the body to send as part of the request.  This is meant
      /// for use primarily with POST/PUT type requests.
      HttpRequest& setBody( const QString& body );

      /// Add the specified key/value combination as a request parameter
      /// to this request.  If a mapping already exists with the specified
      /// key, it will be replaced with the specified value.
      HttpRequest& setParameter( const QString& key, const QString& value );

      /// Add the specified key/value combination as a request attribute
      /// to this request.  If a mapping already exists with the specified
      /// key, it will be replaced with the specified value.
      HttpRequest& setHeader( const QString& key, const QString& value );

      /// Return the requests parameters as a string.
      /// \b Note: A leading ? symbol will not be added, which is required
      /// for GET requests.  Calls must add if making a GET request.
      const QString getParamters() const;

      /// Return a constant iterator to the beginning of the headers
      /// map.
      Iterator beginHeaders() const { return headers.begin(); }

      /// Return a constant iterator to the beginning of the headers
      /// map.
      Iterator endHeaders() const { return headers.end(); }

    private:
      void toHex( unsigned value, int width, QString& output ) const;
      QString encode( const QString& value ) const;

    private:
      static const QString ILLEGAL;

      const QString uri;
      QString body;
      Map parameters;
      Map headers;
    };

  } // namespace net
} // namespace qsense

#endif // QSENSE_NET_HTTPREQUEST_H

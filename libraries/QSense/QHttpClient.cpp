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
#include "QHttpClient.h"


#if defined( ARDUINO )
#include <WiFiClient.h>
#include "../StandardCplusplus/iostream"
#else
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

#include <Poco/Net/StreamSocket.h>
#endif

namespace qsense
{
  namespace net
  {
    namespace data
    {
      bool networkTypeInitialised = false;
      qsense::net::NetworkType networkType;
    }

#ifndef ARDUINO
    class NetworkClient
    {
    public:
      ~NetworkClient() { socket.close(); }

      int16_t connect( const QString& srvr, uint16_t port )
      {
        socket.connect( Poco::Net::SocketAddress( srvr, port ) );
        socket.setNoDelay( true );
        return 1;
      }

      bool connected()
      {
        return ( socket.impl()->initialized() || ( current < buffer.size() ) );
      }

      void print( const QString& str )
      {
        socket.sendBytes( str.c_str(), str.size() );
      }

      void println() { print( "\r\n" ); }

      void println( const QString& str )
      {
        print( str );
        println();
      }

      void println( int value )
      {
        std::stringstream ss;
        ss << value;
        println( ss.str() );
      }

      int available()
      {
        populate();
        return buffer.size() - current;
      }

#if defined( ARDUINO )
      int timedRead()
#else
      char timedRead()
#endif
      {
        populate();
        return ( current < buffer.size() ) ? buffer[current++] : -1;
      }

      void populate()
      {
        if ( ! socket.impl()->initialized() ) return;

        int count = 1;
        while ( count )
        {
          char bytes[8192];
          count = socket.receiveBytes( bytes, 8192 );
          for ( int i = 0; i < count; ++i ) buffer.push_back( bytes[i] );
        }

        socket.close();
      }

    private:
      uint32_t current = 0;
      std::vector<char> buffer;
      Poco::Net::StreamSocket socket;
    };
#endif

    template <typename C>
    class HttpClientImpl : public HttpClient, C
    {
    public:
      HttpClientImpl() : HttpClient(), C(), server() {}

      int16_t connect( const QString& srvr, uint16_t port )
      {
        server = srvr;
        return C::connect( server.c_str(), port );
      }

#if defined( ARDUINO )
      int16_t connect( const IPAddress& srvr, uint16_t port )
      {
        return C::connect( srvr, port );
      }

      uint8_t connected() { return C::connected(); }
#else
      bool connected() { return C::connected(); }
#endif

      uint16_t get( const HttpRequest& request )
      {
        uint16_t status = 0;

        C::print( F( "GET " ) );

        C::print( request.getUri().c_str() );
        const QString& parameters = request.getParamters();
        if ( parameters.size() > 0 )
        {
          C::print( F( "?" ) );
          C::print( parameters.c_str() );
        }

        C::println( F( " HTTP/1.1" ) );
        C::print( F( "Host: " ) );

        C::println( server.c_str() );
        writeHeaders( request );

        if ( connected() )
        {
          const QString& line = readLine();
          if ( line.length() > 14 ) status =  atoi( line.substr( 9, 3 ).c_str() );
        }

        return status;
      }

      uint16_t post( const HttpRequest& request )
      {
        return doMethod( "POST", request );
      }


      uint16_t remove( const HttpRequest& request )
      {
        return doMethod( "DELETE", request );
      }

      uint16_t doMethod( const QString& method, const HttpRequest& request )
      {
        uint16_t status = 0;

        C::print( method.c_str() );
        C::print( " " );

        C::print( request.getUri().c_str() );
        C::println( F( " HTTP/1.1" ) );
        C::print( F( "Host: " ) );
        C::println( server.c_str() );

        if ( request.getBody().size() > 0 )
        {
          C::print( F( "Content-Length: " ) );
          C::println( request.getBody().length() );
        }

#if DEBUG
        std::cout << F( "  [req] ") << method << F( " " ) <<
          request.getUri() << F( " HTTP/1.1" ) << std::endl;
        std::cout << F( "  [req] Host: " ) << server << std::endl;
        std::cout << F( "  [req] Content-Length: " ) << request.getBody().length() << std::endl;
#endif

        writeHeaders( request );

        const QString& parameters = request.getParamters();
        if ( parameters.size() > 0 ) C::println( parameters.c_str() );

        if ( request.getBody().size() > 0 )
        {
          C::println( request.getBody().c_str() );
#if DEBUG
          std::cout << F( "  [req] " ) << request.getBody() << std::endl;
#endif
        }
        C::println();

        if ( connected() )
        {
          const QString& line = readLine();
          if ( line.length() > 14 ) status =  atoi( line.substr( 9, 3 ).c_str() );
#if DEBUG
          std::cout << F( "  [resp] " ) << line << std::endl;
#endif
        }

        return status;
      }


      const QString readLine()
      {
        QString line;

        while ( connected() )
        {
          int bytes = C::available();
#if defined( ARDUINO )
          int c = ' ';
#else
          char c = ' ';
#endif

          if ( bytes )
          {
            line.reserve( bytes );
            c = C::timedRead();

            while ( c >= 0 && c != '\n' )
            {
              if ( c != '\r' ) line += static_cast<char>( c );
              c = C::timedRead();
            }
          }

          if ( c == '\n' ) break;
        }

        return line;
      }


      HttpRequest::Map readHeaders()
      {
        HttpRequest::Map m;

        while ( connected() )
        {
          const QString& line = readLine();

          std::size_t found = line.find( ":" );
          if ( found != QString::npos )
          {
            const QString& key = line.substr( 0, found );
            const QString& value = line.substr( found + 2 );
            m.insert( std::pair<QString,QString>( key, value ) );
          }

          if ( line.length() <= 1 ) break;
        }

        return m;
      }


      const QString readBody()
      {
        QString content;

        if ( connected() ) readHeaders();
        while ( connected() )
        {
          const QString& line = readLine();
          if ( line.length() == 0 ) break;
          content.append( line );
        }

        return content;
      }


    protected:
      void writeHeaders( const HttpRequest& request, bool close = true )
      {
        for ( HttpRequest::Iterator iter = request.beginHeaders();
            iter != request.endHeaders(); ++iter )
        {
          C::print( iter->first.c_str() );
          C::print( ": " );
          C::println( iter->second.c_str() );
#if DEBUG
          std::cout << F( "  [req] " ) << iter->first << ": " << iter->second << std::endl;
#endif
        }

        if ( close )
        {
          C::println( F( "Connection: close" ) );
#if DEBUG
          std::cout << F( "  [req] " ) << F( "Connection: close" ) << std::endl << std::endl;
#endif
        }
        C::println();
      }


    private:
      qsense::QString server;
    };
  }
}

using qsense::QString;
using qsense::net::HttpClient;


HttpClient::Ptr HttpClient::create()
{
#if defined ( ARDUINO )
  switch ( qsense::net::data::networkType )
  {
    case qsense::net::Ethernet:
      return new qsense::net::HttpClientImpl<EthernetClient>;
      break;
    case qsense::net::WiFi:
      return new qsense::net::HttpClientImpl<WiFiClient>;
      break;
    default: return Ptr();
  }
#else
  return new qsense::net::HttpClientImpl<qsense::net::NetworkClient>;
#endif
}


void qsense::net::initNetworkType( qsense::net::NetworkType type )
{
  if ( ! qsense::net::data::networkTypeInitialised )
  {
    qsense::net::data::networkType = type;
    qsense::net::data::networkTypeInitialised = true;
  }
}

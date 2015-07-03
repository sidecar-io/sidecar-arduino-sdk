#include "QHttpClient.h"

#include <EthernetClient.h>
#include <WiFiClient.h>

using qsense::QString;
using qsense::net::HttpClient;

namespace qsense
{
  namespace net
  {
    namespace data
    {
      bool networkTypeInitialised = false;
      qsense::net::NetworkType networkType;
    }

    class HttpClientOverEthernet : public HttpClient, EthernetClient
    {
    public:
      HttpClientOverEthernet() : EthernetClient(), server() {}

      int16_t connect( const QString& srvr, uint16_t port )
      {
        server = srvr;
        return EthernetClient::connect( server.c_str(), port );
      }

      uint8_t connected() { return EthernetClient::connected(); }

      uint16_t get( const QString& uri, const Headers& headers )
      {
        uint16_t status = 0;

        print( F( "GET " ) );
        print( uri.c_str() );
        println( F( " HTTP/1.1" ) );
        print( F( "Host: " ) );
        println( server.c_str() );
        writeHeaders( headers );

        if ( connected() )
        {
          const QString& line = readLine();
          if ( line.length() > 14 ) status =  atoi( line.substr( 9, 3 ).c_str() );
        }

        return status;
      }

      uint16_t post( const QString& uri, const Headers& headers, const QString& body )
      {
        uint16_t status = 0;

        print( F( "POST " ) );
        print( uri.c_str() );
        println( F( " HTTP/1.1" ) );
        print( F( "Host: " ) );
        println( server.c_str() );

        if ( body.size() > 0 )
        {
          print( F( "Content-Length: " ) );
          println( body.length() );
        }

        writeHeaders( headers );
        if ( body.size() > 0 ) println( body.c_str() );
        println();

        if ( connected() )
        {
          const QString& line = readLine();
          if ( line.length() > 14 ) status =  atoi( line.substr( 9, 3 ).c_str() );
        }

        return status;
      }


      const QString readLine()
      {
        QString line;

        while ( connected() )
        {
          int bytes = available();
          int16_t c = ' ';

          if ( bytes )
          {
            line.reserve( bytes );
            c = timedRead();
            while ( c >= 0 && c != '\n' )
            {
              line += static_cast<char>( c );
              c = timedRead();
            }
          }

          if ( c == '\n' ) break;
        }

        return line;
      }


      Headers readHeaders()
      {
        Headers m;

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
      void writeHeaders( const Headers& headers, bool close = true )
      {
        for ( Headers::const_iterator iter = headers.begin(); iter != headers.end(); ++iter )
        {
          print( iter->first.c_str() );
          print( ": " );
          println( iter->second.c_str() );
        }

        if ( close ) println( F( "Connection: close" ) );
        println();
      }


    private:
      qsense::QString server;
    };

    class HttpClientOverWiFi : public HttpClient, WiFiClient
    {
    public:
      HttpClientOverWiFi() : WiFiClient(), server() {}

      int16_t connect( const QString& srvr, uint16_t port )
      {
        server = srvr;
        return WiFiClient::connect( server.c_str(), port );
      }

      uint8_t connected() { return WiFiClient::connected(); }

      uint16_t get( const QString& uri, const Headers& headers )
      {
        uint16_t status = 0;

        print( F( "GET " ) );
        print( uri.c_str() );
        println( F( " HTTP/1.1" ) );
        print( F( "Host: " ) );
        println( server.c_str() );
        writeHeaders( headers );

        if ( connected() )
        {
          const QString& line = readLine();
          if ( line.length() > 14 ) status =  atoi( line.substr( 9, 3 ).c_str() );
        }

        return status;
      }

      uint16_t post( const QString& uri, const Headers& headers, const QString& body )
      {
        uint16_t status = 0;

        print( F( "POST " ) );
        print( uri.c_str() );
        println( F( " HTTP/1.1" ) );
        print( F( "Host: " ) );
        println( server.c_str() );

        if ( body.size() > 0 )
        {
          print( F( "Content-Length: " ) );
          println( body.length() );
        }

        writeHeaders( headers );
        if ( body.size() > 0 ) println( body.c_str() );
        println();

        if ( connected() )
        {
          const QString& line = readLine();
          if ( line.length() > 14 ) status =  atoi( line.substr( 9, 3 ).c_str() );
        }

        return status;
      }


      const QString readLine()
      {
        QString line;

        while ( connected() )
        {
          int bytes = available();
          int16_t c = ' ';

          if ( bytes )
          {
            line.reserve( bytes );
            c = timedRead();
            while ( c >= 0 && c != '\n' )
            {
              line += static_cast<char>( c );
              c = timedRead();
            }
          }

          if ( c == '\n' ) break;
        }

        return line;
      }


      Headers readHeaders()
      {
        Headers m;

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
      void writeHeaders( const Headers& headers, bool close = true )
      {
        for ( Headers::const_iterator iter = headers.begin(); iter != headers.end(); ++iter )
        {
          print( iter->first.c_str() );
          print( ": " );
          println( iter->second.c_str() );
        }

        if ( close ) println( F( "Connection: close" ) );
        println();
      }


    private:
      qsense::QString server;
    };
  }
}


HttpClient* HttpClient::create()
{
  switch ( qsense::net::data::networkType )
  {
    case qsense::net::Ethernet:
      return new qsense::net::HttpClientOverEthernet;
      break;
    case qsense::net::WiFi:
      return new qsense::net::HttpClientOverWiFi;
      break;
    default: return NULL;
  }
}


void qsense::net::initNetworkType( qsense::net::NetworkType type )
{
  if ( ! qsense::net::data::networkTypeInitialised )
  {
    qsense::net::data::networkType = type;
    qsense::net::data::networkTypeInitialised = true;
  }
}

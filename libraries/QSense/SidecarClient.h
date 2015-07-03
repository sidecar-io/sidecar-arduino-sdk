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
      /// Publish the specified event to the Sidecar Event API
      bool publish( const qsense::Event& event ) const;

      /// Initialise the API with the key and secret used to sign messages.
      static void init( const qsense::QString& apiKey,
        const qsense::QString& apiSecret );

    private:
      qsense::QString md5( const qsense::QString& event ) const;
      qsense::QString signature(
        const qsense::QString& method,
        const qsense::QString& uri,
        const qsense::QString& date,
        const qsense::QString& hash ) const;
    };

  } // namespace net
} // namespace qsense

#endif // QSENSE_NET_SIDECARCLIENT_H

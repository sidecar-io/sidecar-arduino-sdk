#ifndef QSENSE_EVENT_H
#define QSENSE_EVENT_H

#if defined( ARDUINO )
#include "QSense.h"
#include "Location.h"
#include "Reading.h"
#include "../StandardCplusplus/vector"
#else
#include <QSense.h>
#include <Location.h>
#include <Reading.h>
#include <vector>
#endif

namespace qsense
{
  /**
   * @brief A simple class that encapsulates an event sent to Sidecar.
   * Events are holders for readings.  Events can be serialised to JSON
   * using the {@link #toString} method.
   */
  class Event
  {
  public:
    /// The vector of readings encapsulated in this event.
    typedef std::vector<qsense::Reading> Readings;

    /// The vector of tags associated with this event.
    typedef std::vector<qsense::QString> Tags;

    /// Iterator for the readings encapsulated in this event.
    typedef Readings::const_iterator ReadingsIterator;

    /// Iterator for the tags associated with this event.
    typedef Tags::const_iterator TagsIterator;

    /// Default constructor.  Uses default location set through {@link #init}
    Event();

    /// Create a new event with the specified location
    Event( const qsense::Location& location );

    /// Destructor.  No actions required
    ~Event() {}

    /// Add the specified reading to this event.
    Event& add( const qsense::Reading& reading );

    /// Add the specified tag to this event.
    Event& add( const qsense::QString& tag );

    /// Operator for adding a reading to the event
    Event& operator += ( const qsense::Reading& reading )
    {
      add( reading );
      return *this;
    }

    /// Operator for adding a tag to the event
    Event& operator += ( const qsense::QString& tag )
    {
      add( tag );
      return *this;
    }

    /// Return the number of readings in this event.
    std::size_t numberOfReadings() const { return readings.size(); }

    /// Return the number of tags associated with this event.
    std::size_t numberOfTags() const { return tags.size(); }

    /// Return a constant iterator to the beginning of the readings vector
    ReadingsIterator beginReadings() const { return readings.begin(); }

    /// The end of the readings vector to check in loops.
    ReadingsIterator endReadings() const { return readings.end(); }

    /// Return a constant iterator to the beginning of the tags vector
    TagsIterator beginTags() const { return tags.begin(); }

    /// The end of the tags vector to check in loops.
    TagsIterator endTags() const { return tags.end(); }

    /**
     * @brief operator [] Retrieve the reading at specified index.
     * Will throw exception if index is out of bounds.  Check the
     * {@link size} of the container before using this operator.
     *
     * @param index The index into the vector of readings.
     * @return The reading at the specified index.
     */
    const qsense::Reading& operator [] ( std::size_t index ) const
    {
      return readings[index];
    }

    /// Return the location used by this event
    const qsense::Location& getLocation() const { return location; }

    /// Serialise the event to JSON
    const qsense::QString toString() const;

    /**
     * @brief Initialise the Event API
     * @param deviceId The deviceId to use.  No way at present to retrieve using API
     * @param stream The stream identifier to use with Sidecar
     * @param location A default location to use.  No location tracking at present
     */
    static void init( const qsense::QString& deviceId,
      const qsense::QString& stream, const qsense::Location& location );

  private:
    Readings readings;
    Tags tags;
    Location location;
  };

  /// Serialise the specified event as JSON to the output stream
  std::ostream& operator << ( std::ostream& os, const Event& event );

} // namespace qsense

#endif // QSENSE_EVENT_H

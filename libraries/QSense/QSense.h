#ifndef QSENSE_QSESNSE_H
#define QSENSE_QSESNSE_H

#if defined( ARDUINO )
#include "Arduino.h"
#include "../StandardCplusplus/StandardCplusplus.h"
#include "../StandardCplusplus/string"
#else
#include <string>
#endif

/**
 * The namespace for the QSense Sidecar Library.
 */
namespace qsense
{
  /**
   * Not really necessary any more.  Initially had it to use String
   * class from Arduino library, but that never seems to work when
   * used for hashing etc.
   */
  typedef std::string QString;

  /** 8-bit unsigned char */
  typedef unsigned char Byte;
}

#endif // QSENSE_QSESNSE_H

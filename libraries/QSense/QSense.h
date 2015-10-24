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
#ifndef QSENSE_QSESNSE_H
#define QSENSE_QSESNSE_H

#if defined( ARDUINO )
#include "Arduino.h"
#include "SPI.h"
#include "../StandardCplusplus/StandardCplusplus.h"
#include "../StandardCplusplus/serstream"
#include "../StandardCplusplus/string"
#else
#include <cstdint>
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

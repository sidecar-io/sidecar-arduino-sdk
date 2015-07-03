/*
  Copyright (C) 1999 Aladdin Enterprises.  All rights reserved.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  L. Peter Deutsch
  ghost@aladdin.com

 */
/*$Id: MD5.h 5782 2015-05-24 16:50:48Z rakesh $ */
/*
  Independent implementation of MD5 (RFC 1321).

  This code implements the MD5 Algorithm defined in RFC 1321.
  It is derived directly from the text of the RFC and not from the
  reference implementation.

  The original and principal author of md5.h is L. Peter Deutsch
  <ghost@aladdin.com>.  Other authors are noted in the change history
  that follows (in reverse chronological order):

  1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
  1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5);
  added conditionalization for C++ compilation from Martin
  Purschke <purschke@bnl.gov>.
  1999-05-03 lpd Original version.
 */
#ifndef QSENSE_HASH_MD5_H
#define QSENSE_HASH_MD5_H

#if defined( ARDUINO )
#include "QSense.h"
#else
#include <QSense.h>
#endif

namespace qsense
{
  namespace hash
  {
    /**
     * @brief Class for generating MD5 hashes
     */
    class MD5
    {
    public:
    #define MD5_HASH_LENGTH 16
      /// Default constructor.
      MD5() : context( NULL ) {}

      /// Destructor.  Destroys the context
      ~MD5() { delete context; }

      /// 8-bit byte
      typedef qsense::Byte Byte;

      /// 32-bit word
      typedef uint32_t Word;

      /** Compute the MD5 digest for the specified data of length nbytes into digest */
      void compute( const Byte* data, Word nbytes, Byte digest[MD5_HASH_LENGTH] );

      /** Compute MD5 digest for specified data and return base64 encoded string */
      qsense::QString compute( const qsense::QString& input );

    private:
      /** MD5 encoding context */
      struct Context
      {
        /* state (ABCD) */
        Word state[4];
        /* number of bits, modulo 2^64 (lsb first) */
        Word count[2];
        /* input buffer */
        Byte buffer[64];
      };

      /** MD5 initialization. Begins an MD5 operation, writing a new context.  */
      void init();

      /**
       * MD5 block update operation. Continues an MD5 message-digest
       * operation, processing another message block, and updating the
       * context.
       */
      void update( const Byte* input, Word inputLen );

      /**
       * MD5 finalization. Ends an MD5 message-digest operation, writing the
       * the message digest and zeroizing the context.
       */
      void finish( Byte digest[16] );

      /** MD5 basic transformation. Transforms state based on block.  */
      void transform( Word state[4], const MD5::Byte block[64] );

      /**
       * Encodes input (Word) into output (unsigned char). Assumes length
       * is a multiple of 4.
       */
      void encode( Byte* output, const Word* input, Word length );

      /**
       * Decodes input (Byte) into output (Word). Assumes length is
       * a multiple of 4.
       */
      void decode( Word *output, const Byte* input, Word length );

    private:
      Context* context;
    };
  }
}

#endif // QSENSE_HASH_MD5_H

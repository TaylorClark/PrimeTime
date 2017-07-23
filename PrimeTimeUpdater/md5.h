// MD5.CC - source code for the C++/object oriented translation and 
//          modification of MD5.

// Translation and modification (c) 1995 by Mordechai T. Abzug 

// This translation/ modification is provided "as is," without express or 
// implied warranty of any kind.

// The translator/ modifier does not claim (1) that MD5 will do what you think 
// it does; (2) that this translation/ modification is accurate; or (3) that 
// this software is "merchantible."  (Language for this disclaimer partially 
// copied from the disclaimer below).

/* based on:

   MD5.H - header file for MD5C.C
   MDDRIVER.C - test driver for MD2, MD4 and MD5

   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.

*/

#include "Base/Types.h"
#include <stdio.h>
#include <fstream>
#include <iostream>

class MD5
{
public:
// methods for controlled operation:
  MD5              ();  // simple initializer
  void  update     (unsigned char *input, unsigned int input_length);
  void  update     (std::istream& stream);
  void  update     (FILE *file);
  void  update     (std::ifstream& stream);
  void  finalize   ();

// constructors for special circumstances.  All these constructors finalize
// the MD5 context.
  MD5              (unsigned char *string); // digest string, finalize
  MD5              (std::istream& stream);       // digest stream, finalize
  MD5              (FILE *file);            // digest file, close, finalize
  MD5              (std::ifstream& stream);      // digest stream, close, finalize

// methods to acquire finalized result
  unsigned char    *raw_digest ();  // digest as a 16-byte binary array
  char *            hex_digest ();  // digest as a 33-byte ascii-hex string
  friend std::ostream& operator <<( std::ostream&, MD5 context );



private:

// next, the private data:
  uint32 state[4];
  uint32 count[2];     // number of *bits*, mod 2^64
  uint8 buffer[64];   // input buffer
  uint8 digest[16];
  uint8 finalized;

// last, the private methods, mostly static:
  void init             ();               // called by all constructors
  void transform        (uint8 *buffer);  // does the real update work.  Note 
                                          // that length is implied to be 64.

  static void encode    (uint8 *dest, uint32 *src, uint32 length);
  static void decode    (uint32 *dest, uint8 *src, uint32 length);
  static void memcpy    (uint8 *dest, uint8 *src, uint32 length);
  static void memset    (uint8 *start, uint8 val, uint32 length);

  static inline uint32  rotate_left (uint32 x, uint32 n);
  static inline uint32  F           (uint32 x, uint32 y, uint32 z);
  static inline uint32  G           (uint32 x, uint32 y, uint32 z);
  static inline uint32  H           (uint32 x, uint32 y, uint32 z);
  static inline uint32  I           (uint32 x, uint32 y, uint32 z);
  static inline void   FF  (uint32& a, uint32 b, uint32 c, uint32 d, uint32 x, 
			    uint32 s, uint32 ac);
  static inline void   GG  (uint32& a, uint32 b, uint32 c, uint32 d, uint32 x, 
			    uint32 s, uint32 ac);
  static inline void   HH  (uint32& a, uint32 b, uint32 c, uint32 d, uint32 x, 
			    uint32 s, uint32 ac);
  static inline void   II  (uint32& a, uint32 b, uint32 c, uint32 d, uint32 x, 
			    uint32 s, uint32 ac);

};

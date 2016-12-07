#ifndef DANVIL_CT_TYPES_H_
#define DANVIL_CT_TYPES_H_
//---------------------------------------------------------------------------
#include <stdint.h>
//---------------------------------------------------------------------------
//--- Fixed sized integers typedefs
//---------------------------------------------------------------------------

/** unsigned 8 bit integer */
typedef uint8_t ui08;
/** signed 8 bit integer */
typedef int8_t i08;

/** unsigned 16 bit integer */
typedef uint16_t ui16;
/** signed 16 bit integer */
typedef int16_t i16;

/*+ unsigned 32 bit integer */
typedef uint32_t ui32;
/** signed 32 bit integer */
typedef int32_t i32;

/** unsigned 64 bit integer */
typedef uint64_t ui64;
/** signed 64 bit integer */
typedef uint64_t i64;

//---------------------------------------------------------------------------
//--- Integers typedefs
//---------------------------------------------------------------------------

/** 8 bit unsigned integer */
typedef unsigned char uchar;

/** Unsigned integer with 16, 32 or 64 bits
 * This depends on the compiler!
 * Should be 32 bit for LLP64 and LP64 (common) and 64 bit for ILP64 and SILP64!
 */
typedef unsigned int uint; // <-- defined by stdint.h

//typedef unsigned long ulong; // <-- defined by stdint.h

//---------------------------------------------------------------------------
#endif

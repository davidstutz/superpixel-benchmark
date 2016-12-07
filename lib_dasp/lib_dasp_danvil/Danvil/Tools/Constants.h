#ifndef DANVIL_CTMATH_MATH_CONSTANTS
#define DANVIL_CTMATH_MATH_CONSTANTS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------

/** Square root of two */
const long double C_SQRT_2 = 1.414213562373095048801688724209698078569671875376948073176679738;

/** The circle constant pi
 * See http://www.cs.arizona.edu/icon/oddsends/pi.htm
 */
const long double C_PI = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111;

const long double C_2_PI = 2 * C_PI;
const long double C_3_PI = 3 * C_PI;
const long double C_4_PI = 4 * C_PI;
const long double C_PI_2 = C_PI / 2;
const long double C_PI_3 = C_PI / 3;
const long double C_PI_4 = C_PI / 4;
const long double C_PI_6 = C_PI / 6;
const long double C_INV_PI = 1 / C_PI;
const long double C_INV_2_PI = 1 / (2 * C_PI);

const long double C_SQRT_PI = 1.772453850905516027298167483341145182797549456122387128213807790;
const long double C_SQRT_2_PI = 2.506628274631000502415765284811045253006986740609938316629923576;

/** Converts radians into degrees */
const long double C_RAD_TO_DEG = 180 / C_PI;

/** Converts degree into radians */
const long double C_DEG_TO_RAD = C_PI / 180;

/** Some degrees in radians */
const long double C_RAD_30 = C_PI_6; // 30/180*PI = PI/6
const long double C_RAD_45 = C_PI_4; // 45/180*PI = PI/4
const long double C_RAD_60 = C_PI_3; // 60/180*PI = PI/3
const long double C_RAD_90 = C_PI_2; // 90/180*PI = PI/2
const long double C_RAD_180 = C_PI;

/** Eulers constant e
 * See http://www.cs.arizona.edu/icon/oddsends/e.htm
 */
const long double C_EULER = 2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746639193200305992181741359662904357290033429526059563;

//---------------------------------------------------------------------------

template<typename K>
K Pi() {
	return static_cast<K>(C_PI);
}

template<typename K>
K TwoPi() {
	return static_cast<K>(C_2_PI);
}

template<typename K>
K E() {
	return static_cast<K>(C_EULER);
}

//---------------------------------------------------------------------------

///** Die Kreiszahl Pi mit 64 Bit Genauigkeit */
//const double PI = 3.1415926535897932384626433832795;
///** Die Kreiszahl Pi mit 32 Bit Genauigkeit */
//const float PIf = (float)PI;
//
///** 2*Pi mit 64 Bit Genauigkeit */
//const long double PI_TWO = 2.0 * PI;
///** 2*Pi mit 32 Bit Genauigkeit */
//const float PIf_TWO = (float)PI_TWO;
//
///** Pi / 2 mit 64 Bit Genauigkeit */
//const long double PI_HALF = 0.5 * PI;
///** Pi / 2 mit 32 Bit Genauigkeit */
//const float PIf_HALF = (float)PI_HALF;
//
///** Die Eulersche Zahl e mit 64 Bit Genauigkeit */
//const double EULER = 2.7182818284590452353602874713527;
///** Die Eulersche Zahl e mit 32 Bit Genauigkeit */
//const float EULERf = (float)EULER;
//
///** Square root of two with 64 bit precision */
//const double SQRTTWO = 1.4142135623730950488016887242097;
///** Square root of two with 32 bit precision */
//const float SQRTTWOf = (float)SQRTTWO;

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif

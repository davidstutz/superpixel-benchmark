#ifndef Elimination_SSE_h_
#define Elimination_SSE_h_

#define ELIMINATION_SSE 0



// If SSE optimizations are enabled via the above #define, make sure the
// hardware/compiler has support for SSE instructions.  If so, we need to
// define a cross platform macro to align variables to sixteen byte multiple
// addresses (for i/o efficiency between the SSE registers).

#if ELIMINATION_SSE && defined( __SSE__ )
	#ifdef __GNUC__
	#define ELIMINATION_SSE_ALIGN __attribute__((__aligned__(16)))
	#endif	
	
	#ifdef _WIN32
	#define ELIMINATION_SSE_ALIGN __declspec(align(16))
	#endif
#else
	#define ELIMINATION_SSE_ALIGN
#endif




#endif // Elimination_SSE_h_

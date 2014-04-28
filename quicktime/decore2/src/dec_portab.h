#ifndef DEC_PORTAB_H_
#define DEC_PORTAB_H_

#ifdef WIN32

#include <windows.h>

#define int8_t char
#define uint8_t unsigned char
#define int16_t short
#define uint16_t unsigned short
#define int32_t int
#define uint32_t unsigned int
#define int64_t __int64
#define uint64_t unsigned __int64

#define DEBUG(S) OutputDebugString((S));
#define DEBUGI(S,I) { char tmp[100]; wsprintf(tmp, "%s %i\n", (S), (I)); OutputDebugString(tmp); }
#define DEBUGI2(X,A,B) { char tmp[100]; wsprintf(tmp, "%s %i %i\n", (X), (A), (B)); OutputDebugString(tmp); }
#define DEBUGI3(X,A,B,C){ char tmp[1000]; wsprintf(tmp,"%s %i %i %i",(X),(A), (B), (C)); OutputDebugString(tmp); }
#define DEBUGI8(X,A,B,C,D,E,F,G,H){ char tmp[1000]; wsprintf(tmp,"%s %i %i %i %i %i %i %i %i",(X),(A),(B),(C),(D),(E),(F),(G),(H)); OutputDebugString(tmp); }


#define EMMS() __asm emms

// needed for bitstream.h
#define BSWAP(a) __asm mov eax,a __asm bswap eax __asm mov a, eax

// needed for timer.c
static __inline int64_t read_counter() {
	int64_t ts;
	uint32_t ts1, ts2;

	__asm {
		rdtsc
		mov  ts1, eax
		mov  ts2, edx
	}
	
	ts = ((uint64_t) ts2 << 32) | ((uint64_t) ts1);
    
	return ts;
}

#elif defined(LINUX)

#include <inttypes.h>

// needed for bitstream.h
#define BSWAP(a) __asm__ ( "bswapl %0\n" : "=r" (a) : "0" (a) )

// needed for timer.c
static __inline int64_t read_counter() {
    int64_t ts;
    uint32_t ts1, ts2;

    __asm__ __volatile__("rdtsc\n\t":"=a"(ts1), "=d"(ts2));

    ts = ((uint64_t) ts2 << 32) | ((uint64_t) ts1);

    return ts;
}

#define DEBUG(S) {}
#define DEBUGI(S,I) {}
#define DEBUGI2(X,A,B) {}
#define DEBUGI3(X,A,B,C) {}
#define DEBUGI8(X,A,B,C,D,E,F,G,H) {}
#define EMMS() {}

#else // OTHER OS

#include <inttypes.h>

// needed for bitstream.h
#define BSWAP(a) \
	 ((a) = ( ((a)&0xff)<<24) | (((a)&0xff00)<<8) | (((a)>>8)&0xff00) | (((a)>>24)&0xff))

// rdtsc command most likely not supported,
// so just dummy code here
static __inline int64_t read_counter() {
	return 0;
}

#define DEBUGI2(X,A,B) {  }

#endif

#endif // _PORTAB_H_


#include "MersenneTwister.h"

#ifdef __cplusplus
extern "C" {
#endif

/* initializes mt[N] with a seed */
void init_genrand(s_mersenne_twister_status *p_status, unsigned long s)
{
	p_status->mt[0]= s & 0xffffffffUL;
	for (p_status->mti=1; p_status->mti<N; p_status->mti++) {
		p_status->mt[p_status->mti] = 
			(1812433253UL * (p_status->mt[p_status->mti-1] ^ (p_status->mt[p_status->mti-1] >> 30)) + p_status->mti); 
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array mt[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
		p_status->mt[p_status->mti] &= 0xffffffffUL;
		/* for >32 bit machines */
	}
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(s_mersenne_twister_status *p_status)
{
	unsigned long y;
	static unsigned long mag01[2]={0x0UL, MATRIX_A};
	/* mag01[x] = x * MATRIX_A  for x=0,1 */

	if (p_status->mti >= N) { /* generate N words at one time */
		int kk;

		if (p_status->mti == N+1)   /* if init_genrand() has not been called, */
			init_genrand(p_status, 5489UL); /* a default initial seed is used */

		for (kk=0;kk<N-M;kk++) {
			y = (p_status->mt[kk]&UPPER_MASK)|(p_status->mt[kk+1]&LOWER_MASK);
			p_status->mt[kk] = p_status->mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		for (;kk<N-1;kk++) {
			y = (p_status->mt[kk]&UPPER_MASK)|(p_status->mt[kk+1]&LOWER_MASK);
			p_status->mt[kk] = p_status->mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		y = (p_status->mt[N-1]&UPPER_MASK)|(p_status->mt[0]&LOWER_MASK);
		p_status->mt[N-1] = p_status->mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

		p_status->mti = 0;
	}

	y = p_status->mt[p_status->mti++];

	/* Tempering */
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);

	return y;
}

#ifdef __cplusplus
}
#endif
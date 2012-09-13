#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

typedef struct _s_mersenne_twister_status
{
	unsigned long mt[N]; /* the array for the state vector  */
	int mti; /* mti==N+1 means mt[N] is not initialized */
} s_mersenne_twister_status, *PMERSENNETWISTERSTATUS;

/* initializes mt[N] with a seed */
void init_genrand(PMERSENNETWISTERSTATUS p_status, unsigned long s);

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(PMERSENNETWISTERSTATUS p_status);

#ifdef __cplusplus
}
#endif
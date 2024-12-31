#ifndef MATHS_H
#define MATHS_H

#include "stdlib.h"
#include "math.h"

#ifndef PI
    #define PI 3.14159265358979323846f
#endif
#ifndef R_RAND_MAX
    #define R_RAND_MAX 0.00003051850947599719f
#endif

float Q_rsqrt( float number )
{
        long i;
        float x2, y;
        const float threehalfs = 1.5F;
 
        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking
        i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
 
        return y;
}

float randfloat(){
    return (float)rand() * R_RAND_MAX;
}

#endif
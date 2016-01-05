#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes

float veclen ( int n , float *v )
{
   float sum = 0.0 ;
   while (n--)
      sum += v[n] * v[n] ;
   return sum ;
}

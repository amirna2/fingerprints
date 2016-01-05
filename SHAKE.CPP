#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes

void shake ( int nvars , float *center , float *x , float temp )
{
   float r ;

/*
   Recall that the variance of a uniform deviate on 0-1 is 1/12.
   Adding four such random variables multiplies the variance by 4,
   while dividing by 2 divides the variance by 4.
*/

   temp *= 3.464101615 / (2. * longrandmax () ) ; // SQRT(12)=3.464...

   while (nvars--) {
      r = (float) longrand() + (float) longrand() -
	  (float) longrand() - (float) longrand() ;
      *x++ = *center++ + temp * r ;
      }
}

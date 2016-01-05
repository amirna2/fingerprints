#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes

/*
   This uses a piecewise linear approximation.  The routine
   'act_func_init' must be called before the 'act_func' function
   in order to initialize the interpolation table.
*/

static float f_factor, f_f[LTAB_LENGTH], f_d[LTAB_LENGTH] ;

void act_func_init ()
{
   int i ;

   f_factor = (float) (LTAB_LENGTH - 1) / (float) LTAB_MAX ;
   for (i=0 ; i<LTAB_LENGTH ; i++) {
      f_f[i] = 1.0 / (1.0 + exp ( - ((float) i) / f_factor )) ;
      if (i)
         f_d[i-1] = f_f[i] - f_f[i-1] ;
      }
}

float act_func ( float x )
{
   int i ;
   float xd ;

   if (x >= 0.0) {                   // One symmetric half
      xd = x * f_factor ;            // Find location in table
      i = (int) xd ;                 // Subscript in table
      if (i >= (LTAB_LENGTH - 1))    // If outside table
         return f_f[LTAB_LENGTH-1] ; // hold at highest entry
      return f_f[i] + f_d[i] * (xd - i) ; // Else interpolate
      }

   else {                            // Other symmetric half
      xd = -x * f_factor ;
      i = (int) xd ;
      if (i >= (LTAB_LENGTH - 1))
         return 1.0 - f_f[LTAB_LENGTH-1] ;
      return 1.0 - (f_f[i] + f_d[i] * (xd - i)) ;
      }
}

/*
   ACTDERIV - Compute the derivative of the activation function.
   Note that it is written as a function of the output activation,
   f(net), rather than net itself.
   This is all that is needed for the logistic activation function.
   If the user tries other functions, appropriate reparameterization
   must be done.  To make this dependent on the net would
   necessitate storing the nets, which is otherwise unnecessary.
*/

float actderiv ( float f )
{
   return f * (1.0 - f) ;   // Logistic derivative
}

/*
   INVERSE_ACT - Compute the inverse of the activation function.
*/

float inverse_act ( float f )
{
   return -log ( 1.0 / f - 1.0 ) ; // Inverse logistic function
}


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes

float activity ( float *input , float *coefs , int n )
{
   int i ;
   float sum ;

   sum = dotprod ( n , input , coefs ) ;
   sum += coefs[n] ;      // Bias term

   return act_func ( sum ) ;
}

// Hebbian activity function for Hopfield network

float hop_activity (int in ,float *input, float *weights, int n )
{
  int i;
  float sum=0.0;

  for (i=0;i<n;i++)
  {
    sum += input[in] * weights[i];
  }

  if (sum > 0.0)    sum = 1.0;
  if (sum <=0.0)    sum = 0.0;
  return sum;
  //return act_func(sum);

}


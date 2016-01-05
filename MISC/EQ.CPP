/******************************************************************************/
/*                                                                            */
/* EQ - Subroutine to generate equilateral input transformation matrix        */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>

void equilat (
   int n ,       // Number of values, at least 2
   float *mat , // Output n by n-1 matrix of activations
   int range     // 0 for .1 to .9, 1 for -1 to 1
   )
{
   int i, j, k, nm1 ;
   float r, f ;

/*
   Initialize mat[0,0]=-1 and mat[1,0]=1
*/

   nm1 = n - 1 ;
   mat[0] = -1.0 ;
   mat[nm1] = 1.0 ;

/*
   Recursively build rest of matrix
*/

   for (k=2 ; k<n ; k++) {

      // Scale matrix so far
      r = (float) k ;
      f = sqrt ( r * r - 1.0 ) / r ; // Scaling factor
      for (i=0 ; i<k ; i++) {        // Rows of mat so far
         for (j=0 ; j<k-1 ; j++)     // And cols
            mat[i*nm1+j] *= f ;      // Scale it
         }

      // Append a column of all -1/k
      r = -1.0 / r ;                 // Value of new column
      for (i=0 ; i<k ; i++)          // Rows of mat so far
         mat[i*nm1+k-1] = r ;        // New column on right

      // Append new row of all 0's except 1 at end
      for (i=0 ; i<k-1 ; i++)
         mat[k*nm1+i] = 0.0 ;
      mat[k*nm1+k-1] = 1.0 ;
      }

/*
   The above algorithm left the values in -1 to 1.
   If they need to be rescaled .1 to .9, do it now.
*/

   if (range)
      return ;

   i = n * nm1 ;
   while (i--)
      mat[i] = 0.4 * mat[i] + 0.5 ;
}

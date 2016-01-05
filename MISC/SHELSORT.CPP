/******************************************************************************/
/*                                                                            */
/*  SHELSORT.CPP - Standard shellsort from Sedgewick 'Algorithms'             */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>

void shellsort (
      int n ,    // Length of x array
      float *x  // Data to be sorted ascending
      )
{
   int i, j, width, other ;
   float sentinel ;

/*
   Generate (in reverse order) a good sequence of sorting widths
*/

   for (width=1 ; width<n ;)    // Until we get large enough
      width = 3 * width + 1 ;   // avoid common factors in widths

   do {             // Outer loop does each of a diminishing sequence of widths
      width /= 3 ;  // Next shell width

      for (i=width ; i<n ; i++) {  // Do each of the shells

         sentinel = x[i] ;
         j = i ;

          for (;;) {
            other = j - width ;
	    if ((other < 0)  ||  (x[other] <= sentinel))
               break ;
            x[j] = x[other] ;
	    j = other ;
            }

         x[j] = sentinel ;
         } // For each shell of this width

      } while (width > 1) ;  // Outer loop does diminishing sequence of widths
}  

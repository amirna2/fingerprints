/******************************************************************************/
/*                                                                            */
/*  PNN - Classify using probabilistic neural network                         */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>

int pnn (
      int nvars ,         // Number of variables in x
      int npops ,         // Number of populations
      int *ntrain ,       // npops vector of number of samples of each pop
      double **tsets ,    // npops vector of ptrs to training sets
      int model ,         // Distance/Weight model (see comments, normally 1)
      double sigma ,      // Scale parameter
      double *unknown ,   // nvars vector to be classified
      double *outputs     // npops vector of classification functions values
      )
{
   int i, n, ipop, icase, ibest ;
   double diff, dist, *tptr, *optr, best ;

   sigma *= (double) nvars ;              // Keep sigma meaningful
   best = -1. ;                           // Keep track of best mean function

   for (ipop=0 ; ipop<npops ; ipop++) {   // Evaluate for each population

      n = ntrain[ipop] ;                  // Number of samples of this pop
      tptr = tsets[ipop] ;                // Point to them
      optr = outputs+ipop ;               // Cumulate mean weight function here
      *optr = 0.0 ;

      for (icase=0 ; icase<n ; icase++) { // Do all cases in this population

         dist = 0.0 ;                     // Will sum distance measure here

         if (model == 2) {                // Use sum of absolute differences
            for (i=0 ; i<nvars ; i++) {   // Compute the distance measure
               diff = unknown[i] - *tptr++ ;
               dist += fabs ( diff ) ;
               }
            dist /= sigma ;
            }

         else {                           // Use squared Euclidean distance
            for (i=0 ; i<nvars ; i++) {   // Compute the distance measure
               diff = unknown[i] - *tptr++ ;
               dist += diff * diff ;
               }
            dist /= sigma * sigma ;
            }

         if (model < 3)                   // Use exponential weighting
            *optr += exp ( - dist ) ;
         else 
            *optr += 1. / (1. + dist ) ;
         }

      *optr /= (double) n ;  // Mean function value for this population

      if (*optr > best) {    // Keep track of best for classification
         best = *optr ;
         ibest = ipop ;
         }
      }

   return ibest ;
}  

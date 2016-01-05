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
--------------------------------------------------------------------------------

   We include a useless destructor to appease errant compilers.
   See the comment in CONST.H regarding BAD_COMPILER.

--------------------------------------------------------------------------------
*/

#if BAD_COMPILER
Network::~Network()
{
   return ;
}
#endif

/*
--------------------------------------------------------------------------------

   trial_error - Compute the mean square error for the entire training set

--------------------------------------------------------------------------------
*/

float Network::trial_error ( TrainingSet *tptr )
{
   int i, size, tset, tclass ;
   float tot_err, temp,  *dptr, diff ,*oldout;
   if (outmod == OUTMOD_CLASSIFY)   // Compute size of each training sample
      size = nin + 1 ;// Size = Number of input + 1 for Class Number
   else if (outmod == OUTMOD_AUTO)
      size = nin ;    // Size = Number of input
   else if (outmod == OUTMOD_GENERAL)
      size = nin + nout ;//Size = Number of inputs + Number of Outputs

   tot_err = 0.0 ;  // Total error will be cumulated here

   for (tset=0 ; tset<tptr->ntrain ; tset++) {  // Do all samples


      dptr = tptr->data + size * tset ;    // Point to this sample vector

      trial ( dptr ) ;			   // Evaluate network for it

      if (outmod == OUTMOD_AUTO)           // If this is AUTOASSOCIATIVE
      {

	   for (i=0 ; i<nout ; i++) {         // then the EXPECTED outputs
	      diff = *dptr++ - out[i] ;       // are just the inputs
	      tot_err += diff * diff ;
	      }

      }

      else if (outmod == OUTMOD_CLASSIFY) {  // If this is Classification
         tclass = (int) dptr[nin] - 1 ;     // class is stored after inputs
         for (i=0 ; i<nout ; i++) {         // Recall that train added a
            if (tclass == i)                // fraction so that the above
               diff = NEURON_ON - out[i] ;  // truncation to get tclass is
            else                            // always safe in any radix
               diff = NEURON_OFF - out[i] ;
	    tot_err += diff * diff ;
            }
         }

      else if (outmod == OUTMOD_GENERAL) {  // If this is GENERAL output
	 dptr += nin ;             // desired outputs stored after inputs
	 for (i=0 ; i<nout ; i++) {
	    diff = *dptr++ - out[i] ;  // diff between desired outputs
	    tot_err += diff * diff ;   // and calculated outpouts out[i]
            }
         }

      } // for all tsets
   tot_err /= (float) tptr->ntrain * (float) nout ;
   neterr = tot_err ;
   return tot_err ;
}

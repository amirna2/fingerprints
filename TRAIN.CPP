#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include <alloc.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes

/*
--------------------------------------------------------------------------------

   Constructor

--------------------------------------------------------------------------------
*/

TrainingSet::TrainingSet (
   int out_model ,
   int n_inputs ,
   int n_outputs
   )
{
   outmod = out_model ;
   nin = n_inputs ;
   nout = n_outputs ;
   ntrain = bufcnt = 0 ;
   data = NULL ;
}


/*
--------------------------------------------------------------------------------

   Destructor

--------------------------------------------------------------------------------
*/

TrainingSet::~TrainingSet ()
{
   if (data != NULL) {
      MEMTEXT ( "TRAIN: data" ) ;
      FREE ( data ) ;
      }
}

/*
--------------------------------------------------------------------------------

   train - Add members to a training set

--------------------------------------------------------------------------------
*/

void TrainingSet::train (
   char *filename ,   // Full name of file supplying training data
   int outclass       // Output class number if CLASSIF output mode
   )
{
   int i, did_any, maxlin ;
   unsigned size, nbuf, tset_bytes, maxtrain;
   unsigned long newblk;

   float  *tptr,  *temp ;
   char msg[81], *line, *lptr ;
   FILE *fp ;
   unsigned long mc;

/*
   Compute the size of each training sample.
   CLASSIF output uses a float at the end of each input sample to indicate
   the class membership (0=reject).
   We will allocate memory in chunks to save calls to realloc.
   Each chunk will hold nbuf cases.
   The number of training sets is limited by the maximum unsigned int bytes
   which we can allocate.
*/

   if (outmod == OUTMOD_CLASSIFY)
      size = nin + 1 ;
   else if (outmod == OUTMOD_AUTO)
      size = nin ;
   else if (outmod == OUTMOD_GENERAL)
      size = nin + nout ;

   tset_bytes = size * sizeof(float) ;
   nbuf = 32400 / tset_bytes ;
   if (! nbuf) {
      error_message ( "Cases too large.  Increase TRAIN_BUF_SIZE." ) ;
      return ;
      }

  // maxtrain = ( (unsigned) ~0) / (nbuf * tset_bytes) * nbuf ;

   maxtrain = 500;
   if (ntrain >= maxtrain) {
      error_message ( "Too many training sets." ) ;
      return ;
      }

   if ((fp = fopen ( filename , "rt" )) == NULL) {
      sprintf ( msg , "Cannot open %s", filename ) ;
      error_message ( msg ) ;
      return ;
      }

/*
   Conservatively estimate the length of each file line, then allocate it.
*/

   if (outmod == OUTMOD_GENERAL)
      maxlin = (nin + nout) * 20 + 100 ;
   else
      maxlin = nin * 20 + 100 ;
   if (maxlin < 1024)
      maxlin = 1024 ;

   MEMTEXT ( "TRAIN:line" ) ;
   if ((line = (char *) MALLOC ( maxlin )) == NULL) {
      memory_message ( "to read training set" ) ;
      fclose ( fp ) ;
      return ;
      }

/*
   If the training set is empty so far, do the initial allocation.
   We allocate for nbuf samples at a time to save calls to REALLOC.
*/

   if (data == NULL) {
      MEMTEXT ( "TRAIN: first block" ) ;

      data = (float  *) MALLOC ( nbuf * tset_bytes ) ;
      if (data == NULL) {
	 memory_message ( "to read training set" ) ;
	 goto FINISH ;
      }
      printf("Allocate First Block of %u bytes\n",nbuf * tset_bytes);

      bufcnt = nbuf ;
   }

/*
   Read the file.
*/

   did_any = 0 ;  /* If file runs out on first try, ERROR! */

   for (;;) {  // Endless loop reads until file exhausted

      if ((fgets ( line , maxlin , fp ) == NULL) || (strlen ( line ) < 2)) {
	 if ((! did_any)  ||  ferror ( fp )) {  // Recall fgets returns newline
	    strcpy ( msg , "Problem reading file " ) ;
	    strcat ( msg , filename ) ;
	    error_message ( msg ) ;
	    goto FINISH ;
	    }
	 else
	    break ;   // Normal end of file
	 }

      if (! bufcnt--) {  // Allocate a new memory block if needed

	 if ((ntrain + nbuf) > maxtrain) {
	    error_message ( "Too many training sets." ) ;
	    printf("\nntrain=%u, nbuf=%u, maxtrain=%u\n",ntrain,nbuf,maxtrain);
	    goto FINISH ;
	 }

	 MEMTEXT ( "TRAIN: new block" ) ;
	 newblk = (ntrain + nbuf) * tset_bytes;
	 temp = (float  *) REALLOC ( data , newblk ) ;
	 if (temp == NULL) {
	    memory_message ( "to read training set" ) ;
	    goto FINISH ;
	 }
	 printf("Allocate new block of %lu bytes\n",newblk);
	 data = temp ;
	 bufcnt = nbuf - 1 ;
      }

      lptr = line ;                          // Parse the data from this line
      tptr = data + ntrain * size ;          // This sample will go here
      for (i=0 ; i<nin ; i++)                // Start with inputs
	 *tptr++ = ParseDouble ( &lptr ) ;
      if (outmod == OUTMOD_GENERAL) {        // Also do outputs if GENERAL
	 for (i=0 ; i<nout ; i++)
	    *tptr++ = ParseDouble ( &lptr ) ;
	 }

      if (outmod == OUTMOD_CLASSIFY)         // Save class if CLASSIF output
	 *tptr++ = (float) outclass + .1 ; // .1 lets us safely truncate

      did_any = 1 ;   // Flag that we got some data, so EOF legitimate
      ++ntrain ;
      printf("N train = %d\n",ntrain);

      while ((! feof ( fp ))  &&  (line[strlen(line)-1] != '\n'))
	 fgets ( line , maxlin , fp ) ; // Line length may exceed what is needed

      if (feof ( fp ))
	 break ;

      }  /* Endless loop until a file runs out */

/*
   Save a little space by shrinking to exactly what we needed
*/

   MEMTEXT ( "TRAIN:final" ) ;
   data = (float  *) REALLOC ( data , ntrain * tset_bytes ) ;
   bufcnt = 0 ;

FINISH:
   fclose ( fp ) ;
   MEMTEXT ( "TRAIN:line" ) ;
   FREE ( line ) ;
   //mc=coreleft();
   //printf("\nMemory left :%lu\n",mc);
}

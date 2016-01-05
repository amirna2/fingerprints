#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes



void Network::test_from_file (
   char *dataname ,  // Input file name
   char *outname,     // Output file name
   int   netmod
   )
{
   int i, maxlin, did_any, best,name=0,win1,win2,win3 ;
   float *inputs, *iptr, maxi1,maxi2,maxi3 ;
   char msg[81], *line, *lptr, *tab[20];
   FILE *fp_in, *fp_out, *fname;


/*
   Open the file which contains the data to be classified
*/
   if ((fp_in = fopen ( dataname , "rt" )) == NULL) {
      strcpy ( msg , "Cannot open input data file " ) ;
      strcat ( msg , dataname ) ;
      error_message ( msg ) ;
      return ;
      }

/*
   Open the file to which outputs will be written.
   If it already exists, write a newline at its end.
*/

   fp_out = fopen ( outname , "rt" ) ;
   if (fp_out != NULL) {
      did_any = 1 ;
      fclose ( fp_out ) ;
      }
   else
      did_any = 0 ;

   if ((fp_out = fopen ( outname , "at" )) == NULL) {
      strcpy ( msg , "Cannot open output file " ) ;
      strcat ( msg , outname ) ;
      error_message ( msg ) ;
      fclose ( fp_in ) ;
      return ;
      }

   if (did_any)
   {
      fprintf ( fp_out , "\n" ) ;
   }

/*
   Allocate for the file lines as read. Conservatively guess length.
   Also allocate for network input vector.
*/

   maxlin = nin * 20 + 100 ;
   if (maxlin < 1024)
      maxlin = 1024 ;

   MEMTEXT ( "EXECUTE:line, inputs" ) ;
   line = (char *) MALLOC ( maxlin ) ;

   inputs = (float *) MALLOC ( nin * sizeof(float) ) ;

   if ((line == NULL)  ||  (inputs == NULL)) {
      memory_message ( "to execute" ) ;
      fclose ( fp_in ) ;
      fclose ( fp_out ) ;
      if (line != NULL)
	 FREE ( line ) ;
      if (inputs != NULL)
	 FREE ( inputs ) ;
      return ;
      }

/*
   Read and process the file.
*/

   did_any = 0 ;  /* If file runs out on first try, ERROR! */
   int e=0;
   for (;;) {  // Endless loop reads until file exhausted

      if ((fgets ( line , maxlin , fp_in ) == NULL) || (strlen ( line ) < 2)) {
	 if ((! did_any)  ||  ferror ( fp_in )) {
	    strcpy ( msg , "Problem reading file " ) ;
	    strcat ( msg , dataname ) ;
	    error_message ( msg ) ;
	    }
	 break ;
	 }

      lptr = line ;           // Parse the data from this line
      iptr = inputs ;         // This will be the network inputs
      for (i=0 ; i<nin ; i++)
	 *iptr++ = ParseDouble ( &lptr ) ;

      if (did_any)            // Start each new case on new line
	 fprintf ( fp_out , "\n" ) ;

      did_any = 1 ;           // Flag that at least one found
      trial ( inputs ) ;      // Compute network's outputs
      maxi1=maxi2=maxi3=0.0;
      win1=win2=win3=0;
      // Saving maximun activation and the winner of the output vector
      if (netmod==NETMOD_KOH)
      {
	// First maximum
	for (i=0 ; i<nout ; i++)
	{
	   if (out[i]>maxi1)
	   {
	     maxi1=out[i];
	     win1=i;
	   }
	}
	// 2nd Maximum
	for (i=0 ; i<nout ; i++)
	{
	   if ( (out[i]>maxi2) && (out[i]<maxi1) )
	   {
	     maxi2=out[i];
	     win2=i;
	   }
	}
	// 3rd Maximum
	for (i=0 ; i<nout ; i++)
	{
	   if ( (out[i]>maxi3) && (out[i]<maxi2) )
	   {
	     maxi3=out[i];
	     win3=i;
	   }
	}

      }

      if (netmod==NETMOD_KOH)
      {
	fprintf (fp_out,"%d %3.2lf\n",win1,maxi1*100.0);
	fprintf (fp_out,"%d %3.2lf\n",win2,maxi2*100.0);
	fprintf (fp_out,"%d %3.2lf",win3,maxi3*100.0);
      }
      else
	for (i=0 ; i<nout ; i++)
	{
	   fprintf ( fp_out , "%.4lf ",out[i]);
	}

      e++;
      while ((! feof ( fp_in ))  &&  (line[strlen(line)-1] != '\n'))
	 fgets ( line , maxlin , fp_in ) ; // Line length may exceed maxlin

      if (feof ( fp_in ))
	 break ;
      }  /* Endless loop until a file runs out */

   MEMTEXT ( "EXECUTE:line, inputs" ) ;
   fclose ( fp_in ) ;
   fclose ( fp_out ) ;
   FREE ( line ) ;
   FREE ( inputs ) ;
}

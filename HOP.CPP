// HOPNET.CPP

// Hopfield network methods and algorithm

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes


static void free_non_null ( void **p ) ;
static void free_non_null ( void **p )
{
   if (*p != NULL) {
      FREE ( *p ) ;
      *p = NULL ;
      }
}

HopNet::HopNet (int n_inputs , int n_outputs,int executable , int zero )
{

   outmod = OUTMOD_AUTO;
   nout= n_outputs;
   nin = n_inputs ;
   exe = executable ;
   neterr = 1.0 ;
   weights = NULL;
   confusion = NULL ;
   out = NULL;
   int Wsize=nin*nout;

   if (((weights = (double *) MALLOC ( Wsize * sizeof(double) )) == NULL)
    || (exe && (out = (double *) MALLOC ( nout * sizeof(double) )) == NULL)){
      free_non_null ( (void **) &weights ) ;
      free_non_null ( (void **) &confusion ) ;
      return ;
      }
   if (zero==1)
   {
      int n=Wsize;
      while(n--)
	  weights[n] = 0.0 ;

   }
   ok=1;
}


int HopNet::copy_weights(HopNet *dest, HopNet *source)
{
  int n=nin*nin;
  memcpy ( dest->weights , source->weights , n * sizeof(double) ) ;
  return 1;
}

void HopNet::zero_weights ()
{
   int n;
   n = nin * nout;
   neterr = 1.0 ;
   while (n>=0)
   {
     weights[n] = 0.0 ;
     n--;
   }
   printf("Zero weights\n");
}

void HopNet::trial ( double *inputs )
{
  int i;

  if (! exe)
  {   // Should NEVER happen, but good style to aid debugging
    error_message ( "Internal error in HopNet::trial" ) ;
    return ;
  }

      for (i=0;i<nout;i++)
      {
	//  out[i] = 1 if SOM( input[i]*weights[i][j] ) >0.0
	// out[i] = 0 if SOM ( input[i]*weights[i][j] )<=0.0
	out[i]=hop_activity(inputs,weights+i*nin,nin);
      }
}

void HopNet::learn ( TrainingSet *tptr , struct LearnParams *lptr )
{
   int     tsest,i,j,size,n;
   double  delta;
   double *inputs;
   double  err;
   char    msg[80];
   double first_err;
   double E;

  size = nin;
 if (lptr->init == 5)				  // HOPINIT
 {
   printf("Hofield Initialisation\n");
   HopInit(tptr);
   neterr=trial_error(tptr);
   printf("Initial Error = %2.2lf\n",100*neterr);
 }

 if (lptr->init == 0)                             // NO INIT
 {
      printf("No Init\n");
      neterr = trial_error ( tptr ) ;
      printf("Error = %2.2lf\n",100*neterr);
 }

 // Main learning loop is here
 for (int Try; ; Try++)
 {
   if (neterr < lptr->quit_err) break;
   for (tset=0;tset <tptr->ntrain;tset++)	// Do All Samples
   {
     printf("Try Nø %d : error = %2.3lf : Energy = %3.2lf\n",tset, 100*neterr,E);

     for (int i=0;i<nin;i++)
     {
	 for (int j=0;j<nin;j++)
	 {
	     E=E + (weights[i*nin+j]*inputs[i]*inputs[j]);
	 }
     }
     E= E * - 0.5;
     neterr = trial_error(tptr);
}

int HopNet::wt_save ( FILE *fp )
{
   int n ;
   n = nout * nin ;
   fwrite ( weights , n * sizeof(double) , 1 , fp ) ;
   if (ferror ( fp ))
      return 1 ;
   return 0 ;
}

void HopNet::wt_restore ( FILE *fp )
{
  int n ;
  n = nout * nin;
  fread ( weights , n * sizeof(double) , 1 , fp ) ;
  if (ferror ( fp ))
    ok = 0 ;
}


void HopNet::HopInit(TrainingSet *tptr)
{
  int n;
  int size = nin;
  int tset;
  double *inputs;

  zero_weights () ;
  for (tset=0 ; tset < tptr->ntrain ; tset++) // Do all samples
  {
       inputs = tptr->data + size * tset ;// Point to i'th vector of entries
       printf("Memorising Sample %d\n",tset);
       n=0;
       for (int i=0;i<nin;i++)
       {
	 for (int j=0;j<nin;j++)
	 {
	   if (i!=j)
	     weights[n]=weights[n]+ inputs[i]*inputs[j];
	   n++;
	 }
       }
  }
  printf("Initial Weights\n");
  for (int h=0;h<nin*nin;h++)
  {
    if ((h%(nin*nin))==0) printf("\n");
    printf("%2.2lf ",weights[h]);
  }
  printf("\n");
}
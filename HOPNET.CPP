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

   if (((weights = (float *) MALLOC ( Wsize * sizeof(float) )) == NULL)
    || (exe && (out = (float *) MALLOC ( nout * sizeof(float) )) == NULL)){
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
   for (int i=0;i<nout;i++)
   {
     out[i]=0.0;
   }
   ok=1;
}


int HopNet::copy_weights(HopNet *dest, HopNet *source)
{
  int n=nin*nin;
  memcpy ( dest->weights , source->weights , n * sizeof(float) ) ;
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
}

void HopNet::trial ( float *inputs )
{
  int i;
  float *wptr;
  for (i=0;i<nout;i++)
  {
    wptr = weights+i*nin;
    out[i]=hop_activity(i, inputs, wptr, nin);
  }
}

void HopNet::learn ( TrainingSet *tptr , struct LearnParams *lptr )
{
   int     tset,i,j,size,n;
   float *inputs;
   char    msg[80];
   float  diff,tot_err;

   size = nin;

 if (lptr->init == 5)				  // HOPINIT
 {
   HopInit(tptr);
//   neterr = trial_error(tptr);
 }

 if (lptr->init == 0)                             // NO INIT
 {
      neterr = trial_error ( tptr ) ;
      printf("Error = %2.2lf\n",100*neterr);
      return;
 }

 for (int Try=0;;Try++)
 {
   if (neterr<lptr->quit_err) break;
   printf("Try Nø %d 		Error = %2.2lf%%\n",Try,100*neterr);
   for (tset=0 ; tset < tptr->ntrain ; tset++) // Do all samples
   {
     inputs = tptr->data + size * tset ;// Point to i'th vector of entries
     trial(inputs);			// Compute OUT for this samples

     // Compute error with  between out(t-1) and out(t);
     for (i=0 ; i<nout ; i++)
     {
       diff = *inputs++ - out[i];
       tot_err += diff * diff ;
     }

     for (int k=0;k<nout;k++)           // the input becomes the output
     {
       tptr->data[tset*size+k]=out[k];
     }

   }  // Tset loop
   tot_err /= (float) tptr->ntrain * (float) nout ;
   neterr = tot_err ;
 }    // Tries loop
}

int HopNet::wt_save ( FILE *fp )
{
   int n ;
   n = nout * nin ;
   fwrite ( weights , n * sizeof(float) , 1 , fp ) ;
   if (ferror ( fp ))
      return 1 ;
   return 0 ;
}

void HopNet::wt_restore ( FILE *fp )
{
  int n ;
  n = nout * nin;
  fread ( weights , n * sizeof(float) , 1 , fp ) ;
  if (ferror ( fp ))
    ok = 0 ;
}


void HopNet::HopInit(TrainingSet *tptr)
{
  int n,i,j;
  int size = nin;
  int tset;
  float *inputs;
  float E;
  float *DeltaW;

  // Alocate memory for Delta Weights
  if ((DeltaW = (float *) MALLOC ( (nin*nin) * sizeof(float) )) == NULL)
  {
    free_non_null ( (void **) &DeltaW ) ;
    return;
  }
  zero_weights ();
  for (n=0;n<nin*nin;n++)
    DeltaW[n]=0.0;

  for (tset=0 ; tset < tptr->ntrain ; tset++) // Do all samples
  {
       inputs = tptr->data + size * tset ;// Point to i'th vector of entries
       printf("Memorising Sample %d\n",tset);
       for (i=0;i<nin;i++)
       {
	 for ( j=0;j<nin;j++)
	 {
	   if (i!=j)
	   {
	     // This is the actual Hebbian learning rule !
	     // But with a complete learning (rate = 1.0)
	     weights[i*nin+j]=weights[i*nin+j] + (inputs[i]*inputs[j]);
	   }
	 }
       }
       for (i=0;i<nin;i++)
       {
	 for ( j=0;j<nin;j++)
	 {
	   if (i!=j)
	   {
	     weights[i*nin+j]=weights[i*nin+j] - (0.02*(inputs[i]*inputs[j]));

	   }
	 }
       }
  }  // Tset loop
}
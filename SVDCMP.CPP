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
   Prototypes for local subroutines
*/

static float bidiag ( float *a , int rows , int cols , float *w ,
		       float *work ) ;

static void cancel ( int rows , int cols , int lower , int index ,
		     float matnorm , float *a , float *w , float *work ) ;

static void qr ( int rows , int cols , int lower , int index ,
		 float *a , float *v , float *w , float *work ) ;

static void transforms ( float *a , int rows , int cols , float *w ,
			 float *v , float *work ) ;

static void verify_nonneg ( int cols , int index , float *w , float *v ) ;

/*
   Local macros.  RSS computes the root of the sum of squares of
   its arguments.  This clever implementation avoids over/underflow.
   SIGN is the old FORTRAN routine which returns the value of its
   first argument with the sign of its second.
   The variables va, vb and vc are local work areas for these macros.
*/

static float va, vb, vc ;

#define RSS(a,b) ((va=fabs(a)) > (vb=fabs(b)) ? \
                 (vc=vb/va , va*sqrt(vc*vc+1.0)) : \
                 ((vb != 0.0) ? (vc=va/vb , vb*sqrt(vc*vc+1.0)) : 0.0))

#define SIGN(a,b) (va=fabs(a) , (b) >= 0.0 ? va : -va)

/*
--------------------------------------------------------------------------------

   Constructor - This allocates memory for the input/output matrix 'a' and
                 any work areas which it will need (including the public
                 outputs of w and v).  It also allocates 'b' which will be
                 input to the backsub routine.  It does not allocate 'x' which
                 is the output of backsub.

                 If there is a problem (rows < cols, or insufficient memory),
                 it leaves public ok=0.  The user should check for this after
                 allocating with new.

--------------------------------------------------------------------------------
*/

SingularValueDecomp::SingularValueDecomp ( int nrows , int ncols , int preserve)
{
   if (nrows < ncols) {
      rows = cols = ok = 0 ;
      return ;
      }

   a = u = w = v = work = b = NULL ;
   if (((a = (float *) MALLOC ( nrows * ncols * sizeof(float) )) == NULL)
    || (preserve  &&
	(u = (float *) MALLOC ( nrows * ncols * sizeof(float)))== NULL)
    || ((w = (float *) MALLOC ( ncols * sizeof(float) )) == NULL)
    || ((v = (float *) MALLOC ( ncols * ncols * sizeof(float) )) == NULL)
    || ((work = (float *) MALLOC ( ncols * sizeof(float) )) == NULL)
    || ((b = (float *) MALLOC ( nrows * sizeof(float) )) == NULL)) {
      if (a != NULL)
         FREE ( a ) ;
      if (u != NULL)
         FREE ( u ) ;
      if (w != NULL)
         FREE ( w ) ;
      if (v != NULL)
         FREE ( v ) ;
      if (work != NULL)
         FREE ( work ) ;
      if (b != NULL)
         FREE ( b ) ;
      rows = cols = ok = 0 ;
      return ;
      }

   rows = nrows ;
   cols = ncols ;
   ok = 1 ;
}

/*
--------------------------------------------------------------------------------

   Destructor - This frees all memory allocated by the constructor.

--------------------------------------------------------------------------------
*/

SingularValueDecomp::~SingularValueDecomp ()
{
   if (! ok)    // If constructor's mallocs failed
      return ;  // there is nothing to free

   FREE ( a ) ;
   if (u != NULL)    // This was allocated only if preserve was nonzero
      FREE ( u ) ;
   FREE ( w ) ;
   FREE ( v ) ;
   FREE ( work ) ;
   FREE ( b ) ;
}


/*
--------------------------------------------------------------------------------

   svdcmp - Perform singular value decomposition on the matrix already stored.

--------------------------------------------------------------------------------
*/

void SingularValueDecomp::svdcmp ()
{
   int cflag, iter, index, lower ;
   float matnorm, *mat ;

   if (u == NULL)    // Do we replace a with u
      mat = a ;
   else {            // or preserve it?
      memcpy ( u , a , rows * cols * sizeof(float) ) ;
      mat = u ;
      }

   matnorm = bidiag ( mat , rows , cols , w , work ) ; // Reduce to bidiagonal
   transforms ( mat , rows , cols , w , v , work ) ;   // Accumulate R&L trans

   for (index=cols-1 ; index>=0 ; index--) { // All singular values
      for (iter=0 ; iter<100 ; iter++) {     // Conservative limit on QR tries
         cflag = 1 ;
         for (lower=index ; lower ; lower--) { // Split?
            if (fabs (work[lower]) + matnorm == matnorm) {
               cflag = 0 ;
               break ;
               }
            if (fabs (w[lower-1]) + matnorm == matnorm)
               break ;
            }
         if (lower && cflag)
            cancel ( rows , cols , lower , index , matnorm , mat , w , work ) ;
         if (lower == index) {  // Converged?
            verify_nonneg ( cols , index , w , v ) ; // Want nonegative singvals
            break ;
            }
         qr ( rows , cols , lower , index , mat , v , w , work ) ; // Another QR
         }
      }
}

/*
--------------------------------------------------------------------------------

   bidiag - Local routine for Householder reduction to bidiagonal form

--------------------------------------------------------------------------------
*/

static float bidiag (
   float *a ,
   int rows ,
   int cols ,
   float *w ,
   float *work
   )

{
   int col, j, k, nextcol ;
   float pp, qq, denom, sum ;
   float matnorm, scale ;

   matnorm = qq = sum = scale = 0.0 ;

   for (col=0 ; col<cols ; col++) {

      nextcol = col + 1 ;
      work[col] = scale * qq ;
      qq = sum = scale = 0.0 ;

      for (k=col ; k<rows ; k++)
         scale += fabs ( a[k*cols+col] ) ;
      if (scale > 0.0) {
         for (k=col ; k<rows ; k++) {
            a[k*cols+col] /= scale ;
            sum += a[k*cols+col] * a[k*cols+col] ;
            }
         pp = a[col*cols+col] ;
         qq = -SIGN ( sqrt(sum) , pp ) ;
         denom = pp * qq - sum ;
         a[col*cols+col] = pp - qq ;

         for (j=nextcol ; j<cols ; j++) {
            sum = 0.0 ;
            for (k=col ; k<rows ; k++)
               sum += a[k*cols+col] * a[k*cols+j] ;
            pp = sum / denom ;
            for (k=col ; k<rows ; k++)
               a[k*cols+j] += pp * a[k*cols+col] ;
            }

         for (k=col ; k<rows ; k++)
            a[k*cols+col] *= scale ;
         } // if scale > 0

      w[col] = scale * qq ;


      qq = sum = scale = 0.0 ;

      for (k=nextcol ; k<cols ; k++)
         scale += fabs ( a[col*cols+k] ) ;

      if (scale > 0.0) {
         for (k=nextcol ; k<cols ; k++) {
            a[col*cols+k] /= scale ;
            sum += a[col*cols+k] * a[col*cols+k] ;
            }
         pp = a[col*cols+nextcol] ;
         qq = -SIGN ( sqrt ( sum ) , pp ) ;
         denom = pp * qq - sum ;
         a[col*cols+nextcol] = pp - qq ;
         for (k=nextcol ; k<cols ; k++)
            work[k] = a[col*cols+k] / denom ;
         if (col != rows-1) {
            for (j=nextcol ; j<rows ; j++) {
               sum = 0.0 ;
               for (k=nextcol ; k<cols ; k++)
                  sum += a[j*cols+k] * a[col*cols+k] ;
               for (k=nextcol ; k<cols ; k++)
                  a[j*cols+k] += sum * work[k] ;
               }
            }
         for (k=nextcol ; k<cols ; k++)
            a[col*cols+k] *= scale ;
         }

      sum = fabs (w[col]) + fabs (work[col]) ;
      if (sum > matnorm)
         matnorm = sum ;
      }
   return matnorm ;
}


/*
--------------------------------------------------------------------------------

   cancel

--------------------------------------------------------------------------------
*/

static void cancel (
   int rows ,
   int cols ,
   int lower ,
   int index ,
   float matnorm ,
   float *a ,
   float *w ,
   float *work
   )
{
   int col, row ;
   float c, rr, ww, hypot, s, pp, qq ;

   s = 1.0 ;
   for (col=lower ; col<=index ; col++) {
      rr = s * work[col] ;
      if (fabs (rr) + matnorm != matnorm) {
         ww = w[col] ;
         hypot = RSS ( rr , ww ) ;
         w[col] = hypot ;
         c =  ww / hypot ;
         s = -rr / hypot ;
         for (row=0 ; row<rows ; row++) {
            pp = a[row*cols+lower-1] ;
            qq = a[row*cols+col] ;
            a[row*cols+lower-1] = qq * s  +  pp * c ;
            a[row*cols+col] = qq * c  -  pp * s ;
            }
         }
      }
}

/*
--------------------------------------------------------------------------------

   Cumulate right and left transforms

--------------------------------------------------------------------------------
*/

static void transforms (
   float *a ,
   int rows ,
   int cols ,
   float *w ,
   float *v ,
   float *work
   )
{
   int col, j, k ;
   float temp, ww, sum ;

/*
   Right
*/

   ww = 0.0 ;  // Insures failure of upcoming if first time
   for (col=cols-1 ; col>=0 ; col--) {
      if (ww != 0.0) {
         for (j=col+1 ; j<cols ; j++)  // Double division avoids underflow
            v[j*cols+col] = (a[col*cols+j] / a[col*cols+col+1]) / ww ;
         for (j=col+1 ; j<cols ; j++) {
            sum = 0.0 ;
            for (k=col+1 ; k<cols ; k++)
               sum += a[col*cols+k] * v[k*cols+j] ;
            for (k=col+1 ; k<cols ; k++)
               v[k*cols+j] += v[k*cols+col] * sum ;
            }
         }
      for (j=col+1 ; j<cols ; j++)
         v[col*cols+j] = v[j*cols+col] = 0.0 ;
      v[col*cols+col] = 1.0 ;
      ww = work[col] ;
      }

/*
   Left
*/

   for (col=cols-1 ; col>=0 ; col--) {

      for (j=col+1 ; j<cols ; j++)
         a[col*cols+j] = 0.0 ;

      if (w[col] == 0.0) {
         for (j=col ; j<rows ; j++)
            a[j*cols+col] = 0.0 ;
         }

      else {
         ww = 1.0 / w[col] ;
         for (j=col+1 ; j<cols ; j++) {
            sum = 0.0 ;
            for (k=col+1 ; k<rows ; k++)
               sum += a[k*cols+col] * a[k*cols+j] ;
            temp = sum / a[col*cols+col] * ww ;
            for (k=col ; k<rows ; k++)
               a[k*cols+j] += a[k*cols+col] * temp ;
            }
         for (j=col ; j<rows ; j++)
            a[j*cols+col] *= ww ;
         }

      a[col*cols+col] += 1.0 ;
      }
}

/*
--------------------------------------------------------------------------------

   qr

--------------------------------------------------------------------------------
*/

static void qr (
   int rows ,
   int cols ,
   int lower ,
   int index ,
   float *a ,
   float *v ,
   float *w ,
   float *work )
{
   int col, colp1, row ;
   float c, cn, s, sn, thisw, rot1, rot2, hypot, temp, ww ;

   ww = w[index] ;
   sn = work[index] ;
   rot1 = work[index-1] ;
   rot2 = w[index-1] ;
   temp = ((rot2-ww) * (rot2+ww) + (rot1-sn) * (rot1+sn)) / (2.0 * sn * rot2) ;
   hypot = RSS ( temp , 1.0 ) ;
   thisw = w[lower] ;
   cn = ((thisw-ww) * (thisw+ww) + sn *
         ((rot2 / (temp + SIGN(hypot,temp))) - sn )) / thisw ;

   c = s = 1.0 ;

   for (col=lower ; col<index ; col++) {
      colp1 = col+1 ;
      rot1 = work[colp1] ;
      sn = s * rot1 ;
      rot1 = c * rot1 ;
      hypot = RSS ( cn , sn ) ;
      work[col] = hypot ;
      c = cn / hypot ;
      s = sn / hypot ;
      cn = thisw * c  +  rot1 * s ;
      rot1 = rot1 * c  -  thisw * s ;
      rot2 = w[colp1] ;
      sn = rot2 * s ;
      rot2 *= c ;
      for (row=0 ; row<cols ; row++) {
         thisw = v[row*cols+col] ;
         temp = v[row*cols+colp1] ;
         v[row*cols+col] = thisw * c  +  temp * s ;
         v[row*cols+colp1] = temp * c  -  thisw * s ;
         }
      hypot = RSS ( cn , sn ) ;
      w[col] = hypot ;
      if (hypot != 0.0) {
         c = cn / hypot ;
         s = sn / hypot ;
         }
      cn = c * rot1  +  s * rot2 ;
      thisw = c * rot2  -  s * rot1 ;
      for (row=0 ; row<rows ; row++) {
         rot1 = a[row*cols+col] ;
         rot2 = a[row*cols+colp1] ;
         a[row*cols+col] = rot1 * c  +  rot2 * s ;
         a[row*cols+colp1] = rot2 * c  -  rot1 * s ;
         }
      }
   w[index] = thisw ;
   work[lower] = 0.0 ;
   work[index] = cn ;
}

/*
--------------------------------------------------------------------------------

   verify_nonneg - Flip sign of this singular value and its vector if negative

--------------------------------------------------------------------------------
*/

static void verify_nonneg (
   int cols ,
   int index ,
   float *w ,
   float *v
   )
{
   int i ;

   if (w[index] < 0.0) {
      w[index] = -w[index] ;
      for (i=0 ; i<cols ; i++)
         v[i*cols+index] = -v[i*cols+index] ;
      }
}


/*
--------------------------------------------------------------------------------

   Backsubstitution algorithm for solving Ax=b where A generated u, w, v
   Inputs are not destroyed, so it may be called with several b's.
   The user must have filled in the public RHS 'b' before calling this.

--------------------------------------------------------------------------------
*/

void SingularValueDecomp::backsub (
   float thresh , // Threshold for zeroing singular values.  Typically 1.e-8.
   float *x       // Output of solution
   )
{
   int row, col, cc ;
   float sum, *mat ;

   if (u == NULL)    // Did we replace a with u
      mat = a ;
   else              // or preserve it?
      mat = u ;

/*
   Set the threshold according to the maximum singular value
*/

   sum = 0.0 ;                       // Will hold max w
   for (col=0 ; col<cols ; col++) {
      if (w[col] > sum)
         sum = w[col] ;
      }
   thresh *= sum ;
   if (thresh <= 0.0)    // Avoid dividing by zero in next step
      thresh = 1.e-30 ;

/*
   Find U'b
*/

   for (col=0 ; col<cols ; col++) {
      sum = 0.0 ;
      if (w[col] > thresh) {
         for (row=0 ; row<rows ; row++)
            sum += mat[row*cols+col] * b[row] ;
         sum /= w[col] ;
         }
      work[col] = sum ;
      }

/*
   Multiply by V
*/

   for (col=0 ; col<cols ; col++) {
      sum = 0.0 ;
      for (cc=0 ; cc<cols ; cc++)
         sum += v[col*cols+cc] * work[cc] ;
      x[col] = sum ;
      }
}


#if 0
/*
--------------------------------------------------------------------------------

   Optional main to test it

--------------------------------------------------------------------------------
*/

#define RANDMAX 32767

void main ()
{
   int rep, m, n, i, j, k ;
   float *x, *sa, sum, err, wmin, wmax ;
   char msg[81] ;
   SingularValueDecomp *s ;

   printf ( "\nEnter m, n:" ) ;
   while (gets (msg) == 0 ) ;
   sscanf ( msg , "%d %d" , &m, &n ) ;
   if (m <= 0  ||  n <= 0)
      exit ( 0 ) ;

   sa = (float *) malloc ( m * n * sizeof(float) ) ;
   x = (float *) malloc ( n * sizeof(float) ) ;
   s = new SingularValueDecomp ( m , n , 0 ) ;

   for (rep=0;;rep++) {

      if (kbhit()) {
         if (getch() == 27)
            exit ( 0 ) ;
         }

      if ((m == n)  &&  ! rep) {  // Ill cond
         for (i=0 ; i<m ; i++) {
            for (j=0 ; j<n ; j++)
               sa[i*n+j] = s->a[i*n+j] = 1.0 / (i + j + 1.0) ;
	    s->b[i] = (float) (rand() - RANDMAX/2) / (float) RANDMAX ;
            }
         }
      else {
         for (i=0 ; i<m ; i++) {
            for (j=0 ; j<n ; j++)
               sa[i*n+j] = s->a[i*n+j] =
			     (float) (rand() - RANDMAX/2) / (float) RANDMAX ;
	    s->b[i] = (float) (rand() - RANDMAX/2) / (float) RANDMAX ;
            }
         }

      s->svdcmp () ;

      wmin = 1.e30 ;
      wmax = -1.e30 ;
      for (i=0 ; i<n ; i++) {
         if (s->w[i] < wmin)
            wmin = s->w[i] ;
         if (s->w[i] > wmax)
            wmax = s->w[i] ;
         }

      printf ( "\n(%lf %lf)", wmin, wmax ) ;

      err = 0.0 ;
      for (i=0 ; i<m ; i++) {
         for (j=0 ; j<n ; j++) {
            sum = 0.0 ;
            for (k=0 ; k<n ; k++)
               sum += s->a[i*n+k] * s->w[k] * s->v[j*n+k] ;
            err += fabs ( sum - sa[i*n+j] ) ;
            }
         }

      printf ( "   Rep=%lf", err ) ;
      if (fabs(err) > 1.e-10) {
         printf ( "\a" ) ;
         getch() ;
         }

      err = 0.0 ;
      for (i=0 ; i<n ; i++) {
         for (j=0 ; j<n ; j++) {
            sum = 0.0 ;
            for (k=0 ; k<m ; k++)
               sum += s->a[k*n+i] * s->a[k*n+j] ;
            if (i == j)
               err += fabs ( sum - 1.0 ) ;
            else 
               err += fabs ( sum ) ;
            }
         for (j=0 ; j<n ; j++) {
            sum = 0.0 ;
            for (k=0 ; k<n ; k++)
               sum += s->v[k*n+i] * s->v[k*n+j] ;
            if (i == j)
               err += fabs ( sum - 1.0 ) ;
            else 
               err += fabs ( sum ) ;
            }
         }
      printf ( "   Orthog=%lf", err ) ;
      if (fabs(err) > 1.e-10) {
         printf ( "\a" ) ;
         getch() ;
         }

      s->backsub ( 1.e-8 , x ) ;
      err = 0.0 ;

      for (i=0 ; i<m ; i++) {
         sum = 0.0 ;
         for (j=0 ; j<n ; j++)
            sum += x[j] * sa[i*n+j] ;
         err += fabs ( sum - s->b[i] ) ;
         }

      printf ( "   Back=%lf", err ) ;
      if ((m == n)  &&  (fabs(err) > 1.e-10)) {
         printf ( "\a" ) ;
         getch() ;
         }

      }
}
#endif

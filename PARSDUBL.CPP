#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes

float ParseDouble ( char **str )
{
   float num ;

   while (! (isdigit ( **str )  ||  (**str == '-')  ||  (**str == '.'))) {
      if (**str)
         ++(*str) ;
      else
         return 0. ;
      }

   num = atof ( *str ) ;

   while ((isdigit ( **str )  ||  (**str == '-')  ||  (**str == '.')))
      ++(*str) ;

   return num ;
}


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes

void error_message ( char *msg )
{
   printf ( "\nERROR: %s", msg ) ;
}

void warning_message ( char *msg )
{
   printf ( "\nWARNING: %s", msg ) ;
}

void normal_message ( char *msg )
{
   printf ( "\n%s", msg ) ;
}

void progress_message ( char *msg )
{
   printf ( "%s", msg ) ;
}

void memory_message ( char *msg )
{
   printf ( "\nInsufficient memory %s", msg ) ;
}

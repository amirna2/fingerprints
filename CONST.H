/******************************************************************************/
/*                                                                            */
/* CONST.H - System and program limitation constants                          */
/*           This also contains typedefs, structs, et cetera.                 */
/*                                                                            */
/******************************************************************************/

#define BAD_COMPILER 1

/*
   These interpose memory allocation checking.
*/
/*
#if 1
#define MALLOC memalloc
#define FREE memfree
#define REALLOC memrealloc
#define MEMTEXT memtext
#define MEMCLOSE memclose
#else
#define MALLOC malloc
#define FREE free
#define REALLOC realloc
#define MEMTEXT notext
#define MEMCLOSE nomemclose
#endif
*/
#if 1
#define MALLOC memalloc
#define FREE memfree
#define REALLOC memrealloc
#define MEMTEXT memtext
#define MEMCLOSE memclose
#else
#define MALLOC calloc
#define FREE free
#define REALLOC realloc
#define MEMTEXT notext
#define MEMCLOSE nomemclose
#endif

// define processor version

#define VERSION_16_BIT 1

#if VERSION_16_BIT
#define TRAIN_BUF_SIZE 65536     /* Alloc this much tsets mem (max) at a time */
#define MAX_INPUTS 300           /* Number of network inputs */
#define MAX_HIDDEN 800           /* Number of hidden neurons */
#define MAX_OUTPUTS 1000         /* Number of network outputs */
#else
#define TRAIN_BUF_SIZE 65536
#define MAX_INPUTS 1024
#define MAX_HIDDEN 1024
#define MAX_OUTPUTS 1024
#endif

#define RANDMAX 32767            /* rand() returns from 0 through this */
#define CONTROL_LINE_LENGTH 255  /* Max length of user's control input */
#define MAX_CONTROL_FILES 16     /* Nesting of control files */
#define LTAB_LENGTH 100          /* Activation function table length */
#define LTAB_MAX 10.0            /* Maximum in that table */

#define NEURON_ON  0.9           /* Target output activation levels */
#define NEURON_OFF 0.1           /* must not be forced to 0 and 1 */

/*
   These are network model codes.  If additional networks are defined, they
   should be appended, leaving existing ones with the same codes, in order
   to avoid disk file incompatibilites.  They must be positive (-1 = unset).
*/

#define NETMOD_LAYER 1
#define NETMOD_KOH 2
#define NETMOD_HOP 3
#define NETMOD_BAM 4

/*
   These are output model codes.  If additional outputs are defined, they
   should be appended, leaving existing ones with the same codes, in order
   to avoid disk file incompatibilites.  They must be positive (-1 = unset).
*/

#define OUTMOD_CLASSIFY 1
#define OUTMOD_AUTO 2
#define OUTMOD_GENERAL 3

/*
   The annealing parameters have a zero suffix for the value used for finding
   starting weights.  The non-zero parameters are for when annealing is used
   to (hopefully) escape from local minima during learning.
*/

struct AnnealParams {
   int temps0 ;        // Number of temperatures
   int temps ;
   int iters0 ;        // Iterations per temperature
   int iters ;
   int setback0 ;      // Set back iteration counter if improvement
   int setback ;
   double start0 ;     // Starting temperature
   double start ;
   double stop0 ;      // Stopping temperature
   double stop ;
   } ;

struct GenInitParams {
   int pool ;          // Number of individuals in population pool
   int gens ;          // Number of generations after initial population
   int climb ;         // Use hill climbing (elitism)?
   double overinit ;   // Overinitialization factor (<= 1)
   double pcross ;     // Probability of crossover
   double pmutate ;    // Probability of mutation
   } ;

struct LearnParams {
   int init ;          // 0=no init, 1=ann noreg, 2=anneal, 3=genetic, 4=regress
   double quit_err ;   // Quit if mean square error fraction of max this low
   int retries ;       // Quit after this many random retries
   struct AnnealParams *ap ;
   struct GenInitParams *gp ;
   struct KohParams *kp ;
   } ;

struct KohParams {     // Both network and learning params here
   int normalization ; // 0=multiplicative, 1=Z
   int learn_method  ; // 0=additive, 1=subtractive
   double rate ;       // Learning rate (0-1)
   double reduction ;  // Learning rate reduction per pass (0-1)
   } ;

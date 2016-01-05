/******************************************************************************/
/*                                                                            */
/*  CLASSES - Headers for all classes                                         */
/*                                                                            */
/******************************************************************************/

/*
--------------------------------------------------------------------------------

   TrainingSet - Collection of samples which will be used for training

   All training data is stored in 'data'.  If the output model is HETERO, the
   input for each sample is followed in 'data' by the number (1 through nout)
   of the output class to which that sample belongs.  If the output model is
   GENERAL, the output values follow the input values.  No output values are
   stored in AUTO mode, since the output is the input!

   The variable 'bufcnt' is the number of samples which could still fit in
   'data' without allocating more memory.  This allows us to allocate memory
   in large blocks, avoiding the overhead of often expensive operating system
   calls to malloc.

--------------------------------------------------------------------------------
*/

class TrainingSet {

public:

   TrainingSet ( int outmodel , int n_inputs , int n_outputs ) ;
   ~TrainingSet () ;
   void train ( char *file , int outclass ) ;

   unsigned ntrain ; // Number of samples in 'data'
   float *data ;    // Actual training data here
   int outmod ;      // Output model (see OUTMOD_? in CONST.H)
   int nin ;         // Number of input neurons
   int nout ;        // Number of output neurons

private:

   unsigned bufcnt ; // Sample areas remaining unused at end of 'data'
} ;

/*
--------------------------------------------------------------------------------

   SingularValueDecomp - Singular value decomposition of matrices

   Normally, the user would:
     1) Use 'new' to create a SingularValueDecomp object with all necessary
	memory (a, u?, w, v, work, b) allocated by the constructor.
     2) Fill in the public 'a' with the matrix to be decomposed.
     3) Call svdcmp to decompose a, replacing it with the U matrix if preserve
        is zero, else computing u.
	This will also compute w and v, which are normally not used but are
        public just in case the user wants to access them.
     4) Fill in the 'b' vector with the right hand side of the equations.
     5) Call backsub with a pointer to the cols vector which is where the
        solution will be placed.  This vector is NOT allocated by the
	constructor.  The outputs of svdcmp (a, u?, w, v) will not be disturbed.
     6) Repeat the above step as desired.
     7) Delete the SingularValueDecomp object, which frees all memory which
	was allocated by the constructor.

--------------------------------------------------------------------------------
*/

class SingularValueDecomp {

public:

   SingularValueDecomp ( int rows , int cols , int preserve ) ;
   ~SingularValueDecomp () ;
   void svdcmp () ;
   void backsub ( float thresh , float *x ) ;

   int ok ;         // Was memory allocation successful?

/*
   The following four input/output areas are allocated by the constructor
*/

   float *a ;      // Rows by cols input of 'A' matrix, output of U
   float *u ;      // unless preserve != 0, in which case U output here
   float *w ;      // Cols vector output of singular values, not sorted
   float *v ;      // Cols by cols output of 'V' matrix
   float *b ;      // Rows vector of RHS input to backsub


private:

   int rows ;       // Number of rows in 'A' matrix
   int cols ;       // and number of columns
   float *work ;   // Cols work vector (allocated by constructor)
} ;

/*
--------------------------------------------------------------------------------

   Network - General parent of things common to all networks

   The constructors for the child classes set 'ok' to one if all memory
   allocation succeeded.  Otherwise they set it to zero,

--------------------------------------------------------------------------------
*/

class Network {

public:

   void classify_from_file ( char *name , float thresh ) ;
   void execute_from_file ( char *inname , char *outname) ;
   void test_from_file ( char *inname , char *outname, int netmod);
   void reset_confusion () ;
   void save_confusion ( char *name ) ;
   void show_confusion () ;

   // make this class an abstract class
   virtual void learn ( TrainingSet *tptr , struct LearnParams *lptr ) = 0 ;
   virtual void trial ( float *input ) = 0 ;
   virtual int wt_save ( FILE *fp ) = 0 ;
   virtual void wt_restore ( FILE *fp ) = 0 ;

#if BAD_COMPILER
   virtual ~Network() ;
#else
   virtual ~Network() = 0 ;
#endif

   int ok ;         // Was all constructor memory allocation successful?
   float *out ;    // Output neuron activations here if exe nonzero
   float neterr ;  // Mean square error of the network if executed
   int outmod ;     // Output model (see OUTMOD_? in CONST.H)
   int nin ;        // Number of input neurons
   int nout ;       // Number of output neurons
   int *confusion ; // Handy work area avoids malloc/free (see CONFUSE.CPP)

protected:

   float trial_error ( TrainingSet *tptr ) ;  // Compute training set error

   int exe ;        // Are work areas allocated to allow executing it?
} ;



/*
----------------------------------------------------------
HOPFIELD Network Class

Classical Hofield network with INPUT=OUTPUT and
Weights matrix =  INPUT*OUTPUT

----------------------------------------------------------
*/

class HopNet : public Network {

private:
   // weights matrix
   float *weights;
   int copy_weights(HopNet *dest, HopNet *source);
   void zero_weights ();
   void HopInit(TrainingSet *tptr);

public:
   // constructor : No hidden layer for Hopfield net
   HopNet (int n_inputs, int noutputs, int exe , int zero ) ;
   // destructor
   ~HopNet (){};
   // learn method defined previouly in Network abstract class
   void learn ( TrainingSet *tptr , struct LearnParams *lptr );

   void trial ( float *input );

   // save and restore weights
   int wt_save ( FILE *fp );
   void wt_restore ( FILE *fp );

};
/*
--------------------------------------------------------------------------------

   LayerNet

   Nhid1 and nhid2 are the number of neurons in the first and second layers
   respectively.  Either or both may be zero.  If nhid1 is zero, it is assumed
   that nhid2 is also zero.

   Weights for a layer are stored as a two dimensional matrix strung out into
   a vector.  For example, the first element in hid1_coefs is the weight
   connecting the first input neuron to the first hidden neuron.  The second
   connects the second input to the first hidden.  The nin+1 element is the
   bias for the first hidden unit.  The nin+2 connects the first input to the
   second hidden, et cetera.

--------------------------------------------------------------------------------
*/

class LayerNet: public Network {

public:

   LayerNet ( int outmodel , int n_inputs , int n_hidden1 , int n_hidden2 ,
	      int n_outputs , int exe , int zero ) ;
   ~LayerNet () ;
   void learn ( TrainingSet *tptr , struct LearnParams *lptr ) ;
   void trial ( float *input ) ;
   int wt_save ( FILE *fp ) ;
   void wt_restore ( FILE *fp ) ;

   int nhid1 ;              // Number of neurons in hidden layer 1
   int nhid2 ;              // Ditto for hidden layer 2

private:

   void anneal ( TrainingSet *tptr , struct LearnParams *lptr ,
		 LayerNet *bestnet , int init ) ;
   float conjgrad ( TrainingSet *tptr , int maxits ,
		     float reltol , float errtol ) ;
      void check_grad ( TrainingSet *tptr , float *grad ) ;
      float find_grad ( TrainingSet *tptr , float *hid2delta ,
			 float *outdelta , float *grad ) ;
      void find_new_dir ( float gam , float *g , float *h , float *grad ) ;
      float gamma ( float *g , float *grad ) ;
   void copy_weights ( LayerNet *dest , LayerNet *source ) ;
   float direcmin ( TrainingSet *tptr , float start_err ,
		     int itmax , float eps , float tol ,
		     float *base , float *direc ) ;
      void negate_dir ( float *direc ) ;
      void preserve ( float *base ) ;
      void step_out ( float step , float *direc , float *base ) ;
      void update_dir ( float step , float *direc ) ;
   void gen_init ( TrainingSet *tptr , struct LearnParams *lptr ) ;
   void perturb ( LayerNet *cent , LayerNet *perturbed , float temp ,
                  int regress) ;
   float regress ( TrainingSet *tptr , SingularValueDecomp *sptr ) ;
   void zero_weights () ;

   float *hid1_coefs ;     // nhid1 * (nin+1) weights (in changes fastest)
   float *hid2_coefs ;     // nhid2 * (nhid1+1) weights (hid1 changes fastest)
   float *out_coefs ;      // nout * (nhid?+1) weights (hid? changes fastest)
   float *hid1 ;           // Hid 1 neuron activations here if exe nonzero
   float *hid2 ;           // Ditto hidden layer 2
} ;

/*
--------------------------------------------------------------------------------

   KohNet

   There are no hidden units and no bias terms.
   The output weights are stored as a two dimensional matrix
   strung out into a vector.
   For example, the first element in out_coefs is the weight
   connecting the first input neuron to the first output neuron.
   The second connects the second input to the first output.
   Element nin+1 connects the first input to the second output, etc.

--------------------------------------------------------------------------------
*/

class KohNet: public Network {

public:

   KohNet ( int n_inputs , int n_outputs , KohParams *kp ,
	    int exe , int zero ) ;
   ~KohNet () ;
   void learn ( TrainingSet *tptr , struct LearnParams *lptr ) ;
   void trial ( float *input ) ;
   int wt_save ( FILE *fp ) ;
   void wt_restore ( FILE *fp ) ;

   int normalization ;      // 0=multiplicative, 1=Z

private:

   void copy_weights ( KohNet *dest , KohNet *source ) ;
   void epoch1 ( TrainingSet *tptr , float rate , int learn_method ,
		 int *won , float *bigerr , float *correc , float *work ) ;
   void epoch2 ( float rate , int learn_method , int *won ,
		 float *bigcorr , float *correc ) ;
   void force_win ( TrainingSet *tptr , int *won ) ;
   void initialize () ;
   void in_norm ( float *input , float *normfac , float *synth ) ;
   int winner ( float *input , float *normfac , float *synth ) ;
   void wt_norm ( float *w ) ;
   void zero_weights () ;

   float *out_coefs ;      // nout * (nin+1) weights
} ;



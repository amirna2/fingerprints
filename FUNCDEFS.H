/******************************************************************************/
/*                                                                            */
/*  FUNCDEFS - Function definition headers                                    */
/*                                                                            */
/******************************************************************************/

extern float	actderiv ( float f ) ;
extern float 	activity ( float *input , float *coefs , int n ) ;
extern float 	act_func ( float x ) ;
extern void 	act_func_init () ;
extern float 	dotprod ( int n , float *vec1 , float *vec2 ) ;
extern void 	error_message ( char *msg ) ;
extern void 	get_control_line ( char *control_line , int *control_file_number ,
				   FILE *control_files[] ) ;
extern float 	hop_activity (int in, float *input, float *weights, int n);
extern float 	inverse_act ( float f ) ;
extern long 	longrand () ;
extern long 	longrandmax () ;
extern void 	*memalloc ( unsigned int n ) ;
extern void	 memclose () ;
extern void 	memfree ( void *ptr ) ;
extern void 	*memrealloc ( void *ptr , unsigned int size ) ;
extern void 	memtext ( char *text ) ;
extern void 	memory_message ( char *msg ) ;
extern void 	normal_message ( char *msg ) ;
extern void 	nomemclose () ;
extern void 	notext ( char *text ) ;
extern float 	ParseDouble ( char **str ) ;
extern void 	progress_message ( char *msg ) ;
extern void	shake ( int nvars , float *center , float *x , float temp ) ;
extern void 	slongrand ( long iseed ) ;
extern void 	split_control_line ( char *line , char **command , int *n_command ,
				     char **rest , int *n_rest ) ;
extern void 	stack_control_file ( char *filename , int *control_file_number ,
				     FILE *control_files[] ) ;
void 		str_to_upr ( char *str ) ;
extern float 	unifrand () ;
extern float 	veclen ( int n , float *v ) ;
extern void 	warning_message ( char *msg ) ;
extern 		Network *wt_restore ( char *name , int *net_model ) ;
extern void 	wt_save ( Network *net , int net_model , int updating , char *name);

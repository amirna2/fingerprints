//////////////////////////////////
// Classes Vecteur et Matrice   //
//////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <sys\stat.h>

#include <alloc.h>
#include <dos.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

double logistic( double activation);

// Valeurs par d‚fauts

const ROWS    = 64;
const COLS    = 64;
const MAXVEC  = 64;

class mat;

// Classe vecteur

class vec {
	    friend ostream& operator<< (ostream& s, vec& v1);
	    friend class mat;
	    friend istream& operator>> (istream& s, vec& v1);

	    int n;
	    float *v;

	    public:

// Constructeur par d‚faut : Vec (Taille, valeur d'initialisation)
		vec (int size=MAXVEC, int val=0);
// Destructeur
	       ~vec ();
// Constructeur de copie
		vec (vec& v1); // constructeur par copie

		int   length();                         // norme du vecteur
		float dist(vec& A);                     // distance
		vec&  normalize();                      // normalisation
		vec&  normalizeon();
		vec&  scale(vec& minvec, vec& maxvec);  // etalonnage
		float d_logistic();                     // retourne V-1
		float maxval();

		vec&  garble(float noise);              // Bruitage des valeur

// Surcharge d'op‚rateurs
		vec&  operator=  (const vec& v1); // V1 = V2
		vec   operator+  (const vec& v1); // V1 + V2
		vec   operator+  (const float d); // V1 + k
		vec   operator-  (const vec& v1); // V1 - V2
		vec   operator-  (const float d); // V1 - k


		vec&  operator+= (const vec& v1); // V1 += V2
		int   operator== (const vec& v1); // V1 == V2
		float operator[] (int x);         // V1[i]
		float operator*  (const vec& v1); // V1 * V2
		vec   operator*  (float       c); // V1 * k
		vec&  sigmoid();
		vec&  set(int i, float f=0);

		int load(FILE *f);                // Lire vecteur … partir d'un fichier
		int save(FILE *f);                // Ecrire vecteur ds un fichier


		int   vec::maxindex();            
		vec&  getstr(char *s);
		void  putstr(char *s);
		float getval(int i);
}; // Classe vecteur

class vecp;


// Classe Matrice
class mat {
	    friend class hop;
	    friend class tsp;
	    friend ostream& operator<< (ostream& s, mat& m1);
	    friend istream& operator>> (istream& s, mat& m1);

	    protected:

	      float **m;  // la matrice
	      int r , c;  // lignes et colonnes

	    public:

// Constructeur par d‚faut
	      mat   (int n=ROWS, int p=COLS, float range=0);
// Constructeur d'initialisation par paramŠtres
	      mat   (int n, int p, float value, float range);
// Idem par fichier
	      mat   (int n, int p, char *fn);
// Par paire de vecteur
	      mat   (const vecp& vp);
// Par 2 vecteurs
	      mat   (vec& a,vec& b);
// Par copie
	      mat   ( mat& m1);
// Destructeur
	      ~mat  ();

// nb Lignes et nb colones
	      int depth();
	      int width();

	      mat&  operator=  (const mat& m1);         // M = M1
	      mat  operator+  (const mat& m1);          // M + M1

	      vec   operator*  (vec& v1);               // M * V1
	      vec   colslice   (int col);               // decalage colone
	      vec   rowslice   (int row);               // decalage ligne

	      void  insertcol  (vec& v, int col);       // insert col
	      void  insertrow  (vec& v, int row);       // insert lin

	      int   closestcol (vec& v);                // col la plus proche du vec v
	      int   closestrow (vec& v);                // ligne la plus proche du vec v

	      int   closestcol (vec& v, int *wins, float scaling);
	      int   closestrow (vec& v, int *wins, float scaling);

	      mat&  operator+= (const mat& m1);         // M += M1
	      mat&  operator*  (const float d);         // M * k
	      mat&  operator*= (const float d);         // M *= k

	      void  initvals   (const vec& v1, const vec& v2,
				const float rate = 1.0,
				const float momentum = 0.0);


	      float getval     (int row, int col);
	      void  setval     (int row, int col, float val);

	      int   save       (FILE *f);
	      int   load       (FILE *f);
}; // Classe matrice


// Classe paire de vecteurs

class vecp {
	     friend class mat;
	     friend ostream& operator<< (ostream& s, vecp& v1);
	     friend mat::mat( const vecp& vp);
	     friend ifstream& operator>> (ifstream& s, vecp& v1);

	     int flag;

	     public:

	       vec   *a;                        // Composante 1
	       vec   *b;                        // Composante 2

// Constructeur par d‚faut
	       vecp  (int n=ROWS, int p=COLS);
// Par Initialisation de paramŠtres
	       vecp  (int n, int p, int v);
// Par Initialisation de 2 vecteurs
	       vecp  ( vec& A, vec& B);
	       vecp  (const vecp& AB);   // constructeur par copie
// Destructeur
	       ~vecp ();

	       vecp& operator=  (const vecp& v1);          // VP = VP1
	       int   operator== (const vecp& v1);          // VP == VP1
	       vecp& scale(vecp& minvecs, vecp& maxvecs);

}; // Classe paire de vecteurs


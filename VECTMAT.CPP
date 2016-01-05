///////////////////////////////////////////////
// Methodes des classes Vecteur et Matrices  //
///////////////////////////////////////////////

#include "vectmat.h"

//  Methode de la classe vecteur


// Constructeur
vec::vec(int size, int val)
{
  v = new float [n=size];
  for (int i=0; i<n; i++)
    v[i]=val;
} 

vec::~vec() { delete v; }  // destructeur

vec::vec (vec& v1)  // constructeur de copie
{
  v=new float [n=v1.n];
  for (int i=0;i<n; i++)
    v[i]=v1.v[i];
}

vec&    vec::operator= (const vec& v1)
{
  delete v;
  v=new float [n=v1.n];
  for (int i=0;i<n;i++)
    v[i]=v1.v[i];
  return *this;
}

vec     vec::operator+ ( const vec& v1)
{
  vec sum(v1.n);
  for (int i=0; i<v1.n; i++)
    sum.v[i] = v1.v[i] + v[i];
    return sum;
}

vec     vec::operator+ (const float d)
{
  vec sum(n);
  for (int i=0;i<n;i++)
    sum.v[i]=v[i]+d;
    return sum;
}

vec&    vec::operator+= (const vec& v1)
{
  for (int i=0;i<n;i++)
    v[i]+=v1.v[i];
  return *this;
}

float   vec::operator*(const vec& v1)
{
  float sum=0.0;
  for (int i=0;i<min(n,v1.n);i++)
    sum+=(v1.v[i] * v[i]);
    return sum;
}

int     vec::operator==(const vec& v1)
{
  if (v1.n != n) return 0;    // tailles diff‚rentes
  for (int i=0;i< min(v1.n,n);i++)
    if (v1.v[i] != v[i]){
      return 0;
    }
    return 1;
}

float   vec::operator[] (int x)
{
  if (x<length() && x >=0)
    return v[x];
  else
    cerr << "Out of range";
  return 0;
}

int     vec::length() { return n; }

vec&    vec::garble(float noise)
{
  time_t t;
  time(&t);
  srand((unsigned)t);
  for (int i=0; i<n;i++)
  {
    if ( (rand()%10)/10 < noise)
      v[i]=1-v[i];
  }
  return *this;
}


// Normalisation V[i] = V[i] / Dim(V)

vec&    vec::normalize()
{
  for (int i=0;i<n;i++)
    v[i] /= n;
  return *this;
}

// Normalisation par composante non nulle
// V[i] = V[i] / On
// Avec On=On + 1 si V[i] <> 0

vec&    vec::normalizeon()
{
  int on=0;
  for (int i=0;i<n;i++)
    if (v[i]) on++;
  for (i=0; i<n;i++)
    v[i] /= on;
  return *this;
}

float   vec::maxval()
{
  float mx=0;
  for (int i=0;i<n;i++)
    if (fabs(v[i] > mx)) {
       mx= fabs(v[i]);
    }
  return mx;
}

vec&    vec::scale(vec& minvec, vec& maxvec)
{
  for (int i=0; i<n;i++)
  {
    if (v[i]<minvec.v[i])
      v[i]=0;
    else if ( (maxvec.v[i]-minvec.v[i]) == 0)
      v[i]=1;
    else
      v[i] = ( v[i] - minvec.v[i] ) / (maxvec.v[i] - minvec.v[i]);
  }
  return *this;
}


// Retourne ä (Vi * 1 - Vi)

float   vec::d_logistic ()
{
  float sum=0.0;
  for (int i=0;i<n;i++)
    sum +=(v[i] * (1-v[i]));
  return sum;
}

float   vec::dist(vec& A)
{
  float sum=0.0 , d;
  for (int i=0;i<n;i++)
  {
    d = v[i] - A.v[i];
    if (d)
      sum += pow(d,2);
  }
  return sum ? pow(sum, 0.5) : 0;
}

int     vec::maxindex()
{
  int idx, i;
  float mx=0.0;
  for (i=0,mx=-FLT_MAX; i<n;i++)
    if (v[i]>mx)
    {
      mx=v[i];
      idx=i;
    }
  return idx;
}

double  logistic ( double activation)
{
  if (activation > 11.5129)
	return 0.99999;
  if (activation <-11.5129)
	return 0.00001;
  return  1.0/(1.0 + exp(-activation));
}

vec&    vec::getstr(char *s)
{
  for (int i=0; i<MAXVEC&&s[i];i++)
  {
    if (isalpha(s[i]))
      v[toupper(s[i]) - 'A']=1;
  }
  return *this;
}

void    vec::putstr( char * s)
{
  int ct=0;
  for (int i=0;i<26;i++)
  {
    if (v[i]>0.9)
      s[ct++]='A'+i;
  }
}

vec     vec::operator- (const vec& v1)
{
  vec diff(n);
  for (int i=0;i<n;i++)
    diff.v[i]=v[i]-v1.v[i];
  return diff;
}

vec     vec::operator* (float c)
{
  vec prod(length());
  for (int i=0;i<n;i++)
    prod.v[i]=v[i]*c;
  return prod;
}


vec&    vec::sigmoid()
{
  for (int i=0;i<n;i++)
    v[i]=(float)logistic((double)v[i]);
  return *this;
}

vec&    vec::set(int i, float f)
{
	v[i]=f;
	return *this;
}
float   vec::getval(int i)
{
  return v[i];
}

///////////////////////////////////////////////////////////
// Lecture d'un vecteur … partir du periph. standard     //
///////////////////////////////////////////////////////////
istream&        operator>> (istream& s, vec& v1)
{
  float d;int i=0,c;
  for(;;)
  {
    s>>d;
    if (s.eof())
      return s;
    if (s.fail())
    {
      s.clear();
      do
	c=s.get();
      while (c!=',');
      return s;
    }

    v1.v[i++]=d;
    if (i==v1.n)
    {
      do
	c=s.get();
      while (c!=',');
      return s;
    }
  }
}

///////////////////////////////////////////////////////////
// Sortie d'un vecteur sur le peripherique standard      //
///////////////////////////////////////////////////////////
ostream&        operator<<(ostream& s, vec& v1)
{
  s.precision(2);
  for(int i=0; i<v1.n;i++)
    s << v1[i] <<" , ";
  s << "\n";
  return s;
}


///////////////////////////////////////////////////////////
// Sauve un vecteur dans un fichier                      //
///////////////////////////////////////////////////////////
int     vec::save (FILE *f)
{
  int success=1;
  for (int i=0;i<n;i++)
  {
    if (fwrite((&v[i]), sizeof(v[i]),1,f)<1)
      success=0;
  }
  return success;
}

///////////////////////////////////////////////////////////
// Lecture d'un vecteur … partir d'un fichier            //
///////////////////////////////////////////////////////////
int     vec::load (FILE *f)
{
  int success=1;
  for (int i=0;i<n;i++)
  {
    if (fread((&v[i]), sizeof(v[i]),1,f)<1)
      success=0;
  }
  return success;
}



///////////////////////////////////
// methodes de la classe matrice //
///////////////////////////////////

mat::mat(int n, int p, float range)
{
  int i,j,rnd;
  time_t t;
  int pct, val;

  m=new float *[n];
  if (range)
  {
    time (&t);
    srand ((unsigned)t);
  }
  for (i=0;i<n;i++)
  {
    m[i]= new float [p];
    for (j=0;j<p;j++)
    {
      if (range)
      {
	rnd = rand();
	pct =(int) (range * 100.0);
	val = rnd % pct;
	m[i][j] = (float) val / 100.0;
	if (range<0)
	  m[i][j] = fabs(range) - (m[i][j] *2.0);
      }
      else
	m[i][j]=0;
    }
  }
  r=n;
  c=p;
}

mat::mat(int n, int p, float value,float range)
{
  int i,j;

  m = new float *[n];
  for (i=0;i<n;i++)
  {
    m[i]=new float [p];
    for (j=0;j<p;j++)
      m[i][j]=range;
      m[i][j]=value;
  }
  r=n;
  c=p;
}
mat::mat (int n, int p, char *fn)
{
  int i,j,rnd;
  time_t t;
  m = new float *[n];
  for (i=0;i<n;i++)
    m[i]=new float [p];
  r=n;
  c=p;
  ifstream in (fn, ios::in);
  in >> *this;
}

mat::mat (const vecp& vp)
{
  r = vp.a->length();
  c = vp.b->length();
  m = new float *[r];
  for (int i=0;i<r;i++)
  {
    m[i] = new float[c];
    for (int j=0;j<c;j++)
      m[i][j]= vp.a->v[i]*vp.b->v[j];
  }
}


mat::mat (vec& a,vec& b)
{
  r = a.length();
  c = b.length();
  m = new float *[r];
  for (int i=0;i<r;i++)
  {
    m[i] = new float[c];
    for (int j=0;j<c;j++)
      m[i][j]= a.v[i] * b.v[j];
  }
}

mat::mat(mat& m1)
{
  r=m1.r;
  c=m1.c;
  m=new float *[r];
  for (int i=0;i<r;i++)
  {
    m[i] = new float[c];
    for (int j=0;j<c;j++)
      m[i][j]=m1.m[i][j];
  }
}


mat::~mat()     // destructeur
{
  for (int i=0; i<r;i++)
   delete m[i];
  delete m;
}

mat&    mat::operator= (const mat& m1)
{
  for (int i=0;i<r;i++)
    delete m[i];
  r=m1.r;
  c=m1.c;
  m = new float* [r];
  for (i=0;i<r;i++)
  {
    m[i] = new float [c];
    for (int j=0;j<r;j++)
      m[i][j] = m1.m[i][j];
  }
  return *this;
}

mat     mat::operator+ (const mat& m1)
{
  int i,j;
  mat sum (r,c);
  for (i =0;i<r;i++)
    for (j=0;j<c;j++)
      sum.m[i][j] = m1.m[i][j] + m[i][j];
  return sum;
}

mat&    mat::operator* (const float d)
{
  int i,j;
  for (i=0;i<r;i++)
    for (j=0;j<c;j++)
      m[i][j] *= d;
  return *this;
}

vec     mat::colslice (int col)
{
  vec tmp (r);
  for (int i=0;i<r;i++)
      tmp.v[i] = m[i][col];
  return tmp;
}

vec     mat::rowslice (int row)
{
  vec tmp (r);
  for (int i=0;i<r;i++)
      tmp.v[i] = m[row][i];
  return tmp;
}

void    mat::insertcol (vec& v, int col)
{
  for (int i=0;i<v.n;i++)
    m[i][col] = v.v[i];
}

void    mat::insertrow (vec& v, int row)
{
  for (int i=0;i<v.n;i++)
    m[row][i] = v.v[i];
}

int     mat::depth(){return r;}
int     mat::width(){return c;}

float   mat::getval(int row, int col)
{
  return m[row][col];
}
void    mat::setval(int row, int col, float val)
{
  m[row][col] = val;
}

int     mat::closestcol(vec& v)
{
  int mincol;
  float d;
  float mindist=INT_MAX;
  vec w(r);
  for (int i=0;i<c;i++)
  {
    w=colslice(i);
    if ( (d=v.dist(w)) < mindist )
    {
      mindist=d;
      mincol=i;
    }
  }
  return mincol;
}

int     mat::closestrow(vec& v)
{
  int minrow;
  float d;
  float mindist=INT_MAX;
  vec w(r);
  for (int i=0;i<c;i++)
  {
    w=rowslice(i);
    if ( (d=v.dist(w)) < mindist)
    {
      mindist=d;
      minrow=i;
    }
  }
  return minrow;
}

int     mat::closestrow(vec& v, int *wins, float scaling)
{
  int minrow;
  float d;
  float mindist=INT_MAX;
  vec w(c);
  for (int i=0;i<r;i++)
  {
    w=rowslice(i);
    d=v.dist(w);
    d*=(1+((float)wins[i]*scaling));
    if (d< mindist)
    {
      mindist = d;
      minrow = i;
    }
  }
  return minrow;
}



// methode de sauvegarde des poids est … d‚finir ult‚rieurement
// methode de chargement des poids est … d‚finir ult‚rieurement

mat&    mat::operator+= (const mat& m1)
{
  int i,j;
  for (i=0;i<r&&i<m1.r;i++)
    for (j=0;j<c&&j<m1.c;j++)
      m[i][j] += (m1.m[i][j]);
  return *this;
}

mat&    mat::operator*= (const float d)
{
  int i,j;
  for (i=0;i<r;i++)
    for (j=0;j<r;j++)
      m[i][j] *=d;
  return *this;
}

vec     mat::operator* (vec& v1)
{
  vec tmp(v1.n==r?c:r), tmp2(v1.n==r?c:r);
  for (int i=0;i < ((v1.n==r)?c:r);i++)
  {
    if (v1.n==r)
      tmp2 = colslice(i);
    else
      tmp2 = rowslice (i);
    tmp.v[i]=v1 * tmp2;
  }
  return tmp;
}

void    mat::initvals(const vec& v1, const vec& v2, const float rate, const float momentum)
{
  for (int i=0;i<r;i++)
    for (int j=0;j<r;j++)
      m[i][j] = ( m[i][j] * momentum ) + ( (v1.v[i] * v2.v[j]) * rate );
}

ostream&        operator<< (ostream& s, mat& m1)
// affiche une matrice
{
  for (int i=0;i<m1.r;i++)
  {
    for (int j=0;j<m1.c;j++)
    {
      s << m1.m[i][j] << "|";
    }
    s << "\n";
  }
  return s;
}

istream&        operator>> (istream& s, mat& m1)
// saisie d'une matrice
{
  for (int i=0;i<m1.r;i++)
  {
    for (int j=0;j<m1.c;j++)
    {
      s >> m1.m[i][j];
    }
  }
  return s;
}

int     mat::save(FILE *f)
{
  int success=1;
  for (int i=0;i<r;i++)
    for (int j=0;j<c;j++)
    {
      if ( fwrite (&m[i][j], sizeof(m[0][0]),1,f) < 1 )
	success=0;
    }
  return success;
}

int     mat::load(FILE *f)
{
  int success=1;
  for (int i=0;i<r;i++)
    for (int j=0;j<c;j++)
    {
      if (fread(&m[i][j], sizeof(m[0][0]),1,f) <1)
	success=0;
    }
  return success;
}

/////////////////////////////////
// methodes de la classe vecp  //
/////////////////////////////////

// constructeurs
vecp::vecp(int n, int p)
{
  a=new vec(n);
  b=new vec(p);
}
vecp::vecp(int n, int p, int v)
{
  a=new vec(n,v);
  b=new vec(p,v);
}
vecp::vecp(vec& A, vec& B)
{
  a = new vec (A.length());
  *a=A;
  b = new vec (B.length());
  *b=B;
}

vecp::vecp(const vecp& AB)
{
  *this=vecp( *(AB.a), *(AB.b) );
}

vecp::~vecp()
{
  delete a;
  delete b;
}


// Autres methodes

vecp&   vecp::operator=(const vecp& v1)
{
  *a= *(v1.a);
  *b= *(v1.b);
  return *this;
}

vecp&   vecp::scale(vecp& minvecs, vecp& maxvecs)
{
  a->scale ( *(minvecs.a), *(maxvecs.a) );
  b->scale ( *(minvecs.b), *(maxvecs.b) );
  return *this;
}

int     vecp::operator== ( const vecp& v1)
{
  return (*a == *(v1.a)) && (*b == *(v1.b));
}

ifstream&       operator>> (ifstream& s, vecp &v1)
// saisie d'une paire de vecteurs
{
  s >> *(v1.a) >> *(v1.b);
  return s;
}
ostream&        operator<< (ostream& s, vecp &v1)
{
  return s<<*(v1.a)<<*(v1.b)<<"\n";
}

/*******************************************/
/* Reconnaissance des empreintes digitales */
/* Nathoo Amir : Paris le 21/09/94         */
/*******************************************/

#include "finger.h"


#define FAUX 0
#define VRAI 1
#define ErreurGraphique 1
#define ErreurFichier   2


char *msg=NULL;
int x,y;
int ero_seuil,nb;
int xwin, ywin,co90,co0,co135,co45,cof;
UL pix=0;
UL nbpix=0,pixel;
enum coul {yellow,red,cyan,blue};

UL mat[4][4]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
UL mat2[4][4]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
UL mat3[4][4]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
UL mat4[4][4]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
UL mat5[4][4]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
FILE *out;

UL lin,col,l,c;
UL mem;

float v=0.0;

// fonction de Lecture d'un fichier image BMP
int f_lectureFichier (char* fichier, struct taille *FileImage)
{
	FILE *streamIn;
	int i,j;
	int couleur;
	struct  {
		  UL ligne;
		  UL colonne;
	} dimension;

	long filesize;
	unsigned hauteur, largeur;
	unsigned char *bufferfile=NULL;
	int nbOctetParLigne;
	int posdep;
	UL pos1,pos2;


	// Contr“le du fichier en entr‚e
	if ((streamIn = fopen (fichier,"rb")) == NULL)
	{
		outtext("Error : cannot open BMP file \n");
		closegraph();
		exit(-1);
	}

	filesize = filelength(fileno(streamIn));

	// Allocation m‚moire pour lecture du fichier
	bufferfile = malloc (30);
	if (bufferfile == NULL)
	{
	  closegraph();
	  printf("Erreur : allocation de m‚moire insuffisante\n");
	  exit(-3);
	}

	if (!fread(bufferfile,30,1,streamIn))
	{
	  outtext("Error : reading BMP file");
	  closegraph();
	  exit(-2);
	}

	// Nombre de colonnes
	memcpy (&largeur, bufferfile + 18,2);

	// Nombre de lignes
	memcpy (&hauteur, bufferfile + 22,2);

	// Ecriture dans le fichier de sortie
	// du nombre de lignes et colonnes ( zone entˆte )
	dimension.ligne = hauteur;
	dimension.colonne = largeur;
	FileImage->colonne = dimension.colonne;
	FileImage->ligne = dimension.ligne;

	// D‚placement … la zone des donn‚es pixels
	memcpy (&posdep,  bufferfile + 10,2);
	free (bufferfile);

	// Nombre d'octets par ligne
	nbOctetParLigne = (filesize - posdep) / hauteur;
	if ((bufferfile = malloc (nbOctetParLigne)) == NULL)
	{
	  closegraph();
	  printf("Erreur : allocation m‚moire insuffisante\n");
	  exit(-3);
	}

	// Lecture des pixels dans le fichier BMP
	ImageInit = (int huge*) farcalloc (dimension.ligne * dimension.colonne,(UL) sizeof(int));
	if (ImageInit == NULL)
	{
	  closegraph();
	  printf("Error ImageInit: Not enough memory to allocate Image buffers\n");
	  exit(-1);
	}

	for (i = 0; i < hauteur; i++)
	{  filesize = filesize - nbOctetParLigne;
		if (fseek (streamIn,filesize,SEEK_SET))
			break;
		if ( !fread (bufferfile,nbOctetParLigne,1,streamIn))
			break;
		pos2 = (UL)i * dimension.colonne;

		for (j = 0; j < largeur; j++)
		{       if (j % 2)
			{      couleur =  bufferfile[j / 2] & 0x0F;
			}
			else
			{      couleur = (bufferfile[j / 2] & 0xF0) >> 4;
			}
			pos1 = pos2 + (UL) j;
			if (!couleur)
			{       *(ImageInit + pos1) = WHITE;
			}
			else
				*(ImageInit + pos1) = BLACK;
		}
	}

	// Lib‚ration de la m‚moire et fermeture de fichier

	free(bufferfile);
	fclose (streamIn);
	return(0);
}





// =====
// D‚finie la direction des pixels dans une fenetre de xwin * ywin
// =====

void New_directions2(int huge *ImageSrc, int huge *ImageDest)
{

  int x,y,i,j,d;
  UL  n,p;

  p=0;

  for (x=0; x < Image.ligne; x=x+1)
  {
    for (y=0; y < Image.colonne; y=y+1)
    {
      p = x * Image.colonne + y;

      if (ImageSrc[p+Image.colonne+1]==WHITE)    // Si il y a un pixel au centre
      {
	if (ImageSrc[p+2*Image.colonne+2]) ImageDest[p+Image.colonne+1]= LIGHTRED;
	else {
	  if (ImageSrc[p+2*Image.colonne+1]) ImageDest[p+Image.colonne+1]=LIGHTCYAN;
	  else {
	    if (ImageSrc[p+2*Image.colonne]) ImageDest[p+Image.colonne+1]=LIGHTBLUE;
	    else {
	      if (ImageSrc[p+Image.colonne]) ImageDest[p+Image.colonne+1]=YELLOW;
	      else {
		if (ImageSrc[p]) ImageDest[p+Image.colonne+1]=LIGHTRED;
		else {
		  if (ImageSrc[p+1]) ImageDest[p+Image.colonne+1]=LIGHTCYAN;
		  else {
		    if (ImageSrc[p+2]) ImageDest[p+Image.colonne+1]=LIGHTBLUE;
		    else {
		      if (ImageSrc[p+Image.colonne+2]) ImageDest[p+Image.colonne+1]=YELLOW;
		    }
		  }
		}
	      }
	    }
	  }
	}
      }  // End if
    }
  }
}



// =====
// contour de l'image
// =====

void contour(int huge *ImageSrc,int huge *ImageDest)
{
	int x,y,i,j,centre;
	UL  n;
	for (x=0; x < Image.ligne; x=x+1)
	{
	  for (y=0; y < Image.colonne; y=y+1)
	  {
	     pix = x * Image.colonne + y;
	     if ( (ImageSrc[pix+1]==WHITE)&&
		  (ImageSrc[pix+Image.colonne]==WHITE)&&
		  (ImageSrc[pix+Image.colonne+2]==WHITE)&&
		  (ImageSrc[pix+(2*Image.colonne)+1]==WHITE)&&
		  (ImageSrc[pix+Image.colonne+1]==WHITE )
		)
		ImageDest[pix+Image.colonne+1]=BLACK;
	  }
	}
}


// =====
// Groupement 45ø 135ø
// =====

void groupe(int huge *ImageSrc, int huge *ImageDest)
{
  int y,x,dep;

  for (y=0; y < Image.ligne; y++)
  {
     for (x=0; x < Image.colonne; x++)
     {
       pix = y * Image.colonne + x;
       if (ImageSrc[pix+Image.colonne+1])
       {
	 if ( (ImageSrc[pix]) && (ImageSrc[pix+2*Image.colonne+2]) )
	 {
	   ImageDest[pix]=LIGHTRED;
	   ImageDest[pix+Image.colonne+1]=LIGHTRED;
	   ImageDest[pix+2*Image.colonne+2]=LIGHTRED;
	 }
	 else if ( (ImageSrc[pix+2]) && (ImageSrc[pix+2*Image.colonne]) )
	 {
	   ImageDest[pix+2]=LIGHTBLUE;
	   ImageDest[pix+Image.colonne+1]=LIGHTBLUE;
	   ImageDest[pix+2*Image.colonne]=LIGHTBLUE;
	 }
       }
     }
  }
}






// =====
// Erosion effectu‚e sur l'image initiale
// =====

void erosion(int huge *ImageSrc, int huge *ImageDest)
{

  int x,y,i,j;
  int w[9]={0,0,0,0,0,0,0,0,0};      // fenˆtre 3x3

  // masques d'‚rosion
  int m1[9] = {0,0,0, 0,15,0, 15,15,15};
  int m2[9] = {15,15,15, 0,15,0, 0,0,0};
  int m3[9] = {0,0,15, 0,15,15, 0,0,15};
  int m4[9] = {15,0,0, 15,15,0, 15,0,0};

  int co = Image.colonne;


  for (y=0; y < Image.ligne; y=y+1)
  {
    for (x=0; x < Image.colonne; x=x+1)
    {
      pix = y * Image.colonne + x;

      if (ImageSrc[pix+co+1]==WHITE)    // Si il y a un pixel au centre du la fenˆtre 3x3
      {
	 // initialise la fenˆtre courante
	 w[0]=ImageSrc[pix];
	 w[1]=ImageSrc[pix+1];
	 w[2]=ImageSrc[pix+2];
	 w[3]=ImageSrc[pix+co];
	 w[4]=ImageSrc[pix+co+1];
	 w[5]=ImageSrc[pix+co+2];
	 w[6]=ImageSrc[pix+2*co];
	 w[7]=ImageSrc[pix+2*co+1];
	 w[8]=ImageSrc[pix+2*co+2];

	 if ( (w[0]==m1[0]) && (w[1]==m1[1]) && (w[2]==m1[2]) &&
	      (w[3]==m1[3]) && (w[4]==m1[4]) && (w[5]==m1[5]) &&
	      (w[6]==m1[6]) && (w[7]==m1[7]) && (w[8]==m1[8]) )
	 {
	    ImageDest[pix+co+1]=BLACK; goto OUT_IF;
	 }


	 if ( (w[0]==m2[0]) && (w[1]==m2[1]) && (w[2]==m2[2]) &&
	      (w[3]==m2[3]) && (w[4]==m2[4]) && (w[5]==m2[5]) &&
	      (w[6]==m2[6]) && (w[7]==m2[7]) && (w[8]==m2[8]) )
	 {
	    ImageDest[pix+co+1]=BLACK; goto OUT_IF;
	 }

	 if ( (w[0]==m3[0]) && (w[1]==m3[1]) && (w[2]==m3[2]) &&
	      (w[3]==m3[3]) && (w[4]==m3[4]) && (w[5]==m3[5]) &&
	      (w[6]==m3[6]) && (w[7]==m3[7]) && (w[8]==m3[8]) )
	 {
	    ImageDest[pix+co+1]=BLACK; goto OUT_IF;
	 }

	 if ( (w[0]==m4[0]) && (w[1]==m4[1]) && (w[2]==m4[2]) &&
	      (w[3]==m4[3]) && (w[4]==m4[4]) && (w[5]==m4[5]) &&
	      (w[6]==m4[6]) && (w[7]==m4[7]) && (w[8]==m4[8]) )
	 {
	    ImageDest[pix+co+1]=BLACK; goto OUT_IF;
	 }

      }
      OUT_IF: ;
    }
  }
}

// ****
// Erosion effectu‚e sur le contour de l'image
// ****

void erosion2(int seuil,int huge *ImageDest)
{
	int y,x,dep;
	int total;

	for (y=0; y < Image.ligne; y++)
	{
	   for (x=0; x < Image.colonne; x++)
	   {
	     pix = y * Image.colonne + x;
	     total=0;
	     if (ImageDest[pix+Image.colonne+1])
	     {
		if ( (ImageDest[pix])) total++;
		if ( (ImageDest[pix+1])) total++;
		if ( (ImageDest[pix+2])) total++;
		if ( (ImageDest[pix+Image.colonne])) total++;
		if ( (ImageDest[pix+Image.colonne+2])) total++;
		if ( (ImageDest[pix+2*Image.colonne])) total++;
		if ( (ImageDest[pix+2*Image.colonne+1])) total++;
		if ( (ImageDest[pix+2*Image.colonne+2])) total++;
	     }
	     if (total >= seuil ) // On ‚limine le pixel sur image origine
	     {
		ImageDest[pix+Image.colonne+1]=BLACK;
	     }
	   }
	}
}

// ****
// Erosion sur l'image des orientations
// ****
void erosion3(int huge *ImageSrc, int huge *ImageDest)
{
  int nbcou[4]={0,0,0,0},ok=VRAI;
  int w[8]={0,0,0,0,0,0,0,0};
  int x,y,co,cou,i,max,maxind;

  co=Image.colonne;

  for (y=0; y < Image.ligne; y++)
  {
     for (x=0; x < Image.colonne; x++)
     {
	pix = y * Image.colonne + x;

	max=maxind=0;
	nbcou[yellow]=0;nbcou[red]=0;nbcou[cyan]=0;nbcou[blue]=0;

	ok=VRAI;

	w[0]=ImageSrc[pix]; w[1]=ImageSrc[pix+1]; w[2]=ImageSrc[pix+2];
	w[3]=ImageSrc[pix+co+1-1]; w[4]=ImageSrc[pix+co+1+1];
	w[5]=ImageSrc[pix+2*co]; w[6]=ImageSrc[pix+2*co+1]; w[7]=ImageSrc[pix+2*co+2];

	if (ImageSrc[pix+co+1])       // Si il y a un pixel au centre
	{
	  i=0;
	  while ( (i<=7)&&(ok) )   // V‚rifie si un pixel autour du centre
	  {                        // n'a pas la mˆme couleur que le centre
	    if (w[i]!=ImageSrc[pix+co+1])
	      i++;
	    else ok=FAUX;
	  }
	}

	if ( (ImageSrc[pix+co+1]) && (ok) )     // pixel centre <> des autres pixels
	{
	  for (i=0;i<=7;i++)        // Pour tous les pixel autour du centre
	  {
	    switch(w[i]) {
	      case YELLOW    : nbcou[yellow]++;break;
	      case LIGHTRED  : nbcou[red]++;break;
	      case LIGHTCYAN : nbcou[cyan]++;break;
	      case LIGHTBLUE : nbcou[blue]++;
	    }
	  }

	  // trouve la couleur dominante
	  for (i=yellow;i<=blue;i++)
	  {
	    if (nbcou[i] > max)
	    {
	      max = nbcou[i];
	      maxind=i;
	    }
	  }

	  if (nbcou[maxind]>=2)
	  {
	    switch (maxind) {
	      case yellow : ImageDest[pix+co+1]=YELLOW;break;
	      case red    : ImageDest[pix+co+1]=LIGHTRED;break;
	      case cyan   : ImageDest[pix+co+1]=LIGHTCYAN;break;
	      case blue   : ImageDest[pix+co+1]=LIGHTBLUE;break;
	    }
	  }
	  else ImageDest[pix+co+1]=BLACK;
	}
     }
  }
}

// ****
// Matrice de coocurence : 4 directions par couleur
// ****

void matrix_co4d ( int huge *ImageSrc,
			int lorg,int corg,int lend,int cend,UL ma[4][4] )
{
  int x,y,n,i,j,k,l,co;
  char txt[80];
  UL dj,p;

  nbpix=0;

  for (y=lorg; y < lend; y=y+1)
  {
    for (x=corg; x <=cend-1; x=x+1)
    {
      pix = y * Image.colonne + x;	// pixel courant
      if (ImageSrc[pix])
      {
	 // On regarde vers 0ø
	 for (i=1; i<=(cend-x) ;i++)
	 {
	    // Si on rencontre un pixel alors on sort de la boucle
	    if (ImageSrc[pix+i]!=BLACK) break;
	 }
	 if (ImageSrc[pix+i]==BLACK) goto OUTIF;

	 // Compte les cooccurences Pixel courant=pix, Pixel cible=pix+i
	 switch (ImageSrc[pix]) {
	   case YELLOW :
	     switch (ImageSrc[pix+i]) {
	       case YELLOW    : ma[yellow][yellow]++;break;
	       case LIGHTCYAN : ma[yellow][cyan]++;break;
	       case LIGHTBLUE : ma[yellow][blue]++;break;
	       case LIGHTRED  : ma[yellow][red]++;
	     }
	     break;
	   case LIGHTCYAN :
	     switch (ImageSrc[pix+i]) {
	       case YELLOW    : ma[cyan][yellow]++;break;
	       case LIGHTCYAN : ma[cyan][cyan]++;break;
	       case LIGHTBLUE : ma[cyan][blue]++;break;
	       case LIGHTRED  : ma[cyan][red]++;
	     }
	     break;
	   case LIGHTBLUE :
	     switch (ImageSrc[pix+i]) {
	       case YELLOW    : ma[blue][yellow]++;break;
	       case LIGHTCYAN : ma[blue][cyan]++;break;
	       case LIGHTBLUE : ma[blue][blue]++;break;
	       case LIGHTRED  : ma[blue][red]++;
	     }
	     break;
	   case LIGHTRED :
	     switch (ImageSrc[pix+i]) {
	       case YELLOW    : ma[red][yellow]++;break;
	       case LIGHTCYAN : ma[red][cyan]++;break;
	       case LIGHTBLUE : ma[red][blue]++;break;
	       case LIGHTRED  : ma[red][red]++;
	     }
	 } // End Switch 0ø

	 // On regarde vers 90ø
	 for (i=1;i<=(y-lorg);i++)
	 {
	    if (ImageSrc[pix-(Image.colonne*i)]!=BLACK) break;
	 }
	 if ( (ImageSrc[pix-(Image.colonne*i)]==BLACK) && (i>1)  ) goto OUTIF;

	 // Compte les cooccurences Pixel courant=pix, Pixel cible=pix-(dj*i)
	 switch (ImageSrc[pix]) {
	   case YELLOW :
	     switch (ImageSrc[pix-(Image.colonne*i)]) {
	       case YELLOW    : ma[yellow][yellow]++;break;
	       case LIGHTCYAN : ma[yellow][cyan]++;break;
	       case LIGHTBLUE : ma[yellow][blue]++;break;
	       case LIGHTRED  : ma[yellow][red]++;
	     }
	     break;
	   case LIGHTCYAN :
	     switch (ImageSrc[pix-(Image.colonne*i)]) {
	       case YELLOW    : ma[cyan][yellow]++;break;
	       case LIGHTCYAN : ma[cyan][cyan]++;break;
	       case LIGHTBLUE : ma[cyan][blue]++;break;
	       case LIGHTRED  : ma[cyan][red]++;
	     }
	     break;
	   case LIGHTBLUE :
	     switch (ImageSrc[pix-(Image.colonne*i)]) {
	       case YELLOW    : ma[blue][yellow]++;break;
	       case LIGHTCYAN : ma[blue][cyan]++;break;
	       case LIGHTBLUE : ma[blue][blue]++;break;
	       case LIGHTRED  : ma[blue][red]++;
	     }
	     break;
	   case LIGHTRED :
	     switch (ImageSrc[pix-(Image.colonne*i)]) {
	       case YELLOW    : ma[red][yellow]++;break;
	       case LIGHTCYAN : ma[red][cyan]++;break;
	       case LIGHTBLUE : ma[red][blue]++;break;
	       case LIGHTRED  : ma[red][red]++;
	     }
	 } // End Switch 90ø

	 // On regarde vers 135ø

	 i=y-1;j=x-1; // pixel suivant
	 while ( (i>=lorg)&&(j>=corg) )  // tant qu'on est pas sur un bord
	 {
	   p = i * Image.colonne + j;
	   if (ImageSrc[p]!=BLACK) break; // si pixel <> noir on sort
	   i--;
	   j--;
	 }
	 if (ImageSrc[p]==BLACK) goto OUTIF;// si pixel = noir on sort du if

	 // Compte les cooccurences Pixel courant=pix, Pixel cible=p
	 switch (ImageSrc[pix]) {
	   case YELLOW :
	     switch (ImageSrc[p]) {
	       case YELLOW    : ma[yellow][yellow]++;break;
	       case LIGHTCYAN : ma[yellow][cyan]++;break;
	       case LIGHTBLUE : ma[yellow][blue]++;break;
	       case LIGHTRED  : ma[yellow][red]++;
	     }
	     break;
	   case LIGHTCYAN :
	     switch (ImageSrc[p]) {
	       case YELLOW    : ma[cyan][yellow]++;break;
	       case LIGHTCYAN : ma[cyan][cyan]++;break;
	       case LIGHTBLUE : ma[cyan][blue]++;break;
	       case LIGHTRED  : ma[cyan][red]++;
	     }
	     break;
	   case LIGHTBLUE :
	     switch (ImageSrc[p]) {
	       case YELLOW    : ma[blue][yellow]++;break;
	       case LIGHTCYAN : ma[blue][cyan]++;break;
	       case LIGHTBLUE : ma[blue][blue]++;break;
	       case LIGHTRED  : ma[blue][red]++;
	     }
	     break;
	   case LIGHTRED :
	     switch (ImageSrc[p]) {
	       case YELLOW    : ma[red][yellow]++;break;
	       case LIGHTCYAN : ma[red][cyan]++;break;
	       case LIGHTBLUE : ma[red][blue]++;break;
	       case LIGHTRED  : ma[red][red]++;
	     }
	 } // End Switch 135ø

	 // On regarde vers 45ø
	 i=y-1;j=x+1;
	 while ( (i>=lorg)&&(j<=cend) )
	 {
	   p = i*Image.colonne + j;
	   if (ImageSrc[p]!=BLACK) break;
	   i--;
	   j++;
	 }
	 if (ImageSrc[p]==BLACK) goto OUTIF;

	 // Compte les cooccurences Pixel courant=pix, Pixel cible=p
	 switch (ImageSrc[pix]) {
	   case YELLOW :
	     switch (ImageSrc[p]) {
	       case YELLOW    : ma[yellow][yellow]++;break;
	       case LIGHTCYAN : ma[yellow][cyan]++;break;
	       case LIGHTBLUE : ma[yellow][blue]++;break;
	       case LIGHTRED  : ma[yellow][red]++;
	     }
	     break;
	   case LIGHTCYAN :
	     switch (ImageSrc[p]) {
	       case YELLOW    : ma[cyan][yellow]++;break;
	       case LIGHTCYAN : ma[cyan][cyan]++;break;
	       case LIGHTBLUE : ma[cyan][blue]++;break;
	       case LIGHTRED  : ma[cyan][red]++;
	     }
	     break;
	   case LIGHTBLUE :
	     switch (ImageSrc[p]) {
	       case YELLOW    : ma[blue][yellow]++;break;
	       case LIGHTCYAN : ma[blue][cyan]++;break;
	       case LIGHTBLUE : ma[blue][blue]++;break;
	       case LIGHTRED  : ma[blue][red]++;
	     }
	     break;
	   case LIGHTRED :
	     switch (ImageSrc[p]) {
	       case YELLOW    : ma[red][yellow]++;break;
	       case LIGHTCYAN : ma[red][cyan]++;break;
	       case LIGHTBLUE : ma[red][blue]++;break;
	       case LIGHTRED  : ma[red][red]++;
	     }
	 } // End Switch 45ø
	 OUTIF:;
      } // End if(Pix)
    }
  }

  for (k=yellow;k<=blue;k++)
  {
    for (l=yellow;l<=blue;l++)
    {
     nbpix=nbpix+ma[l][k];
    }
  }
  sprintf(txt,"%lu",nbpix);
  outtextxy(40,360,txt);
  setcolor(co0);      outtextxy(40,370,"0ø");
  setcolor(co135);       outtextxy(120,370,"135ø");
  setcolor(co90);outtextxy(200,370,"90ø");
  setcolor(co45);       outtextxy(280,370,"45ø");

  setcolor(co0);outtextxy(0,380,"0ø");
  setcolor(co135);outtextxy(0,390,"135ø");
  setcolor(co90);outtextxy(0,400,"90ø");
  setcolor(co45);outtextxy(0,410,"45ø");
  setcolor(WHITE);

  for (k=yellow;k<=blue;k++)
  {
    for (l=yellow;l<=blue;l++)
    {
     if (nbpix)
       sprintf(txt,"%3.2f%%", 100.0*((float)ma[l][k]/(float)nbpix) );
     else
       sprintf(txt,"%3.2f%%", 0);

       outtextxy(40+80*k,380+10*l,txt);

    }
  }
}


// *****
// Initialisation d'un matrice Image … zero
// *****

void InitImage(int huge *ImageDest)
{

  UL p = 0;
  for (y = 0; y < Image.ligne; y++)
    for (x = 0; x < Image.colonne; x++)
    {
     if (ImageDest[p])
       ImageDest[p] = 0;
     p++;
    }
}


// *****
// Affiche une image avec une couleur donn‚e
// *****

void ViewImage(int huge *ImageSrc,int color,int xoffset,int yoffset)
{
  UL p = 0;
  if (color)       // Si couleur est d‚finie alors affiche avec couleur
  {
    for (y = 0; y < Image.ligne; y=y+1)
    {
      for (x = 0; x < Image.colonne; x=x+1)
      {
	if (ImageSrc[p]) putpixel(x+xoffset,y+yoffset,color);
	else putpixel(x+xoffset,y+yoffset,BLACK);
	p++;
      }
    }
  }
  else   // Sinon affiche avec couleurs propre
  {
    for (y = 0; y < Image.ligne; y=y+1)
    {
      for (x = 0; x < Image.colonne; x=x+1)
      {
	if (ImageSrc[p]) putpixel(x+xoffset,y+yoffset,ImageSrc[p]);
	else putpixel(x+xoffset,y+yoffset,BLACK);
	p++;
      }
    }
  }
}


// ****
// Affiche une Image en Zoom
// ****

void ViewZoom(int huge *ImageSrc)
{
  UL p=0;
  pixel=0;
  for (y = 0; y < Image.ligne; y=y+1)
  {
      for (x = 0; x < Image.colonne; x=x+1)
      {

	if (ImageSrc[p]==LIGHTRED)
	{
	   pixel++;
	   putpixel(2*x,   2*y,co135);
	   putpixel(2*x+1, 2*y,co135);
	   putpixel(2*x,   2*y+1,co135);
	   putpixel(2*x+1, 2*y+1,co135);
	}
	else if (ImageSrc[p]==LIGHTBLUE)
	{
	   pixel++;
	   putpixel(2*x,   2*y,co45);
	   putpixel(2*x+1, 2*y,co45);
	   putpixel(2*x,   2*y+1,co45);
	   putpixel(2*x+1, 2*y+1,co45);
	}

	else if (ImageSrc[p]==LIGHTCYAN)
	{
	   pixel++;
	   putpixel(2*x,   2*y,co90);
	   putpixel(2*x+1, 2*y,co90);
	   putpixel(2*x,   2*y+1,co90);
	   putpixel(2*x+1, 2*y+1,co90);
	}

	else if (ImageSrc[p]==YELLOW)
	{
	   pixel++;
	   putpixel(2*x,   2*y,co0);
	   putpixel(2*x+1, 2*y,co0);
	   putpixel(2*x,   2*y+1,co0);
	   putpixel(2*x+1, 2*y+1,co0);
	}
	else
	{
	  putpixel(2*x,   2*y,ImageSrc[p]);
	  putpixel(2*x+1, 2*y,ImageSrc[p]);
	  putpixel(2*x,   2*y+1,ImageSrc[p]);
	  putpixel(2*x+1, 2*y+1,ImageSrc[p]);
	}
	p++;

      }
  }

}

// ****
// Copie ImageSrc dans ImageDest
// ****
void CopyImage(int huge *ImageDest,int huge *ImageSrc)
{

  UL p = 0;
  for (y = 0; y < Image.ligne; y++)
    for (x = 0; x < Image.colonne; x++)
    {

     ImageDest[p]=0;
     if (ImageSrc[p]!=BLACK)
       ImageDest[p] = ImageSrc[p];
     p++;
    }
}

void message(char *txt)
{
  setcolor(WHITE);
  outtextxy(0,470,txt);
  while (!getch());
  setfillstyle(EMPTY_FILL,BLACK);
  bar(0,470,480,480);
}

void window(int top,int left,int right,int bottom)
{
  setcolor(LIGHTMAGENTA);
  rectangle(top,left,right,bottom);
}

void clear_zone(int top, int left, int right, int bottom)
{
  setfillstyle(EMPTY_FILL,BLACK);
  bar(top,left,right,bottom);
}

// Point d'entr‚e du prg proncipal

void main( int argc, char *argv[])
{
	int gdriver = DETECT, gmode, errorcode;
	UL pos,m;
	int x,y,couleur,i;
	if (argc != 2)
	{
	  printf("Finger Step By Step Version 1.01\n");
	  printf("Developped by A. NATHOO Sept. 1994\n");
	  printf("Ecole Sup‚rieure d'Informatique\n");
	  printf("Laboratoire de Traitement d'Images\n");
	  printf("Professeur : Mr. AHAYAN\n");
	  printf("Usage is : Fing4d <file.bmp>\n");
	  exit(1);
	}
	co0   = LIGHTCYAN;
	co45  = LIGHTBLUE;
	co90  = YELLOW;
	co135 = LIGHTRED;
	cof   = BLACK;

	// recopie des paramŠtres
	xwin = 3;
	ywin = 3;

		// initialise le mode graphique
	initgraph(&gdriver, &gmode, "");
	errorcode = graphresult();
	if (errorcode != grOk)  // erreur d‚tect‚e
	{ Erreur = ErreurGraphique;
		printf("Erreur Graphique Code Nø %d \n",Erreur);
		exit(1);
	}

	MaxX = getmaxx();
	MaxY = getmaxy();

	// Lecture du fichier image
	f_lectureFichier (argv[1],&Image);

// Allocation de m‚moire pour les images

	ImageOut = (int huge*) farcalloc(Image.ligne*Image.colonne,(UL)sizeof(int));
	if (ImageOut == NULL)
	{
	  closegraph();
	  printf("Error : Not enough memory to allocate Image buffers\n");
	  exit(-1);
	}

	Image2 = (int huge*) farcalloc(Image.ligne*Image.colonne,(UL)sizeof(int));
	if (Image2 == NULL)
	{
	  closegraph();
	  printf("Error : Not enough memory to allocate Image buffers\n");
	  exit(-1);
	}

// Initialisation de ImageOut, Image2 … z‚ro
	InitImage(ImageOut);
	InitImage(Image2);

// Affiche ImageOut
	ViewZoom(ImageInit);
	message("Image Initiale : Tapez une touche");
// ImageOut=ImageInit
	CopyImage(ImageOut,ImageInit);
// Erosion de ImageInit : r‚sultat = ImageOut
	erosion(ImageInit,ImageOut);
	farfree(ImageInit);
// Affiche ImageOut
	ViewZoom(ImageOut);
	message("PremiŠre ‚rosion : tapez une touche");
// Image2 = ImageOut
	CopyImage(Image2,ImageOut);
// D‚termine contour de ImageOut : r‚sultat dans Image2
	contour(ImageOut,Image2);
// Groupe pixel 45ø 135ø
	CopyImage(ImageOut,Image2);
	groupe(Image2,ImageOut);
	ViewZoom(ImageOut);
	message("Groupement partiel : tapez une touche");
	CopyImage(Image2,ImageOut);
// Erosion du contour sur Image2
	erosion2(4,Image2);
// Affiche le contour ‚rod‚ : Image2
	ViewZoom(Image2);
	message("Contour ‚rod‚ : Tapez une touche");
// Init de ImageOut
	InitImage(ImageOut);
// D‚finir le orientations de l'empreinte : r‚sultat dans ImageOut
	CopyImage(ImageOut,Image2);
	New_directions2(Image2,ImageOut);
// affiche r‚sultat de New_directions2
	ViewZoom(ImageOut);
	message("Orientations Brutes : Tapez une touche");

// Copie ImageOut dans Image2
	CopyImage(Image2,ImageOut);
// Erosion de ImageOut : r‚sultat dans Image2
	erosion3(ImageOut,Image2);
// Affichage de Image2
	ViewZoom(Image2);
	message("Orientations finales : Tapez une touche");

  farfree(ImageOut);
  farfree(Image2);

  lin=Image.ligne;
  col=Image.colonne;

  if ((out = fopen("resu.dat", "at"))== NULL)
  {
   fprintf(stderr, "Erreur … l'ouverture de Resu.dat\n");
   exit(-1);
  }
  //fprintf(out,"%s\n",argv[1]);

// ZONE GLOBALE
  v=0.0;
  nbpix=0;
  matrix_co4d(Image2,0, 0, lin, col, mat5);
  message("Matrice : Zone globale, Tapez une touche");
  clear_zone(0,360,340,460);
  for (x=yellow;x<=blue;x++)
  {
    for (y=yellow;y<=blue;y++)
    {
      if ( nbpix )
	v= (float)mat5[y][x] / (float)nbpix;
      fprintf(out,"%2.5f ",v);
    }

  }



// ZONE 1
  nbpix=0;
  window(0,0,col,lin);

  matrix_co4d(Image2, 0, 0, lin/2, col/2,mat);
  message("Matrice : Zone 1, Tapez une touche");
  clear_zone(0,360,340,460);
  for (x=yellow;x<=blue;x++)
  {
    for (y=yellow;y<=blue;y++)
    {
      if (nbpix)
      v = (float)mat[y][x] / (float)nbpix;
      fprintf(out,"%2.5f ",v);
    }

  }


// ZONE 2
  window(col+1, 0, 2*col,lin);
  nbpix=0;
  matrix_co4d(Image2, 0, 1+col/2, lin/2 ,col, mat2);
  message("Matrice : Zone 2, Tapez une touche");
  clear_zone(0,360,340,460);
  for (x=yellow;x<=blue;x++)
  {
    for (y=yellow;y<=blue;y++)
    {
      if (nbpix)
      v= (float)mat2[y][x] / (float)nbpix;
      fprintf(out,"%2.5f ",v);
    }

  }


// ZONE 3
  window(0, lin+1, col, 2*lin);
  nbpix=0;
  matrix_co4d(Image2,1+lin/2, 0, lin, col/2, mat3);
  message("Matrice : Zone 3, Tapez une touche");
  clear_zone(0,360,340,460);
  for (x=yellow;x<=blue;x++)
  {
     for (y=yellow;y<=blue;y++)
     {
	if (nbpix)
	v= (float)mat3[y][x] / (float)nbpix;
	fprintf(out,"%2.5f ",v);
     }

  }


// ZONE 4
  window(col+1, lin+1, 2*col, 2*lin);
  nbpix=0;
  matrix_co4d(Image2,1+lin/2, 1+col/2, lin, col, mat4);
  message("Matrice : Zone 4, Tapez une touche");
  clear_zone(0,360,340,460);
  for (x=yellow;x<=blue;x++)
  {
    for (y=yellow;y<=blue;y++)
    {
      if (nbpix)
      v= (float)mat4[y][x] / (float)nbpix;
      fprintf(out,"%2.5f ",v);
    }

  }

  fprintf(out,"%s\n",argv[1]);
  fclose(out);
  closegraph();
}

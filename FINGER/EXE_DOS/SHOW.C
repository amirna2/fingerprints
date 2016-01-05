#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp1,*fp2,*fp3;

char *line=NULL,tx[8];
int n,i,j,trouve,nb,k;
char *nom[500]={NULL};
int  neurone, neuro[500];
double t;

void main( int argc, char *argv[])
{

  if (argc > 1)
    nb=atoi(argv[1]);
  else
    nb=1;
  line = (char *)calloc(20,sizeof(char));
  if ((fp1 = fopen("table.dat", "rt")) == NULL)
  {
    printf("Erreur : Ouverture de fichier Table.dat");
    exit(-1);
  }

  if ( (fp2 = fopen("test.out", "rt")) == NULL)
  {
    printf("Erreur : Ouverture du fichier test.out\n");
    exit(-1);
  }

  if ((fp3 = fopen("reconn.out", "wt")) == NULL)
  {
    printf("Erreur : Ouverture de fichier Reconn.out");
    exit(-1);
  }

  fgets(line,20, fp1);
  n=atoi(line);
  for (i=0;i<n;i++)
     nom[i] = (char *) calloc(20, sizeof(char));

  for (i=0;i<n;i++)
  {
    fscanf(fp1,"%d %s",&neuro[i],nom[i]);
  }
  for (j=1;j<=nb;j++)
  {
    for (k=1;k<=3;k++)
    {
      fscanf(fp2,"%d %s",&neurone,&tx);
      t=atof(tx);
      trouve=0;
      i=0;
      while( (i<n)&&(trouve==0) )   // Boucle de Parcours de NEURO[i]
      {
	// Lecture de table de correspondance
	// Compare Nø de neurone de test.out avec Nø de neurone de table.dat
	if (neurone==neuro[i])
	{
	  if (k==1)    // Si c'est le Premier Num‚ro
	  {
	    printf(     "EXEMPLE Nø %d\n",j);
	    fprintf(fp3,"EXEMPLE %d\n",j);
	    printf(     "RECONNU : %s => Activation = %3.2lf\n",nom[i],t);
	    fprintf(fp3,"RECONNU : %s => Activation = %3.2lf\n",nom[i],t);
	  }
	  else
	  {
	    printf(     "SUIVANT : %s => Activation = %3.2lf\n",nom[i],t);
	    fprintf(fp3,"SUIVANT : %s => Activation = %3.2lf\n",nom[i],t);
	  }
	  trouve=1;
	}
	i++;
      }
    }
  }
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  free(line);
  free(nom);
}
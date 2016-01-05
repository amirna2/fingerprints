#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp1,*fp2,*fp3;

char *line;
int n,i;
char *tab[500];
int  *neurone;

void main(void)
{
  clrscr();

  line=(char *)malloc(20);

  if ((fp1 = fopen("names.dat", "rt")) == NULL)
  {
    printf("Erreur : Ouverture de fichier");
    exit(-1);
  }

  if ( (fp2 = fopen("global.out", "rt")) == NULL)
  {
    printf("Erreur : Ouverture du fichier Visages.out\n");
    exit(-1);
  }
  if ((fp3 = fopen("table.dat", "wt")) == NULL)
  {
    printf("Erreur : Ouverture de fichier");
    exit(-1);
  }

  fgets(line,20, fp1);
  n=atoi(line);
  printf("Nb de noms=%d\n",n);

	neurone=malloc(n);

  fprintf(fp3,"%d\n",n);
  for (i=0;i<n*3;i=i+3)
  {
    tab[i]=malloc(20);
    fgets(tab[i],20, fp1);
    fgets(line,20, fp2);
    neurone[i]=atoi(line);
    printf("%d %s",neurone[i],tab[i]);
    fprintf(fp3,"%d %s",neurone[i],tab[i]);
  }
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);

  free(line);
  free(neurone);
}
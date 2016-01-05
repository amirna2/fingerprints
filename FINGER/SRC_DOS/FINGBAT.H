#include <stdio.h>
#include <alloc.h>
#include <io.h>
#include <conio.h>
#include <process.h>
#include <mem.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>


typedef unsigned long UL;

#ifndef PI
#define PI 3.141592653589793
#endif

struct taille {
	UL ligne;
	UL colonne;
};


int Erreur = 0;
int MaxX,MaxY;

struct taille Image;
int huge *ImageInit=NULL;
int huge *ImageOut=NULL;
int huge *Image2=NULL;



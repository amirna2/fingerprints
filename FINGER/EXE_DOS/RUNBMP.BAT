@echo off

echo 旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
echo � RECONNAISSANCE DES EMPREINTES DIGITALES �
echo � EXECUTABLE BMP, RUNBMP Fichier.bmp      �
echo � Amir NATHOO 1993-94 0293A               �
echo � ECOLE SUPERIEURE D'INFORMATIQUE         �
echo � Laboratoire de Traitement d'Images      �
echo � Professeur  M. AHYAN                    �
echo 읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�


if EXIST %1 goto RUN
if NOT ERRORLEVEL 1 goto FIN
goto fin
:RUN
fdemo %1
neuro2 test.con
show
del *.out
del resu.dat
:FIN




@echo off
cls
echo 旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
echo � RECONNAISSANCE DES EMPREINTES DIGITALES �
echo � EXECUTABLE TEXTE, RUNTXT Fichier.bmp    �
echo � Amir NATHOO 1993-94 0293A               �
echo � ECOLE SUPERIEURE D'INFORMATIQUE         �
echo � Laboratoire de Traitement d'Images      �
echo � Professeur  M. AHYAN                    �
echo 읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�


if EXIST %1 goto RUN
if NOT ERRORLEVEL 1 goto FIN
goto fin
:RUN
Echo TRAITEMENT DE L'EMPREINTE DIGITALE...
fingtxt %1
Echo RECONNAISSANCE DE L'EMPREINTE...
neuro2 test.con
show 1
del *.out
del resu.dat
:FIN




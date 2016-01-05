@echo off
cls
echo зддддддддддддддддддддддддддддддддддддддддд©
echo Ё RECONNAISSANCE DES EMPREINTES DIGITALES Ё
echo Ё EXECUTABLE TEXTE, RUNTXT Fichier.bmp    Ё
echo Ё Amir NATHOO 1993-94 0293A               Ё
echo Ё ECOLE SUPERIEURE D'INFORMATIQUE         Ё
echo Ё Laboratoire de Traitement d'Images      Ё
echo Ё Professeur  M. AHYAN                    Ё
echo юддддддддддддддддддддддддддддддддддддддддды


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




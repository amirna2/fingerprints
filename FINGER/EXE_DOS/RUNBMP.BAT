@echo off

echo зддддддддддддддддддддддддддддддддддддддддд©
echo Ё RECONNAISSANCE DES EMPREINTES DIGITALES Ё
echo Ё EXECUTABLE BMP, RUNBMP Fichier.bmp      Ё
echo Ё Amir NATHOO 1993-94 0293A               Ё
echo Ё ECOLE SUPERIEURE D'INFORMATIQUE         Ё
echo Ё Laboratoire de Traitement d'Images      Ё
echo Ё Professeur  M. AHYAN                    Ё
echo юддддддддддддддддддддддддддддддддддддддддды


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




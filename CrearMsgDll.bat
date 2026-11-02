::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: parametros
:: %1 $(InputPath) -> archivo fuete que se esta compilando *.mc (EjeM: .\cu_mensajes.mc)
:: %2 $(InputDir)  -> desde que directorio de esta haciendo (Ejem: .)
:: %3 $(OutDir)    -> directorio destino (Debug | Release) (Ejem: .\Release)
:: %4 $(PlarformSDK) --> directorio donde se encuentran los binarios del SDK
::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: primero verifico si hay que crear el GuruSvcMsg o no.

echo -------------------------
echo 1: %1
echo 2: %2
echo 3: %3
echo 4: %4
echo 5: %5
echo 6: %6
echo -------------------------

:: Ejem:
:: 1: "c:\Desarrollo\ControlUsuariosV2\src\desbloqueo\cu_mensajes.mc"
:: 2: "C:\Desarrollo\ControlUsuariosV2\src\build\vs2015\desbloqueo\x64\Debug\"
:: 3: "C:\Desarrollo\ControlUsuariosV2\src\build\vs2015\..\..\desbloqueo"
:: 4: "C:\Program Files (x86)\Windows Kits\8.1\Bin"
:: 5: "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\x86_amd64"
:: 6: desbloqueoMsg.dll

:: set PATH=%4;%5;%PATH%

:: cambio al directorio donde estan los archivos .obj o de intermedio
cd %2

:: creo en el directorio de los fuentes el archivo Mensajes.mc que luego sera mensajes.res
echo "type %3\..\..\..\Librerias\GuruSvcMsg.mc > %2\Mensajes.mc"
type %3\..\..\..\Librerias\GuruSvcMsg.mc > %2\Mensajes.mc

:: le agrego entradas personalizadas del proyecto.
echo "type %1 >> %2\Mensajes.mc"
type %1 >> %2\Mensajes.mc

echo mc.exe: %4\mc.exe -v -U %2\Mensajes.mc
%4\mc.exe -v -U Mensajes.mc

echo rc.exe:
%4\rc.exe Mensajes.Rc

echo %5\link.exe -dll -noentry Mensajes.res -out:%6
%5\link.exe -dll -noentry Mensajes.res -out:%6

echo Mensajes.h -^> %3\
move Mensajes.h %3\

echo salio bien?

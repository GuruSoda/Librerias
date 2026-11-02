//////////////////////////////////////////////////////////////////////
//
// Para Compilar los mensajes de la clase servicio 
// y agregarlo mas:
//
// 0) Craear un archivo .mc (si no se desean mensajes, que sea en blanco)
// 1) Ir a el archivo .mc creado con los nuevos mensajes (Que no se llame mensajes.mc)
// 2) Click derecho sobre el archivo .mc -> Settings/Properties
// 3) En Custom Buil
// 4) Commands -> Escribir
//	VS6		-->	..\..\Librerias\CrearGuruSvcMsg.bat $(InputPath) $(InputDir) $(OutDir)
//	VS2005		-->	$(SolutionDir)..\..\Librerias\CrearGuruSvcMsg.bat $(InputPath) $(InputDir) $(OutDir) "$(VSInstallDir)..\Microsoft Platform SDK\Bin"
//	VS2015(v100)	-->	"$(SolutionDir)..\..\..\..\Librerias\CrearGuruSvcMsg.bat" "%(FullPath)" "$(ProjectDir)$(ProjectName)\$(PlatformTarget)\$(Configuration)" "$(SolutionDir)%(RelativeDir)" "$(FrameworkSdkDir)Bin"
//	VS2015(v140)	-->	"$(SolutionDir)..\..\..\..\Librerias\CrearGuruSvcMsg.bat" "%(FullPath)" "$(ProjectDir)$(ProjectName)\$(PlatformTarget)\$(Configuration)" "$(SolutionDir)%(RelativeDir)" "$(FrameworkSdkDir)Bin\$(PlatformTarget)"
//
//	Para crear .dll
//	VS2015(v100)	-->	"$(SolutionDir)..\..\..\..\Librerias\CrearMsgDll.bat" "%(FullPath)" "$(IntDir)" "$(SolutionDir)%(RelativeDir)" "$(FrameworkSdkDir)Bin" "$(VCInstallDir)Bin" $(ProjectName)Msg.dll
//	VS2015(v140)	-->	"$(SolutionDir)..\..\..\..\Librerias\CrearMsgDll.bat" "%(FullPath)" "$(IntDir)" "$(SolutionDir)%(RelativeDir)" "$(FrameworkSdkDir)Bin\$(PlatformTarget)" "$(VC_ExecutablePath_x86_x64)" $(ProjectName)Msg.dll
// 5) OutPuts -> Escribir
//			Mensajes.rc Mensajes.h Msg0001.bin
//          o
//			$(InputName).rc $(InputName).h Msg0001.bin
// 6) Aceptar
// 7) Otra vez Click Derecho y Compilar ...
// 8) Si NO esta creando un .dll, Agregar al Proyecto el archivo Mensajes.rc
// 9) Rebuil All ...
// 10) Incluir la cabecera Mensajes.h para los mensajes generados en el .mc
//
// De forma manual:
// 1) "c:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\MC.Exe" -U Mensajes.mc
// 2) "c:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\rc.Exe" Mensajes.rc
// 3) "c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\link.exe" -dll -noentry Mensajes.res
// o
// 3) "c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\link.exe" -dll -noentry Mensajes.res -out:ejecutablemsg.dll
//
// Info:
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa363680(v=vs.85).aspx
// https://www.eventsentry.com/blog/2010/11/creating-your-very-own-event-m.html
// 
//////////////////////////////////////////////////////////////////////
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: GURUSVC_INSTALADO
//
// MessageText:
//
//  El Servicio %1 se ha instalado correctamente
//
#define GURUSVC_INSTALADO                0x00000001L

//
// MessageId: GURUSVC_DESINSTALADO
//
// MessageText:
//
//  El Servicio %1 se ha desintalado correctamente
//
#define GURUSVC_DESINSTALADO             0x00000002L

//
// MessageId: GURUSVC_INICIADO
//
// MessageText:
//
//  El Servicio %1 se ha iniciado
//
#define GURUSVC_INICIADO                 0x00000003L

//
// MessageId: GURUSVC_DETENIDO
//
// MessageText:
//
//  El Servicio %1 se ha detenido
//
#define GURUSVC_DETENIDO                 0x00000004L


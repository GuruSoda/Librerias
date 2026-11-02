######################################################################
# MakeFile para las librerias externas
######################################################################

######################################################################
# Existen 3 tipos de llamada a este Makefile
# 1) Directa:
#    Desde linea de comando ingresando el siguiente comando:
#    make -f GuruLibs.mak GURULIBRERIAS_FULL=1
#    o
#    make -f GuruLibs.mak GURULIBRERIAS_FULL=1 limpiar ZIP=1
# 2) Include:
#    Cuando desde otro archivo de proyecto es llamado como:
#    include ${GURULIBRERIAS}/GuruLibs.mak (Ver adelante como se hace)
# 3) Sub-Make:
#    Cuando desde otro archivo de proyecto se llamado como:
#    $(MAKE) -C ${GURULIBRERIAS} -f GuruLibs.mak
######################################################################

######################################################################
# Pasos a seguir para agregar las librerias a un Makefile
#
# 1) Agregar una variable llamada GURULIBRERIAS que contenga la ruta
#    del directorio donde se encuentran las librerias.
#    Ejem:
#         export GURULIBRERIAS = ../../Librerias
# 2) Agregar una variable llamada OBJ_GURULIB que contenga los
#    objetos a utilizar en el proyecto.
#    Ejem:
#          export OBJ_GURULIB := GuruLista.o GuruInterComConsola.o \
#          GuruLog.o GuruInterCom.o GuruZlib.o GuruUnZlib.o
# 3) Agregar las librerias externas que se deseen usar, pueden ser:
#    ZIP | PNG | JPG | LDAP | SDL
#    Ejem:
#         export ZIP := 1
# 4) Agregar un include hacia las librerias para que mapee los
#    objetos externos del proyecto a la ruta real.
#    Ejem:
#        # me mapea al objeto real ... -> ../../Librerias/Obj/GuruLista.o
#        include ${GURULIBRERIAS}/GuruLibs.mak
# 5) Llamar a GuruLibs.mak que se encuentra en el directorio de las
#    librerias
#    Existen dos formas de hacerlo (y recomiendo la 5.3):
#    5.1) Crar un objetivo con una llamada a las librerias y otra
#         llamada al programa a crear.
#         Ejem:
#              Compilar: Librerias Armacom.exe
#         Librerias:
#                 $(MAKE) -C ${GURULIBRERIAS} -f GuruLibs.mak
#         Armacom.exe: ${OBJ_PROG}
#                $(LD) $(LDFLAGS) -o $@ $(OBJ_PROG) $(OBJ_GURULIB)
#    5.2) Despues que se compilaron las librias del programa
#         Ejem:
#              Armacom.exe: ${OBJ_PROG}
#                     $(MAKE) -C ${GURULIBRERIAS} -f GuruLibs.mak
#                     $(LD) $(LDFLAGS) -o $@ $(OBJ_PROG) $(OBJ_GURULIB)
#    5.3) Agregando un pre-requisito con los objetos ($(OBJ_GURULIB))
#         Ejem:
#              Armacom.exe: ${PROG} ${OBJ_GURULIB} Makefile
#                     $(CXX) -s $(LDFLAGS) -o $@ $(PROG) $(OBJ_GURULIB)
#                     @echo $@ a sido creado ....
#              ${OBJ_GURULIB}:
#                     $(MAKE) -C ${GURULIBRERIAS} -f GuruLibs.mak
######################################################################

DIR_OBJS := Objs
DIRS_SRC := .
CC = gcc
CFLAGS += -Wall -pedantic

# cuando es ejecutado como: "make -f GuruLibs.mak GURULIBRERIAS_FULL=1"
ifdef GURULIBRERIAS_FULL
	ZIP := 1
	GURULIBS := 1
	PNG := 1
	JPEG := 1
#	LDAP := 1

else # por include o como sub-make
	ifneq ($(MAKELEVEL), 0)  #como sub-make
		# cuando es ejecutado como sub-make los datos en obj_gurulib vienen
		# con ../../Librerias/ entonces, se lo saco
        OBJ_GURULIB := $(subst ${GURULIBRERIAS}/,,${OBJ_GURULIB})
	endif
endif

######################################################################
# Para las librerias de Zip (Zlib)
######################################################################
ifeq (${ZIP},1)
DIR_ZIP = ZLib
TMP_ZLIB = adler32.o compress.o crc32.o deflate.o gzio.o infblock.o \
			infcodes.o inffast.o inflate.o inftrees.o infutil.o \
			trees.o uncompr.o unzip.o zip.o zutil.o

TMP_ZLIB := $(addprefix ${DIR_ZIP}/, ${TMP_ZLIB})

OBJ_GURULIB += ${TMP_ZLIB}

DIRS_SRC += ${DIR_ZIP}

ZIP := 0
endif

######################################################################
# Para las librerias de JPG (LibJpeg)
######################################################################
ifeq (${JPEG},1)
DIR_JPEG = LibJpeg
TMP_JPEG = jdphuff.o jdpostct.o jdsample.o jdtrans.o jerror.o jfdctflt.o \
			jfdctfst.o jfdctint.o jidctflt.o jidctfst.o jidctint.o jidctred.o \
			jmemmgr.o jmemnobs.o jquant1.o jquant2.o jutils.o jcapimin.o \
			jcapistd.o jccoefct.o jccolor.o jcdctmgr.o jchuff.o jcinit.o \
			jcmainct.o jcmarker.o jcmaster.o jcomapi.o jcparam.o jcphuff.o \
			jcprepct.o jcsample.o jctrans.o jdapimin.o jdapistd.o jdatadst.o \
			jdatasrc.o jdcoefct.o jdcolor.o jddctmgr.o jdhuff.o jdinput.o \
			jdmainct.o jdmarker.o jdmaster.o jdmerge.o

TMP_JPEG := $(addprefix ${DIR_JPEG}/, ${TMP_JPEG})

OBJ_GURULIB += ${TMP_JPEG}

DIRS_OBJ += DIR_JPEG

JPEG := 0
endif


######################################################################
# Para las librerias de PNG (LibPng)
######################################################################
ifeq (${PNG},1)
DIR_PNG = LibPng
TMP_PNG = png.o pngerror.o pnggccrd.o pngget.o pngmem.o pngpread.o \
			pngread.o pngrio.o pngrtran.o pngrutil.o pngset.o pngtest.o \
			pngtrans.o pngvcrd.o pngwio.o pngwrite.o pngwtran.o pngwutil.o

TMP_PNG := $(addprefix ${DIR_PNG}/, ${TMP_PNG})

OBJ_GURULIB += ${TMP_PNG}

DIRS_OBJ += DIR_PNG

PNG := 0
endif


######################################################################
# Para las Gurulibrerias de OpenLDAP
######################################################################
ifeq (${LDAP}, 1)
DIR_LDAP = LDAP

OBJ_GURULIB += abandon.o add.o addentry.o apitest.o bind.o cache.o \
				charray.o compare.o controls.o cyrus.o delete.o dnssrv.o \
				dntest.o error.o extended.o filter.o free.o ftest.o \
				getattr.o getdn.o getentry.o getvalues.o init.o kbind.o \
				messages.o modify.o modrdn.o open.o options.o os-ip.o \
				os-local.o print.o references.o request.o result.o sasl.o \
				sbind.o schema.o search.o sort.o sortctrl.o string.o \
				t61.o test.o tls.o unbind.o url.o utf-8-conv.o utf-8.o \
				util-int.o vlvctrl.o

LDAP := 0
endif

######################################################################
# Para las Gurulibrerias
######################################################################
ifeq (${GURULIBS},1)
DIR_GURULIBS = ${DIR_OBJS}

#OBJ_GURULIB := ${wildcard *.cpp}
#OBJ_GURULIB := ${OBJ_GURULIB:.cpp=.o}
#OBJ_GURULIB := $(addprefix ${DIR_OBJS}/, ${OBJ_GURULIB})

# OPERATIVO := $(findstring linux, $(OSTYPE))
# ANTERIOR_OPERATIVO := $(OSTYPE)
ifeq (${OPERATIVO},)
OPERATIVO = Windows
endif

# Las librerias puestas primeras son compatible para los dos sistemas operativos

OBJ_GURULIB += \
	Correr.o \
	GuruArchivos.o \
	GuruAsyncSocket.o \
	GuruBase64.o \
	GuruCRC.o \
	GuruCmd.o \
	GuruCmdPipe.o \
	GuruCmdSocket.o \
	GuruCorreo.o \
	GuruCorrer.o \
	GuruDb.o \
	GuruDiscoVirtual.o \
	GuruEncrypt.o \
	GuruFtp.o \
	GuruFtp32.o \
	GuruInterCom.o \
	GuruInterComConsola.o \
	GuruInterComSocket.o \
	GuruLista.o \
	GuruLog.o \
	GuruMD5.o \
	GuruMensaje.o \
	GuruPantalla.o \
	GuruParserMame.o \
	GuruPipes.o \
	GuruPoMaSrv.o \
	GuruProcesos.o \
	GuruProtocoloServicios.o \
	GuruSO.o \
	GuruSocket.o \
	GuruSocketArchivos.o \
	GuruSocketArchivosDar.o \
	GuruSocketArchivosEnviar.o \
	GuruSocketArchivosRecibir.o \
	GuruSocketArchivosTraer.o \
	GuruSocketNavegarDir.o \
	GuruSocketProtocolo.o \
	GuruSocketUtils.o \
	GuruUnZlib.o \
	GuruZlib.o \
	Log.o \

# Solo para las que funcionan en linux
ifeq (${OPERATIVO}, linux)
else
# Solo para las que funcionan en Windows
	ifeq (${OPERATIVO}, Windows)
	OBJ_GURULIB += \
		StdAfx.o \
		GuruDBLibrary.o \
		Guru.o \
		GuruNet.o \
		GuruMaquinasNT.o \
		GuruUsuariosNt.o \
    	GuruShareNT.o \
		GuruRegistry.o \
		GuruVisorEventos.o \
		GuruVisorEventosEscribir.o \
		GuruWTS.o \
		GuruError.o \
		GuruSvc.o \
		GuruSvcUtils.o \
		GuruServicios.o \
		GuruGruposNT.o \
		GuruIni.o \
		GuruSession.o
	endif
endif


# con errores
# Objs/GuruDiskID.o \
#	GuruUsuariosNt.o ---> con errores aun ....
#	GuruWTS.o  ---> con errores aun ....

GURULIBS := 0
endif

ifndef GURULIBRERIAS_FULL # por include o como sub-make
	ifeq ($(MAKELEVEL), 0) # como include
		OBJ_GURULIB := $(addprefix ${GURULIBRERIAS}/${DIR_OBJS}/, ${OBJ_GURULIB})
	endif
else # llamado por linea de comandos
	# aqui le digo cual es el directorio donde quedan los objetos
	OBJ_GURULIB := $(addprefix ${DIR_OBJS}/, ${OBJ_GURULIB})
endif

#OBJ_LDAP := ${wildcard D:/Proyectos/Desarrollo/Librerias/OpenLDAP/libraries/libldap/*.c}
#OBJ_LDAP := ${OBJ_LDAP:.c=.o}
#OBJ_LDAP := $(notdir ${OBJ_LDAP})

ifneq ($(MAKELEVEL), 0)

compilando: ${OBJ_GURULIB}
	@echo Librerias compiladas ...

endif

ifdef GURULIBRERIAS_FULL

inicio: Mensajes Completa

Mensajes:
	@echo "Creando Librerias para $(OPERATIVO) - $(ANTERIOR_OPERATIVO)"
	@echo "Por Compilar = ${OBJ_GURULIB}"
#	@echo DIRS_SRC = ${DIRS_SRC}
	@echo DIR_OBJS = ${DIR_OBJS}
#	@echo TMP_ZLIB = ${TMP_ZLIB}

Completa: ${OBJ_GURULIB}
	@echo GuruLibrerias creadas Correctamente

limpiar:
	@rm ${OBJ_GURULIB}

init:
	@mkdir ${DIR_OBJS}

all: ${OBJ_GURULIB}
	@echo Todos las librerias han sido compiladas en ${DIR_OBJS}

endif

# cuando es llamado desde linea de comandos
ifdef GURULIBRERIAS_FULL

${DIR_OBJS}/%.o:./%.cpp
	@echo Compilando C++ $? - $@
	@${CXX} -O2 -s -x c++ -pipe ${CFLAGS} -c $? -o $@

${DIR_OBJS}/%.o:./%.c
	@echo Compilando C $? - $@
	@gcc -c $? -o $@

endif

# cuando es llamado por sub-make
ifneq ($(MAKELEVEL), 0)
${DIR_OBJS}/%.o:./%.cpp
	@echo Compilando C++ $? - $@
	@${CXX} -O2 -s -x c++ -pipe ${CFLAGS} -c $? -o $@

${DIR_OBJS}/%.o:./%.c
	@echo Compilando C $? - $@
	@gcc -c $? -o $@
endif

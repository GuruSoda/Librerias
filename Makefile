######################################################################
# MakeFile para las GuruLibrerias
######################################################################

######################################################################
DIR_OBJS := Objs
CFLAGS += -Wall -pedantic
CC = gcc
CXX = gcc


ZIP := 1
GURULIBS := 1
GURUCOMPAT := 0
PNG := 0
JPEG := 0
SHA1 := 1
BASE64 := 0
GURUHTTP := 1

ifdef MINGDIR
	OSTYPE = Windows
else 
	OSTYPE = linux
endif

#ifeq (${OSTYPE},)
#endif


######################################################################
# Para las librerias de Zip (Zlib)
######################################################################
ifeq (${ZIP},1)
DIR_ZIP = zlib
TMP_ZLIB = adler32.o compress.o crc32.o deflate.o gzio.o \
	    inffast.o inflate.o inftrees.o \
	    trees.o uncompr.o unzip.o zip.o zutil.o

TMP_ZLIB := $(addprefix ${DIR_ZIP}/, ${TMP_ZLIB})

OBJ_GURULIB += ${TMP_ZLIB}

DIRS_OBJ += $(addprefix ${DIR_OBJS}/, ${DIR_ZIP})

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

DIRS_OBJ += $(addprefix ${DIR_OBJS}/, ${DIR_JPEG})

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

DIRS_OBJ += $(addprefix ${DIR_OBJS}/, ${DIR_PNG})

PNG := 0
endif

######################################################################
# Para las librerias de Base64
######################################################################
ifeq (${BASE64},1)
DIR_BASE64 = Base64
TMP_BASE64 = base64.o

TMP_BASE64 := ${DIR_BASE64}/${TMP_BASE64}

OBJ_GURULIB += ${TMP_BASE64}

DIRS_OBJ += $(addprefix ${DIR_OBJS}/, ${DIR_BASE64})

BASE64 := 0
endif


######################################################################
# Para las librerias de SHA1
######################################################################
ifeq (${SHA1},1)
DIR_SHA1 = sha1
TMP_SHA1 = sha1.o

TMP_SHA1 := $(addprefix ${DIR_SHA1}/, ${TMP_SHA1})

OBJ_GURULIB += ${TMP_SHA1}

DIRS_OBJ += $(addprefix ${DIR_OBJS}/, ${DIR_SHA1})

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
# Para las librerias Compatibilidad entre Sistemas Operativos
######################################################################
ifeq (${GURUCOMPAT},1)
DIR_COMPAT = GuruCompat
TMP_COMPAT = splitpath.o xtoa.o

TMP_COMPAT := $(addprefix ${DIR_COMPAT}/, ${TMP_COMPAT})

OBJ_GURULIB += ${TMP_COMPAT}

DIRS_OBJ += $(addprefix ${DIR_OBJS}/, ${DIR_COMPAT})

GURUCOMPAT := 0

endif

######################################################################
# Para las librerias de servidor http
######################################################################
ifeq (${GURUHTTP},1)
DIR_GURUHTTP = GuruHttp
TMP_GURUHTTP = Guruhttp.o GuruhttpConfig.o GuruhttpExcepcion.o GuruhttpInterface.o GuruhttpServidor.o

TMP_GURUHTTP := $(addprefix ${DIR_GURUHTTP}/, ${TMP_GURUHTTP})

OBJ_GURULIB += ${TMP_GURUHTTP}

DIRS_OBJ += $(addprefix ${DIR_OBJS}/, ${DIR_GURUHTTP})

GURUHTTP := 0

endif


######################################################################
# Para las Gurulibrerias
######################################################################
ifeq (${GURULIBS},1)
DIR_GURULIBS = ${DIR_OBJS}

# Las librerias puestas primeras son compatible para los dos sistemas operativos

OBJ_GURULIB += \
GuruLista.o \
GuruSocket.o \
GuruInterCom.o \
GuruInterComConsola.o \
GuruInterComSocket.o \
GuruSocketProtocolo.o \
GuruProtocoloCliente.o \
GuruMD5.o \
GuruCRC.o \
GuruError.o \
GuruParserMame.o \
GuruMameListInfo.o \
GuruBuffer.o \
GuruConfig.o \
GuruSincLogLeer.o \
GuruSHA1.o \
GuruAcceso.o \
GuruEtiquetaLeer.o \
GuruSincLog.o \
GuruAsyncSocket.o \
GuruDirectorio.o \
GuruSvc.o \
GuruEnvioCorreo.o \
GuruBase64.o \
GuruEtiquetaLeer.o \
GuruAcceso.o \
GuruDirectorio.o \
GuruHilo.o \
GuruHttp/GuruhttpInterface.o \
GuruHttp/Guruhttp.o \
GuruHttp/GuruhttpConfig.o \
GuruHttp/GuruhttpServidor.o \
GuruHttp/GuruhttpExcepcion.o 

#  GuruSocketArchivos.o \
GuruSocketArchivosDar.o \
GuruSocketArchivosEnviar.o \
GuruSocketArchivosTraer.o \
GuruSocketArchivosRecibir.o

		
# Solo para las que funcionan en linux
ifeq (${OSTYPE}, linux)
else
# Solo para las que funcionan en Windows (y estan por ser portadas)
	ifeq (${OSTYPE}, Windows)
	OBJ_GURULIB += \
		Correr.o \
		GuruArchivos.o \
		GuruCmd.o \
		GuruCmdPipe.o \
		GuruCmdSocket.o \
		GuruCorreo.o \
		GuruCorrer.o \
		GuruDb.o \
		GuruDiscoVirtual.o \
		GuruEncrypt.o \
		GuruFtp.o \
		GuruLog.o \
		GuruPipes.o \
		GuruPoMaSrv.o \
		GuruProcesos.o \
		GuruProtocoloServicios.o \
		GuruSO.o \
		GuruSocketNavegarDir.o \
		GuruSocketUtils.o \
		GuruUnZlib.o \
		GuruZlib.o \
		Log.o

# desde aca son librerias "Solo" de Windows
	OBJ_GURULIB += \
		StdAfx.o \
		GuruDBLibrary.o \
		Guru.o \
		GuruFtp32.o \
		GuruNet.o \
		GuruMaquinasNT.o \
		GuruUsuariosNt.o \
	    	GuruShareNT.o \
		GuruPantalla.o \
		GuruRegistry.o \
		GuruVisorEventos.o \
		GuruVisorEventosEscribir.o \
		GuruWTS.o \
		GuruSvcUtils.o \
		GuruServicios.o \
		GuruGruposNT.o \
		GuruIni.o \
		GuruMensaje.o \
		GuruSession.o
	endif
endif

OBJ_GURULIB := $(addprefix ${DIR_OBJS}/, ${OBJ_GURULIB})

GURULIBS := 0
endif

inicio: preparar compilar

compilar: ${OBJ_GURULIB}
	@echo Librerias compiladas ...

preparar:
ifndef OSTYPE
	@echo OSTYPE no definido !!!!!
	@echo "make [clean] OSTYPE=linux|windows"
	@exit 1
else
	@echo "Compilando para $(OSTYPE)"
endif
	@mkdir -p ${DIRS_OBJ}

${DIR_OBJS}/%.o:./%.cpp
	@echo "Compilando C++ $? -> $@"
	@${CXX} -I. -O2 -s -x c++ -pipe ${CFLAGS} -c $? -o $@

${DIR_OBJS}/%.o:./%.c
	@echo "Compilando C $? -> $@"
	@${CXX} -c $? -o $@

clean:
	@rm ${OBJ_GURULIB}


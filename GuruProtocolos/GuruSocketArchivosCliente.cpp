#include "GuruSocketArchivosCliente.h"

#ifndef GURUSOCKETARCHIVOSTRAER_NO
//////////////////////////////////////////////////////////////////////
// Tarea:		Traer un archivo por el protocolo Guru de transmision
//				de archivos.
// Parametro:	Nombre remoto de archivo.
//				Directorio local donde quedara guardado.
// Retorna:		true si pudo traer el archivo, false si no pudo
//////////////////////////////////////////////////////////////////////
//	enum GuruSocketArchivosCliente::respuesta_traer traer_archivo (const string& nombre_archivo, const string& directorio_local, uint32_t opciones_traer=0, uint32_t velocidad_limite=0);
enum GuruSocketArchivosCliente::respuesta_traer GuruSocketArchivosCliente::traer_archivo (GuruSocketArchivosCliente::transferencia& dataArchivoTraer)
{
	enum GuruSocketArchivosCliente::respuesta_traer codigo_salida;
/*
	try {
		conexion = FabricaSocketConectar(servidor, puerto);
		conexion.timeout(60); // espera en segundos.
	} catch (GuruSocketException& e) {
		msg_salida = e.what();
		codigo_salida = GuruSocketArchivosCliente::respuesta_traer::error_traer_conectando;

		return codigo_salida;
	}

	GuruProtocoloCliente a(conexion);
	ProtocoloRespuesta b;
*/
	try {
		/*
				if (!protocolo.empty())
					b = a.cmd_linea(protocolo);
				else b = a.respuesta();

				if (b.tipo() != GuruProtocolo::codigo_bien()) throw GuruSocketArchivosExceptionTraer(GuruSocketArchivosCliente::respuesta_traer::error_protocolo_traer, b.texto());
		*/
		//		GuruSocketArchivosTraer_Opciones = opciones_traer;
		//		archivo_trayendo.archivo_traer = nombre_archivo;

		GuruSocketArchivosTraer_Opciones = dataArchivoTraer.opciones();
		archivo_trayendo.archivo_traer = dataArchivoTraer.rutaCompletaRemota();

		ProtocoloRespuesta b;
		std::string cmd;

		cmd = "recibir";

		cmd += " archivo=" + GuruStringAlgo::comillas(dataArchivoTraer.rutaCompletaRemota());

		if (dataArchivoTraer.velocidad() != 0) {
			cmd += " velocidad=" + GuruStringAlgo::itos(dataArchivoTraer.velocidad());
			archivo_trayendo.velocidad_transmision = dataArchivoTraer.velocidad();
		}

		b = protocoloCliente.cmd_linea(cmd);

		if (b.tipo() != GuruProtocolo::codigo_bien()) throw GuruSocketArchivosExceptionTraer(GuruSocketArchivosCliente::respuesta_traer::error_traer, b.texto());

		GuruEtiquetaLeer tag(b.texto());

		string tmp(tag["tamanio"]);
		archivo_trayendo.tamanio = _atoi64(tmp.c_str());

		tmp = tag["fecha"];
		archivo_trayendo.fecha = GuruAlgo::RFC822FechaHora(tmp);

		// puede no venir el nombre del archivo local, lo tomo del archivo remoto
		if (dataArchivoTraer.nombreLocal().empty())
			dataArchivoTraer.nombreLocal(GuruDirectorio::GuruSplitPath(dataArchivoTraer.rutaCompletaRemota()).nombre_ext());

		archivo_trayendo.nombre = dataArchivoTraer.nombreLocal();
		archivo_trayendo.ruta_archivo_local = dataArchivoTraer.rutaCompletaLocal();

//		crear_nombre_archivo_local(dataArchivoTraer.rutaCompletaLocal());

		if (!GuruDirectorio::existe(dataArchivoTraer.directorioLocal())) GuruDirectorio::crearDirectorioRecursivo(dataArchivoTraer.directorioLocal());

		Notificar(InformacionArchivoTraer::traer);

		traer_contenido();

		b = protocoloCliente.respuesta();
		if (b.tipo() != GuruProtocolo::codigo_bien()) throw GuruSocketArchivosExceptionTraer(GuruSocketArchivosCliente::respuesta_traer::error_traer, b.texto());

		tag.cargar(b.texto());
		tmp = tag["SHA1"];
		if (!tmp.empty())
			if (archivo_trayendo.sha1 != tmp) 
				throw GuruSocketArchivosExceptionTraer(GuruSocketArchivosCliente::respuesta_traer::error_sha1_traer, string("Error en la verificacion de SHA1 (remoto:" + tmp + " <> recibido:" + archivo_trayendo.sha1 + ")."));

		string msg_final(b.texto());

		msg_salida = msg_final;
		codigo_salida = transferencia_correcta_traer;
	} catch (GuruSocketException& e) {
		msg_salida = e.what();
		codigo_salida = GuruSocketArchivosCliente::respuesta_traer::error_conexion_traer;
	} catch (GuruSocketArchivosExceptionTraer& e) {
		msg_salida = e.what();
		codigo_salida = e.codigo();
	}
/*
	// Traigo la linea de despedida
	b = a.cmd_linea("salir");

	if (!protocolo.empty()) 
		a.cmd("salir");

	conexion.cerrar();
*/
	return codigo_salida;
}

bool GuruSocketArchivosCliente::existe_archivo (const string& nombre, time_t fecha, uint64_t tamanio)
{
	try{
		std::string cmd;

		cmd = "existe";
		cmd += " archivo=" + GuruStringAlgo::comillas(nombre);

		if (fecha)
			cmd += " fecha=" + GuruStringAlgo::comillas(GuruAlgo::fechaHoraRFC822(fecha));
		if (tamanio)
			cmd += " tamanio=" + GuruStringAlgo::itos(tamanio);

		ProtocoloRespuesta respuesta(protocoloCliente.cmd_linea(cmd));
		if (respuesta.tipo() != GuruProtocolo::codigo_bien()) throw respuesta.texto();

		return true;
	}catch (GuruSocketException& e) {
		throw string(e.what());
	} catch (string e) {
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Traer el contenido del archivo remoto.
// Parametro:	Ninguno.
// Retorna:		true si lo pudo hacer, false si no pudo.
//////////////////////////////////////////////////////////////////////
void GuruSocketArchivosCliente::traer_contenido ()
{
	ios::openmode modo = ios::trunc | ios::binary;
	if (archivo_trayendo.agregar) {modo = ios::app | ios::binary;}

	ofstream archivo(archivo_trayendo.ruta_archivo_local.c_str(), modo);
	
	if (!archivo) 
		throw GuruSocketArchivosExceptionTraer(GuruSocketArchivosCliente::respuesta_traer::error_creando_archivo, GuruError().descripcion());

	uint32_t	bytes_leidos;
	sha1_context  sha1ctx;

	uint32_t	tamanio_buffer=BufferEntrada(conexion.handle());

	if (archivo_trayendo.velocidad_transmision != 0 && archivo_trayendo.velocidad_transmision < tamanio_buffer)
		tamanio_buffer=archivo_trayendo.velocidad_transmision;

	GuruBuffer	buffer_archivo(tamanio_buffer + 2);

	archivo.seekp(0, ios::beg);
	archivo.seekp(0, ios::end);
	archivo_trayendo.bytes_recibidos = archivo.tellp();

	if (archivo_trayendo.calcular_sha1) sha1_starts(&sha1ctx);

	uint32_t inicio,total_x_seg=0,segundos=0;
	inicio = GetTickCount();

	do {
		if (static_cast<uint64_t>(archivo_trayendo.tamanio - archivo_trayendo.bytes_recibidos) < static_cast<uint64_t>(tamanio_buffer))
			tamanio_buffer = static_cast<uint32_t>(archivo_trayendo.tamanio - archivo_trayendo.bytes_recibidos);

		bytes_leidos = recv(conexion.handle(), reinterpret_cast<char*>(buffer_archivo.buf()), static_cast<int>(tamanio_buffer), MSG_NOSIGNAL);

		if (bytes_leidos == (uint32_t)SOCKET_ERROR) break;

		archivo.write(reinterpret_cast<const char*>(buffer_archivo.buf()), static_cast<std::streamsize>(bytes_leidos));

		if (archivo.fail()) break;

		archivo_trayendo.bytes_recibidos += bytes_leidos;

		if (archivo_trayendo.calcular_sha1) sha1_update(&sha1ctx, (uint8_t*)buffer_archivo.buf(), (unsigned int)bytes_leidos);

		total_x_seg+=bytes_leidos;

		if (GetTickCount() >= inicio+1000) {
			segundos++;
			archivo_trayendo.bytes_seg=total_x_seg;
			archivo_trayendo.promedio_seg=static_cast<uint32_t>(archivo_trayendo.bytes_recibidos/static_cast<uint64_t>(segundos));

			inicio = GetTickCount();
			total_x_seg=0;

			Notificar(InformacionArchivoTraer::trayendo);
		}

	}while (archivo_trayendo.bytes_recibidos < archivo_trayendo.tamanio);

	archivo.close();

//////////////////////////////////////////////////////////////////////
// Cambio la fecha del archivo.
//////////////////////////////////////////////////////////////////////
	struct utimbuf times;

	times.actime = archivo_trayendo.fecha;
	times.modtime = archivo_trayendo.fecha;

	if (utime(archivo_trayendo.ruta_archivo_local.c_str(), &times) == -1) cerr << "Error al modicar fecha archivo: " << GuruError().descripcion() << endl;

//////////////////////////////////////////////////////////////////////
// Me fijo si llego el archivo completo
//////////////////////////////////////////////////////////////////////
	if (archivo_trayendo.bytes_recibidos != archivo_trayendo.tamanio) throw GuruError().descripcion();

//////////////////////////////////////////////////////////////////////
// Genero el SHA1.
//////////////////////////////////////////////////////////////////////
	if (archivo_trayendo.calcular_sha1) {
		uint8_t checksum[42]; // en realidad son 40 bytes
		sha1_finish(&sha1ctx,checksum);
//		sha1_digest(&sha1ctx, 20, checksum);
		archivo_trayendo.sha1 = GuruSHA1::checksum_a_string(checksum);
	}

	//////////////////////////////////////////////////////////////////////
	// Ejecuto el callback para el fin de la copia exitosa.
	//////////////////////////////////////////////////////////////////////
	Notificar(InformacionArchivoTraer::traido);
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Verificar si existe el archivo a traer.
// Parametro:	Ninguno.
// Retorna:		true si existe el archivo, false si no existe.
//////////////////////////////////////////////////////////////////////
bool GuruSocketArchivosCliente::existe_archivo()
{
	struct stat64 buffer;

	if (stat64(archivo_trayendo.ruta_archivo_local.c_str(), &buffer) == 0) {
		if ((archivo_trayendo.tamanio == static_cast<uint64_t>(buffer.st_size)) && (archivo_trayendo.fecha == buffer.st_mtime)) {
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
// Tarea:		Crear el nombre de como quedara localmente el archivo.
// Parametro:	Directorio en donde quedara.
// Retorna:		Nada.
//////////////////////////////////////////////////////////////////////
void GuruSocketArchivosCliente::crear_nombre_archivo_local(const string& directorio_local)
{
	GuruDirectorio::GuruSplitPath a(archivo_trayendo.archivo_traer);

	archivo_trayendo.nombre = a.nombre_ext();

	archivo_trayendo.ruta_archivo_local = GuruDirectorio::comoDirectorio(directorio_local) + a.nombre_ext();
}
#endif

#ifndef GURUSOCKETARCHIVOSENVIAR_NO
//////////////////////////////////////////////////////////////////////
// Tarea:	Enviar un archivo por socket, utizando el protocolo Guru
// Parametros:	Ruta completa local y directorio en la maquina
//				destino.
//////////////////////////////////////////////////////////////////////
// GuruSocketArchivosCliente::respuesta_enviar GuruSocketArchivosCliente::enviar_archivo(const string& nombre_archivo_local, const string& directorio_destino, uint32_t opciones_envio, uint32_t velocidad_limite)
GuruSocketArchivosCliente::respuesta_enviar GuruSocketArchivosCliente::enviar_archivo(GuruSocketArchivosCliente::transferencia& dataArchivoEnviar)
{
	enum GuruSocketArchivosCliente::respuesta_enviar codigo_salida;

	archivo_enviando.limpiar();

	GURULOG_DEBUG("Enviar archivo: " + dataArchivoEnviar.rutaCompletaLocal());

	try {
//		if (servidor.empty()) throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::falta_servidor, string("Falta el nombre de la computadora."));

//		if (nombre_archivo_local.empty()) throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::falta_archivo, string("Falta especificar el nombre del archivo a enviar."));

//		if (!cargar_data_archivo(nombre_archivo_local)) 
//			throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::error_archivo, GuruError().descripcion());

		if (!cargar_data_archivo(dataArchivoEnviar.rutaCompletaLocal()))
			throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::error_archivo, GuruError().descripcion());

//		conexion = FabricaSocketConectar(servidor, puerto);

//		GuruProtocoloCliente a(conexion);

		// recibo la bienvenida
//		if (!protocolo.empty())
//			b = a.cmd_linea(protocolo);
//		else b = a.respuesta();

//		if (b.tipo() != GuruProtocolo::codigo_bien()) throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::error_protocolo_enviar, b.texto());
		
//		GuruSocketArchivosEnviar_Opciones = opciones_envio;
		GuruSocketArchivosEnviar_Opciones = dataArchivoEnviar.opciones();

//		if ((GuruSocketArchivosEnviar_Opciones & enviar_limitar_velocidad) && velocidad_limite != 0)
//			archivo_enviando.velocidad_transmision = velocidad_limite;

		if (dataArchivoEnviar.velocidad() != 0)
			archivo_enviando.velocidad_transmision = dataArchivoEnviar.velocidad();

		std::string cmd;

		cmd = "enviar";

		cmd += " nombre=" + GuruStringAlgo::comillas(dataArchivoEnviar.nombreRemoto());
		cmd += " tamanio=" + GuruStringAlgo::itos(archivo_enviando.tamanio);
		cmd += " fechahora=" + GuruStringAlgo::comillas(GuruAlgo::fechaHoraRFC822(archivo_enviando.fecha));

		if (GuruSocketArchivosEnviar_Opciones & enviar_sha1) {
			archivo_enviando.sha1 = GuruSHA1::archivo(dataArchivoEnviar.rutaCompletaLocal());
			cmd += " sha1=" + archivo_enviando.sha1;

			GuruSocketArchivosEnviar_Opciones |= enviar_no_sha1;
			archivo_enviando.calcular_sha1 = false;
		}

		if (!dataArchivoEnviar.directorioRemoto().empty())
			cmd += " destino=" + GuruStringAlgo::comillas(dataArchivoEnviar.directorioRemoto());

		if (GuruSocketArchivosEnviar_Opciones & enviar_reemplazar) 
			cmd += " reemplazar=1";
		else 
			cmd += " reemplazar=0";

	#if defined __GURUWIN32__ && __CONTENIDO_COMPACTADO__
		if ((GuruSocketArchivosEnviar_Opciones & compactado))
			env_contenido_compactado(nombre_archivo_local);
		else
	#endif

		Notificar(InformacionArchivoEnviar::enviar);

		ProtocoloRespuesta b;

		b = protocoloCliente.cmd_linea(cmd);

		if (b.tipo() != GuruProtocolo::codigo_bien()) throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::error_enviar, b.texto());

		GuruEtiquetaLeerEx et(b.texto());

		if (et["Existe"].compare("1") == 0) { // el archivo existe y es exactamente el mismo
			throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::existente, string("Msg=\"El Archivo Existe.\""));
		} else if (et["Existe"].compare("2") == 0) {// el archivo existe pero el tamaño es menor lo que quiere decir que es una transmision cortada.
				string tmp(et["agregar"]);
				if (!tmp.empty()) archivo_enviando.agregar = _atoi64(tmp.c_str());
		} 

		cancelar(false);

		enviar_contenido();

		b = protocoloCliente.respuesta();
		if (b.tipo() != GuruProtocolo::codigo_bien()) throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::error_enviar, b.texto());

		GuruEtiquetaLeerEx et2(b.texto());

		if (!et2["SHA1"].empty())
			if (archivo_enviando.sha1 != et2["SHA1"]) throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::error_sha1_enviar, string("Error en la verificacion de SHA1, remoto:" + et2["SHA1"] + " local:" + archivo_enviando.sha1));

		string msg_final(b.texto());

		// Recibo la despedida
//		b = a.cmd_linea("salir");

//		if (!protocolo.empty()) a.cmd("salir");

		msg_salida = msg_final;
		codigo_salida = transferencia_correcta_enviar;
	} catch (GuruSocketException& e) {
		msg_salida = e.what();
		codigo_salida = GuruSocketArchivosCliente::respuesta_enviar::error_conexion_enviar;
	} catch (GuruSocketArchivosExceptionEnviar& e) {
		msg_salida = e.what();
		codigo_salida = e.codigo();
	}

//	conexion.cerrar();

	return codigo_salida;
}

void GuruSocketArchivosCliente::enviar_contenido()
{
	try{
		time_t inicio_transmision=GetTickCount();

		ifstream archivo(archivo_enviando.ruta_archivo_local.c_str(), ios::in | ios::binary);

		if (!archivo) throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::error_archivo, GuruError().descripcion());

		uint32_t	tamanio_buffer=BufferEntrada(conexion.handle());

		// calculo el tamanio optimo del buffer de salida para cuando se limita la velocidad
		if (archivo_enviando.velocidad_transmision) {
			uint32_t tmp_vel=archivo_enviando.velocidad_transmision;
			while (tmp_vel > tamanio_buffer) tmp_vel=tmp_vel/2;

			tamanio_buffer=tmp_vel;
		}

		uint32_t		enviados=0;
		GuruBufferChar	buffer_archivo(tamanio_buffer + 2);
		sha1_context	sha1ctx;

		uint64_t		inicio_envio=0, fin_envio=0;
		uint32_t		total_tiempo=0;
		uint64_t		total_bytes_enviados_seg=0;

		if (archivo_enviando.agregar != 0) {
			archivo.seekg(static_cast<ifstream::streamoff>(archivo_enviando.agregar));
			archivo_enviando.bytes_enviados = archivo.tellg();
		}

		if (archivo_enviando.calcular_sha1) sha1_starts(&sha1ctx);

		if (GuruSocketArchivosEnviar_Opciones & enviar_limitar_velocidad)
			inicio_envio = GetTickCount();

		GURULOG_DEBUG("time out en Socket: " + GuruStringAlgo::itos(conexion.timeout()));

		do{
			GURULOG_TRACE("Enviando datos...");

			archivo.read(buffer_archivo.buf(), tamanio_buffer);

			if (archivo.bad()) throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::error_archivo, GuruError().descripcion());

			enviados = (uint32_t)send(conexion.handle(), buffer_archivo.const_buf(), static_cast<int>(archivo.gcount()), MSG_NOSIGNAL);

		//	Sleep(120000);

			if (enviados == (uint32_t)SOCKET_ERROR) {
				int a = WSAGetLastError();
				conexion.cerrar();
				throw GuruSocketException(GuruError().descripcion(a));
			}

			archivo_enviando.bytes_enviados += enviados;

			if ((GuruSocketArchivosEnviar_Opciones & enviar_limitar_velocidad) && archivo_enviando.velocidad_transmision != 0) {
				total_bytes_enviados_seg += enviados;

				if (total_bytes_enviados_seg >= archivo_enviando.velocidad_transmision) {
					fin_envio = GetTickCount();
					total_tiempo = static_cast<uint32_t>(fin_envio - inicio_envio);
					if (total_tiempo <= 1000) {					 	
					 	if (static_cast<uint64_t>(1000) != total_tiempo) {
					 		Sleep((static_cast<uint32_t>(1000) - total_tiempo));
					 	} else {
					 		Sleep(1000);
					 	}
					}
					total_bytes_enviados_seg = 0;
					inicio_envio = GetTickCount();
				}
			}

			if (archivo_enviando.calcular_sha1) sha1_update(&sha1ctx, (uint8_t*)buffer_archivo.buf(), archivo.gcount());

			Notificar(InformacionArchivoEnviar::enviando);

		}while (archivo_enviando.bytes_enviados < archivo_enviando.tamanio && !cancelar_operacion);

		archivo.close();

		if (archivo_enviando.calcular_sha1) {
			uint8_t checksum[42]; // en realidad son 40 bytes
			sha1_finish(&sha1ctx, checksum);
//			sha1_digest(&sha1ctx, 20, checksum);
			archivo_enviando.sha1 = GuruSHA1::checksum_a_string(checksum);
		}

		time_t fin_transmision=GetTickCount();

		archivo_enviando.tiempo_transmision=static_cast<uint64_t>((fin_transmision-inicio_transmision));

		if (cancelar_operacion) throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::envio_cancelado, string("Cancelado por el usuario."));

		Notificar(InformacionArchivoEnviar::enviado);
	}catch (const string& e) {
		throw GuruSocketArchivosExceptionEnviar(GuruSocketArchivosCliente::respuesta_enviar::error_desconocido, e);
	}
}

#if defined __GURUWIN32__ && __CONTENIDO_COMPACTADO__
void GuruSocketArchivosCliente::env_contenido_compactado(const string& nombre_archivo_local)
{
	string ruta_archivo_local;

	ruta_archivo_local = nombre_archivo_local;

	HANDLE archivo;
	if ((archivo = CreateFile(ruta_archivo_local.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL )) != INVALID_HANDLE_VALUE)
		{
// envio el contenido del archivo

		int		tamanio_buffer			= 1024-2;
		char*	buffer_salida			= new char [tamanio_buffer];
		char*   buffer_no_compactado	= new char [GURUSOCKETARCHIVOS_BUFFER_COMPACTADO + ((GURUSOCKETARCHIVOS_BUFFER_COMPACTADO * 15) / 100) + 25];
		char*	buffer_compactado		= new char [GURUSOCKETARCHIVOS_BUFFER_COMPACTADO + ((GURUSOCKETARCHIVOS_BUFFER_COMPACTADO * 15) / 100) + 25];

		dword	enviados=0;
		dword	cont_enviados=0;

//		if (agregar) cont_enviados = SetFilePointer(archivo, data.inicio, 0, FILE_BEGIN); // != INVALID_SET_FILE_POINTER !!

		HANDLE hlectura = NULL;
		thread_carga thread_lectura;

		thread_lectura.archivo = archivo;
		thread_lectura.buffer_compactado = buffer_compactado;
		thread_lectura.buffer_no_compactado = buffer_no_compactado;
		thread_lectura.extra = this;
		thread_lectura.tamanio_buffer_compactado = GURUSOCKETARCHIVOS_BUFFER_COMPACTADO + ((GURUSOCKETARCHIVOS_BUFFER_COMPACTADO * 15) / 100) + 25;
		thread_lectura.tamanio_buffer_no_compactado = GURUSOCKETARCHIVOS_BUFFER_COMPACTADO;

		// se?alizado == libre | wait == no espera
		// no se?alizado == ocupado | wait == espera
		evento_cargado = CreateEvent(NULL, false, false, NULL);

		evento_entrada_salida = CreateEvent(NULL, false, false, NULL);

		evento_terminar = CreateEvent(NULL, true, false, NULL);

		ResetEvent(evento_terminar);

		dword tid;
		hlectura = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hilo_lectura, &thread_lectura, 0, &tid);

		HANDLE	hvector[2];

		hvector[0] = hlectura;
		hvector[1] = evento_cargado;


		do{
			// espero a que se cargue el buffer o termine el thread
			if (WaitForMultipleObjects(2, hvector, false, INFINITE) == WAIT_OBJECT_0 + 0)
				break;

			// buffer compactado tiene la data compactada
			char* temp_buffer_compactado = new char [thread_lectura.tamanio_buffer_compactado + 2];
			memcpy(temp_buffer_compactado, thread_lectura.buffer_compactado, thread_lectura.tamanio_buffer_compactado);

			dword tamanio_compactado_actual;
			memcpy(&tamanio_compactado_actual, temp_buffer_compactado, sizeof(unsigned long));

			dword tamanio_no_compactado_actual;
			memcpy(&tamanio_no_compactado_actual, temp_buffer_compactado + sizeof (unsigned long), sizeof(unsigned long));

			thread_lectura.tamanio_buffer_compactado = GURUSOCKETARCHIVOS_BUFFER_COMPACTADO + ((GURUSOCKETARCHIVOS_BUFFER_COMPACTADO * 15) / 100) + 25;
			thread_lectura.tamanio_buffer_no_compactado = GURUSOCKETARCHIVOS_BUFFER_COMPACTADO;

			// vuelvo a cargar el buffer
			SetEvent(evento_entrada_salida); // libero la espera

			// por el header tamanio compactado
			tamanio_compactado_actual += sizeof(unsigned long);

			// por el header tamanio sin compactar
			tamanio_compactado_actual += sizeof(unsigned long);

			dword cantidad = (tamanio_compactado_actual / tamanio_buffer);
			dword total_enviado = 0;
            dword i;

	//		dump(&temp_buffer_compactado[8], tamanio_compactado_actual - 8);
			for (i = 0; i < cantidad; i++)
				{
				memcpy(buffer_salida, &temp_buffer_compactado[i * tamanio_buffer], tamanio_buffer);

				enviados = send(Guru_Socket, buffer_salida, tamanio_buffer, 0);

				if (enviados == (dword)SOCKET_ERROR)
					{
					// TerminateThread(hlectura, 1);
					SetEvent(evento_terminar);		// le digo que termine el thread
					GuruSocket_error.descripcion();
					break;
					}
				else
					{
					total_enviado += enviados;

//					if (notificar) (*notificar) (data.tamanio, cont_enviados + total_enviado, NULL);
					}
				}

			// error en el envio?
			if (i != cantidad) break;

			// por la diferencia (bytes que faltan enviar)
			if (total_enviado < tamanio_compactado_actual)
				{
				memcpy(buffer_salida, &temp_buffer_compactado[total_enviado], (tamanio_compactado_actual - total_enviado));

				enviados = send(Guru_Socket, buffer_salida, (tamanio_compactado_actual - total_enviado), 0);

				total_enviado += enviados;
				}

//			dump(&temp_buffer_compactado[8], total_enviado - 8);

			delete temp_buffer_compactado;
			temp_buffer_compactado = NULL;

			cont_enviados += tamanio_no_compactado_actual;

		}while (cont_enviados < archivo_enviando.tamanio);

		dword salida_hilo = 0;

		GetExitCodeThread(hlectura, &salida_hilo);

		if (salida_hilo == STILL_ACTIVE)
			{
			SetEvent(evento_terminar);	// le digo que termine el thread
			// TerminateThread(hgrabar, 1);
			// espero a que se haya escrito en disco
			WaitForSingleObject(hlectura, INFINITE);
			}


		if (buffer_salida)
			delete buffer_salida;

		if (buffer_no_compactado)
			delete buffer_no_compactado;

		if (buffer_compactado)
			delete buffer_compactado;

		CloseHandle(archivo);

		CloseHandle(evento_cargado);

		CloseHandle(evento_entrada_salida);

		CloseHandle(evento_terminar);
		}

	if (respuesta_correcta(readline()) == bien)	respuesta_contenido = sacar_texto();
}
#endif

#if defined __GURUWIN32__ && __CONTENIDO_COMPACTADO__
void GuruSocketArchivosCliente::hilo_lectura(thread_carga* info)
{
	GuruSocketArchivosEnviar *esto = (GuruSocketArchivosEnviar*)info->extra;
	dword	leidos, salida	= 0;
	dword	bytes_leidos	= 0;
	dword	tamanio_archivo = GetFileSize(info->archivo, NULL);
	HANDLE	hvector[2];

	hvector[0] = esto->evento_terminar;
	hvector[1] = esto->evento_entrada_salida;
/*
	dword a = Z_OK;
	a = Z_MEM_ERROR;
	a = Z_BUF_ERROR;
	a = Z_STREAM_ERROR;
*/

	do {
		// indico que esta cargando
		ResetEvent(esto->evento_cargado); // == pongo en estado no-se?alizado (ocupado)

		if (ReadFile(info->archivo, info->buffer_no_compactado, info->tamanio_buffer_no_compactado, &leidos, NULL))
			{
			if ((leidos == 0) && (GetLastError() == ERROR_HANDLE_EOF))
				{
				esto->GuruSocket_error.descripcion();
				salida = GetLastError();
				}
			else
				{
				salida = compress((unsigned char*)info->buffer_compactado, (unsigned long *)&info->tamanio_buffer_compactado, (unsigned char *)info->buffer_no_compactado, leidos);
				if (salida == Z_OK)
					{
					memset(info->buffer_no_compactado, 0, info->tamanio_buffer_no_compactado);

					// armo el header que dice la cantidad de bytes compactados que van
					memcpy(info->buffer_no_compactado, &info->tamanio_buffer_compactado, sizeof(unsigned long));

					// armo el header que dice la cantidad de bytes no compactados que van
					memcpy(info->buffer_no_compactado + sizeof(unsigned long), &leidos, sizeof(unsigned long));

					// mando la data compactada
					memcpy(info->buffer_no_compactado + (2 * sizeof(unsigned long)), info->buffer_compactado, info->tamanio_buffer_compactado);

					// copy a buffer compactado, la data compactada y con el header
					memcpy(info->buffer_compactado, info->buffer_no_compactado, info->tamanio_buffer_compactado +  (2 * sizeof(unsigned long)));

					info->tamanio_buffer_compactado = info->tamanio_buffer_compactado + (2 * sizeof(unsigned long));
					}
				}
			}
		else
			{
			esto->GuruSocket_error.descripcion();
			salida = GetLastError();
			}

		if (salida != 0)
			break;

		bytes_leidos += leidos;

		// indico que se termino de cargar
		SetEvent(esto->evento_cargado); // ==  pongo en estado se?alizado (libre)

		OutputDebugString("Esperando cargar otra vez\n");

		// espero que me avisen que tengo que volver a llenar el buffer
		if (WaitForMultipleObjects(2, hvector, false, INFINITE) == WAIT_OBJECT_0 + 0)
			{
			OutputDebugString("Terminado por Evento Terminar\n");
			ExitThread(0);
			}

		OutputDebugString("Fin de la espera de carga\n");

	}while (bytes_leidos < tamanio_archivo);

	OutputDebugString("Terminado Normal\n");
	ExitThread(salida);
}
#endif

bool GuruSocketArchivosCliente::cargar_data_archivo(const string& nombre_archivo_local)
{
	struct stat64 buffer;
	int ret = 0;
	if ((ret=stat64(nombre_archivo_local.c_str(), &buffer)) == 0) {
		char nombre[_MAX_FNAME];
		char extension[_MAX_EXT];

		_splitpath(nombre_archivo_local.c_str(), NULL, NULL, nombre, extension);

		archivo_enviando.nombre = string(nombre) + extension;
		archivo_enviando.tamanio = buffer.st_size;
		archivo_enviando.fecha = buffer.st_mtime;
		archivo_enviando.ruta_archivo_local = nombre_archivo_local;

		return true;
	} else 
		return false;
}

bool GuruSocketArchivosCliente::comparar_sha1(const string& sha1, const string& nombre_archivo)
{
	string archivo;
	archivo = GuruSHA1::archivo(nombre_archivo);

	return (stricmp(archivo.c_str(), sha1.c_str()) == 0) ? true : false ;
}

void GuruSocketArchivosCliente::directorio(const string& dire)
{
	std::string cmd;

	cmd = "dir";

	cmd += " directorio=" + GuruStringAlgo::comillas(dire);

	for (GuruProtocoloCliente::iterator it= protocoloCliente.begin(cmd);it!= protocoloCliente.end();++it)
		cout << it->texto() << endl;

/*
	vector<ProtocoloRespuesta> archivos;
	a.cmd_multilinea(oss.str(), archivos);

	for (vector<ProtocoloRespuesta>::iterator it=archivos.begin();it!=archivos.end();++it)
		cout << it->texto() << endl;
*/
}

#endif

#if !defined __GURUEJECUTAR_HPP__
#define __GURUEJECUTAR_HPP__

#include <GuruCompat.h>
#include <GuruError.hpp>

#include <string>

#if defined __GURUWIN32__
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef _WINSOCKAPI_
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
	#endif

	#include <windows.h>
#elif defined __GURULINUX__
#endif

namespace GuruEjecutar {

	class opciones {
	public:
		opciones() {
			_esperar = false;
			_codigo_salida = 0;

			ptr_entrada = NULL;
			ptr_salida = NULL;
			ptr_error = NULL;

			h_hijo_salida_r = NULL;
			h_hijo_error_r = NULL;
			hdl_proceso = NULL;
		};

		virtual ~opciones() {};

		inline void esperar(bool espera) { _esperar = espera; }
		inline bool esperar() { return _esperar; }

		inline void msg(const std::string& mensaje) { _msg = mensaje; }
		inline std::string& msg() { return _msg; }

		inline void codigo(unsigned long codigo_salida) { _codigo_salida = codigo_salida; }
		inline unsigned long codigo() { return _codigo_salida; }

		inline void directorio(const std::string& dir) { _directorio = dir; }
		inline std::string& directorio() { return _directorio; }

		inline void entrada_stream(istream& entrada) { this->ptr_entrada = static_cast<istream*>(&entrada); };
		inline istream* entrada_stream() { return this->ptr_entrada; };

		inline void salida_stream(ostream& salida) { this->ptr_salida = static_cast<ostream*>(&salida); };
		inline ostream* salida_stream() { return this->ptr_salida; };

		inline void error_stream(ostream& salida) { this->ptr_salida = static_cast<ostream*>(&salida); };
		inline ostream* error_stream() { return this->ptr_error; };

		inline void entrada_archivo(const string& nombre) { _entrada = nombre; }
		inline string& entrada_archivo() { return _entrada; }

		inline void salida_archivo(const string& nombre) { _salida = nombre; }
		inline string& salida_archivo() { return _salida; }

		inline void error_archivo(const string& nombre) { _error = nombre; }
		inline string& error_archivo() { return _error; }

		inline void entrada_buf(string& buf) { _buf_entrada = buf; }
		inline string& entrada_buf() { return _buf_entrada; }

		inline void salida_buf(string& buf) { _buf_salida = buf; }
		inline string& salida_buf() { return _buf_salida; }

		inline void error_buf(string& buf) { _buf_error = buf; }
		inline string& error_buf() { return _buf_error; }

		inline void handle_salida(HANDLE& hdl) { h_hijo_salida_r = hdl; }
		inline HANDLE handle_salida() { return h_hijo_salida_r; }

		inline void handle_error(HANDLE& hdl) { h_hijo_error_r = hdl; }
		inline HANDLE handle_error() { return h_hijo_error_r; }

		inline void handle_entrada(HANDLE& hdl) { h_hijo_entrada_w = hdl; }
		inline HANDLE handle_entrada() { return h_hijo_entrada_w; }

		inline void handle_proceso(HANDLE& hdl) { hdl_proceso = hdl; }
		inline HANDLE handle_proceso() { return hdl_proceso; }

	private:
		bool			_esperar;
		std::string		_msg;
		unsigned long	_codigo_salida;
		std::string		_directorio;

		std::string		_entrada;
		std::string		_salida;
		std::string		_error;

		std::string		_buf_entrada;
		std::string		_buf_salida;
		std::string		_buf_error;

		istream*		ptr_entrada;
		ostream*		ptr_salida;
		ostream*		ptr_error;

		HANDLE			h_hijo_salida_r;
		HANDLE			h_hijo_error_r;
		HANDLE			h_hijo_entrada_w;

		HANDLE			hdl_proceso;
	};

#if defined __GURUWIN32__

	static HANDLE crear_archivo_salida(const string& archivo) {
		SECURITY_ATTRIBUTES sa;

		ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));

		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		HANDLE hdl;

		if ((hdl = CreateFile(archivo.c_str(), FILE_APPEND_DATA, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
			std::string a(GuruError().descripcion());

		return hdl;
	}

	// retorna el handle de escritura
	static HANDLE crear_pipe_salida(HANDLE& hdl_lectura) {
		SECURITY_ATTRIBUTES sa;

		ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));

		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		HANDLE			h_hijo_salida_w;

		// Create a pipe for the child process's. 
		CreatePipe(&hdl_lectura, &h_hijo_salida_w, &sa, 0);

		// Ensure the read handle to the pipe for STDOUT is not inherited
		SetHandleInformation(hdl_lectura, HANDLE_FLAG_INHERIT, 0);

		return h_hijo_salida_w;
	}

	static void leer_pipe_original(GuruEjecutar::opciones& opt) {
		DWORD dwRead;
		CHAR chBuf[2050];
		BOOL bSuccess = FALSE;
		ostream* salida = opt.salida_stream();

		for (;;) {
			bSuccess = ReadFile(opt.handle_salida(), chBuf, 2048, &dwRead, NULL);

			if (!bSuccess || dwRead == 0) break;

			chBuf[dwRead] = '\0';

			*salida << chBuf;
		}
	}


	/* 
	// http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix/35658917#35658917
	bool bProcessEnded = false;
	for (; !bProcessEnded ;) {
		// Give some timeslice (50ms), so we won't waste 100% cpu.
		bProcessEnded = WaitForSingleObject( pi.hProcess, 50) == WAIT_OBJECT_0;

		// Even if process exited - we continue reading, if there is some data available over pipe.
		for (;;) {
			char buf[1024];
			DWORD dwRead = 0;
			DWORD dwAvail = 0;

			if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL)) break;

			if (!dwAvail) // no data available, return
				break;

			if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
				// error, the child process might ended
				break;

			buf[dwRead] = 0;
			strResult += buf;
		}
	} //for
	*/
	// info: https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/48f47377-ada2-4ee0-b1c9-c61c7d8c9f8f/peeknamedpipe-on-anonymous-pipes-always-returns-zero-bytes-available?forum=windowssdk
	static void leer_pipe(GuruEjecutar::opciones& opt) {

		DWORD dwRead=100;
		BOOL bSuccess = FALSE;
		CHAR chBuf[4096];
		ostream* salida = opt.salida_stream();

		// Con este comando la funcion PeekNamedPipe retorna algun valor.
		WaitForSingleObject(opt.handle_proceso(), 50);

		do {
			bSuccess = PeekNamedPipe(opt.handle_salida(), NULL, 0, NULL, &dwRead, NULL);

			if (!bSuccess || dwRead == 0) {
				if (WaitForSingleObject(opt.handle_proceso(), 50) == WAIT_OBJECT_0) {

					// Se termino el proceso pero quedo data en el pipe.
					bSuccess = PeekNamedPipe(opt.handle_salida(), NULL, 0, NULL, &dwRead, NULL);

					if (!dwRead) break;
				}
			}

			if (dwRead) {
				bSuccess = ReadFile(opt.handle_salida(), chBuf, 4090, &dwRead, NULL);

				if (!bSuccess || dwRead == 0) break;

				chBuf[dwRead] = '\0';

				*salida << chBuf;
			}

		} while (8);
	}

	static void leer_pipe_prueba(GuruEjecutar::opciones& opt) {

		DWORD dwRead = 100;
		BOOL bSuccess = FALSE;
		CHAR chBuf[2050];
		ostream* salida = opt.salida_stream();

		bSuccess = SetNamedPipeHandleState(opt.handle_salida(), NULL, NULL, &dwRead);

		if (!bSuccess)
			string a(GuruError().descripcion());

		do {

			bSuccess = ReadFile(opt.handle_salida(), chBuf, 2048, &dwRead, NULL);

			// hay datos para sacar del pipe?
			if (!bSuccess || dwRead == 0) {// nop
				if (GetExitCodeProcess(opt.handle_proceso(), &dwRead) != STILL_ACTIVE) break;
				Sleep(100);
			}

			chBuf[dwRead] = '\0';

			*salida << chBuf;

		} while (8);
	}

	// Buen ejemplo:
	// https://msdn.microsoft.com/en-us/library/ms682499(VS.85).aspx
	// Para guardar la salida en una cadena.
	// http://stackoverflow.com/questions/14147138/capture-output-of-spawned-process-to-string
	// Para guardar la salida en un archivo.
	// http://stackoverflow.com/questions/7018228/how-do-i-redirect-output-to-a-file-with-createprocess
	static bool ejecutar(const std::string& comando, opciones& opt) {
		BOOL				salida_create;
		STARTUPINFO			si;
		PROCESS_INFORMATION	pi;

		memset(&si, 0, sizeof(STARTUPINFO));
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));

		// redireccion de la salida a un archivo
		if (!opt.salida_archivo().empty()) { si.hStdOutput = crear_archivo_salida(opt.salida_archivo()); si.dwFlags |= STARTF_USESTDHANDLES; }
		if (!opt.error_archivo().empty())  { si.hStdError = crear_archivo_salida(opt.error_archivo()); si.dwFlags |= STARTF_USESTDHANDLES; }
//		else if (opt.salida_archivo().empty()) { si.hStdError = crear_archivo_salida(opt.error_archivo()); si.dwFlags |= STARTF_USESTDHANDLES; }

		if (opt.salida_stream() != NULL) { HANDLE hdl; si.hStdOutput = crear_pipe_salida(hdl); opt.handle_salida(hdl); si.dwFlags |= STARTF_USESTDHANDLES; }
		if (opt.error_stream() != NULL) { HANDLE hdl; si.hStdError = crear_pipe_salida(hdl); opt.handle_error(hdl); si.dwFlags |= STARTF_USESTDHANDLES; }
//		else if (opt.salida_stream()) { HANDLE hdl; si.hStdError = crear_pipe_salida(hdl); opt.handle_error(hdl); si.dwFlags |= STARTF_USESTDHANDLES; }
		if (opt.entrada_stream() != NULL) { HANDLE hdl; si.hStdInput = crear_pipe_salida(hdl); opt.handle_entrada(hdl); si.dwFlags |= STARTF_USESTDHANDLES; }

		si.cb = sizeof(STARTUPINFO);

		si.dwFlags |= STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWDEFAULT;

		SECURITY_ATTRIBUTES	sa;

		sa.bInheritHandle = TRUE; // TRUE para redireccionar la salida.
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;

		BOOL heredar_handles = TRUE;

		if (!opt.esperar()) heredar_handles = FALSE;

		salida_create = CreateProcess(NULL,			// name of executable module
								(char*)comando.c_str(),		// command line string
								&sa,		// SD
								&sa,		// SD
								heredar_handles,		// handle inheritance option
								NORMAL_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE,	// creation flags
								NULL,		// new environment block
								((opt.directorio().empty()) ? NULL : opt.directorio().c_str()),		// current directory name
								&si,		// startup information
								&pi			// process information
						);

		if (salida_create) {
			DWORD salida_apps;

			GetExitCodeProcess(pi.hProcess, &salida_apps);

			opt.codigo(salida_apps);
			opt.handle_proceso(pi.hProcess);

			if (opt.esperar()) { 

				if (opt.salida_stream() != NULL) 
					leer_pipe(opt);
				else 
					WaitForSingleObject(pi.hProcess, INFINITE); 
			}

			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );

			if (!opt.salida_archivo().empty()) CloseHandle(si.hStdOutput);
			if (!opt.error_archivo().empty()) CloseHandle(si.hStdError);
			if (!opt.entrada_archivo().empty()) CloseHandle(si.hStdInput);

			return true;
		} else {
			opt.msg(GuruError().descripcion());

			if (!opt.salida_archivo().empty()) CloseHandle(si.hStdOutput);
			if (!opt.error_archivo().empty()) CloseHandle(si.hStdError);

			return false;
		}

	}
#elif defined __GURULINUX__ 
	// http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/exec.html
	// man fork:
	// RETURN VALUE:
	//	On success, the PID of the child process is returned in the parent,
	//	and 0 is returned in the child.  On failure, -1 is returned in the
	//	parent, no child process is created, and errno is set appropriately.
	//
	// http://www.reloco.com.ar/linux/prog/pipes.html
	static void  execute(char **argv)
	{
		 pid_t  pid;
		 int    status;

		 if ((pid = fork()) < 0) {     /* fork a child process           */
			  printf("*** ERROR: forking child process failed\n");
			  exit(1);
		 }
		 else if (pid == 0) {          /* for the child process:         */
			  if (execvp(*argv, argv) < 0) {     /* execute the command  */
				   printf("*** ERROR: exec failed\n");
				   exit(1);
			  }
		 }
		 else {                                  /* for the parent:      */
			  while (wait(&status) != pid)       /* wait for completion  */
				   ;
		 }
	}
#endif

	// desde 
	// http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
	// https://www.microsoft.com/resources/documentation/windows/xp/all/proddocs/en-us/redirection.mspx?mfr=true
	// Para redirigir a un archivo:
	// http://stackoverflow.com/questions/2605130/redirecting-exec-output-to-a-buffer-or-file
	std::string exec(const char* cmd) {
		char buffer[128];
		std::string result = "";
		FILE* pipe = _popen(cmd, "r");

		if (!pipe) throw std::runtime_error("popen() failed!");

		try {
			while (!feof(pipe)) {
				if (fgets(buffer, 128, pipe) != NULL)
					result += buffer;
			}
		} catch (...) {
			_pclose(pipe);
			throw;
		}
		_pclose(pipe);
		return result;
	}
};

#endif

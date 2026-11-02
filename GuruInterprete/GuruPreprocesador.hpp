#ifndef __GURUPREPROCESADOR_HPP__
#define __GURUPREPROCESADOR_HPP__

#include <GuruMarca.hpp>
#include <GuruError.hpp>
#include <GuruInterprete/GuruInterprete.hpp>

#include <fstream>

using namespace std;

/*
<label class="description" for="element_3">Resultado: </label>
<div>
	<?Guru if (parametro("instrucciones")) {
		mostrar(parametro("instruccines"))
		?>
		Con parametros
	<?Guru } else {?>
		Sin Parametros
	<?Guru }?>
</div> 
*/

class ControlFin_instruccion:public GuruInterprete::IGuruControl {
public:
	ControlFin_instruccion(const string& inicio, const string& fin):inicio(inicio), fin(fin) {}
	virtual ~ControlFin_instruccion() {}

protected:
	virtual void ejecutar(istream& entrada, ostream& salida, GuruInterprete::estado& actual_estado) {
		std::ios::pos_type pos_inicio=0,pos_fin=0;
		int32_t total=0;

		// NO me gusta, esto es horrible!, si no tiene cantidad para analizar quiere decir que todavia no empezo a analizar.
		if (actual_estado.cantidad_analizar)
			GuruBuscaMarca::ir_hasta_marca(entrada, salida, fin, static_cast<enum GuruBuscaMarca::opciones>(GuruBuscaMarca::despues | !GuruBuscaMarca::salida));

		enum GuruBuscaMarca::opciones opt;
		if (actual_estado.control_interprete & GuruInterprete::mostrar)
			opt=static_cast<enum GuruBuscaMarca::opciones>(GuruBuscaMarca::antes | GuruBuscaMarca::salida);
		else
			opt=static_cast<enum GuruBuscaMarca::opciones>(GuruBuscaMarca::antes | !GuruBuscaMarca::salida);

		if (GuruBuscaMarca::ir_hasta_marca(entrada, salida, inicio, opt)) {
			pos_inicio=entrada.tellg();

			if (GuruBuscaMarca::ir_hasta_marca(entrada, salida, fin, static_cast<enum GuruBuscaMarca::opciones>(GuruBuscaMarca::despues | !GuruBuscaMarca::salida))) {
				pos_fin=entrada.tellg();

				total=(static_cast<int32_t>(pos_fin)-static_cast<int32_t>(pos_inicio))-static_cast<int32_t>(inicio.length())-static_cast<int32_t>(fin.length());
				entrada.seekg(static_cast<std::ios::off_type>(static_cast<int32_t>(0)-static_cast<int32_t>(total+fin.length())), std::ios_base::cur);

				actual_estado.cantidad_analizar=total;
				actual_estado.cantidad_analizada=0;
				actual_estado.posicion=entrada.tellg();
			} 
		} else {
				actual_estado.cantidad_analizada=0;
				actual_estado.posicion=entrada.tellg();
		}

	}

private:
	string inicio;
	string fin;
};

class GuruPreprocesador {
public:
	GuruPreprocesador(){};
	virtual ~GuruPreprocesador() {};
/*
	void abrir_archivos(const string& archivo_entrada, const string& archivo_salida) {

		entrada.open(archivo_entrada.c_str(), ios_base::in | ios_base::binary);
		if (!entrada.is_open()) throw GuruError().cargar_descripcion("Error al abrir archivo de entrada.");

		salida.open(archivo_salida.c_str(), ios_base::out | ios_base::binary);
		if (!salida.is_open()) {
			entrada.close();
			throw GuruError().cargar_descripcion("Error al abrir archivo de salida.");
		}
	};

	void procesar(const string& archivo_entrada, const string& archivo_salida, const string& inicio, const string& fin) {

		abrir_archivos(archivo_entrada, archivo_salida);

		GuruLenguaje lang;
		lang.agregar_control(GuruInterprete::fin_analisis, new ControlFin_instruccion(inicio, fin));

		lang.interpretar(entrada, salida, 0);
	};
*/
	void procesar(istream& entrada, ostream& salida, GuruInterprete::Interprete& lenguaje, const string& inicio, const string& fin) {

		lenguaje.agregar_control(GuruInterprete::fin_analisis, new ControlFin_instruccion(inicio, fin));

		lenguaje.interpretar(entrada, salida, 0);
	}

protected:
//	ifstream entrada;
//	ofstream salida;
};

#endif

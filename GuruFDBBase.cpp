#include "GuruFDBBase.h"

iterator_fdb::iterator_fdb(GuruFDBBase& base, const string& consulta):base(base)
{
	if (consulta.empty()) return ;

	bd = base.conexion();
	tr = IBPP::TransactionFactory(bd);
	st = IBPP::StatementFactory(bd, tr);

	tr->Start();

	try{
		st->Execute(consulta);
	}catch (IBPP::Exception& e)	{ 
		cerrar_iteracion();
		throw string(e.what());
	}

	if (st->Type() != IBPP::stSelect) { 
		// tengo que ver que pasa con: 
		// stInsert, stUpdate, stDelete, stDDL, stExecProcedure, stSelectUpdate
		cerrar_iteracion();
		return ;
	}

	try {
		sacar_registro();
	} catch (IBPP::LogicException& e) {
		string tmp(e.what());
		cerrar_iteracion();	
	};
}

void iterator_fdb::sacar_registro()
{
	registro.resultado.clear();

	if (st->Fetch()) {
		int cant_columnas = st->Columns();
		for (int i=1;i<=cant_columnas;i++) {
			ostringstream oss;
			FDBColumna it;
			switch (st->ColumnType(i)) {
			case IBPP::sdString:{
								string columna;
								st->Get(i, columna);
								oss << columna;
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdString;
								break;
								}
			case IBPP::sdSmallint:{
								int16_t columna;
								st->Get(i, columna);
								oss << columna;
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdSmallint;
								break;
								}
			case IBPP::sdInteger:{
								int32_t columna;
								st->Get(i, columna);
								oss << columna;
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdInteger;
								break;
								}
			case IBPP::sdLargeint:{
								int64_t columna;
								st->Get(i, columna);
								oss << columna;
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdLargeint;
								break;
								}
			case IBPP::sdFloat:{
								float columna;
								st->Get(i, columna);
								oss << columna;
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdFloat;
								break;
								}
			case IBPP::sdDouble:{
								double columna;
								st->Get(i, columna);
								oss << columna;
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdDouble;
								break;
								}
			case IBPP::sdTimestamp:{
								IBPP::Timestamp columna;
								st->Get(i, columna);
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdTimestamp;

								/* 
								struct tm fechahora={0};
								fechahora.tm_hour = columna.Hours();
								fechahora.tm_min = columna.Minutes();
								fechahora.tm_sec = columna.Seconds();

								fechahora.tm_mday = columna.Day();
								fechahora.tm_mon = columna.Month() - 1;
								fechahora.tm_year = columna.Year() - 1900;
								oss << fechaHoraRFC822(mktime(&fechahora)); // hasta el 25/06/2013
								*/

								oss << FDBFecha(columna) << " " << FDBHora(columna);
								
								break;
								}
			case IBPP::sdDate:{
								IBPP::Date columna;
								st->Get(i, columna);
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdDate;

								oss << FDBFecha(columna);
								break;
								}
			case IBPP::sdTime:{
								IBPP::Time columna;
								st->Get(i, columna);
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdTime;

								oss << FDBHora(columna);
								break;
								}
			case IBPP::sdBlob:{
								IBPP::Blob columna;
								st->Get(i, columna);
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdBlob;

								// oss << columna;
								break;
								}
			case IBPP::sdArray:{
								IBPP::Array columna;
								st->Get(i, columna);
								it.dato_fdb=columna;
								it.tipo_fdb=IBPP::sdArray;

								// oss << columna;
								break;
								}
			default: break;
			}
			it._str = oss.str();
			it.nombre_columna = st->ColumnAlias(i);
			registro.resultado.push_back(it);
		}
	} else { 
		cerrar_iteracion();
	}
}

void iterator_fdb::cerrar_iteracion()
{
	try {
		tr->Commit();
		st->Close();
		st=NULL;
	} catch (IBPP::LogicException&) {
	} catch (IBPP::SQLException&) {}
}

GuruFDBBase::~GuruFDBBase(void)
{
	if (bd == NULL) return;

	try{
		if (bd->Connected()) desconectar();
	}catch (IBPP::Exception& e)	{
		cerr << e.what() << endl;
	}
}

void GuruFDBBase::crear_bdd(const char* ddl[])
{
	try{
		try{
			conectar();
			bd->Drop();
		}catch (IBPP::Exception& e) { cerr << e.what() << endl;};

		if (bd == NULL) return ;

		bd->Create(3);

		bd->Connect();

		__INIT_STATEMENT_START__

		if (ddl){
			int i=0;
			while (ddl[i])
				st->Execute(ddl[i++]);
		}

		tr->Commit();
		desconectar();
	}catch (IBPP::Exception& e)	{
	//	throw string(e.what());
		cerr << e.what() << endl;
	};
}

void GuruFDBBase::crear_bdd(const string& ddl)
{
	try{
		try{
			conectar();
			bd->Drop();
		}catch (IBPP::Exception& e) {cerr << e.what() << endl;};

		bd->Create(3);

		bd->Connect();

		__INIT_STATEMENT_START__

		st->ExecuteImmediate(ddl);

		tr->Commit();
		desconectar();
	}catch (IBPP::Exception& e)	{
		cerr << e.what() << endl;
		throw string(e.what());
	};
}

// Ejecuta una instruccion sql que no produce ningun registro de salida.
bool GuruFDBBase::ejecutar(const string& consulta)
{
	try{
		IBPP::Transaction tr = IBPP::TransactionFactory(conexion());
		tr->Start();

		IBPP::Statement st = IBPP::StatementFactory(bd, tr);
		st->Execute(consulta);
		tr->Commit();
	}catch (IBPP::Exception& e)	{
		cout << e.what() << endl;
		return false;
	};

	return true;
}


void GuruFDBBase::borrar_bdd()
{
	bd->Drop();
}

void GuruFDBBase::conectar(void)
{
	bd = IBPP::DatabaseFactory(servidor_bdd, archivo_bdd, usuario_bdd, clave_bdd);
	bd->Connect();
}

void GuruFDBBase::desconectar()
{
	bd->Disconnect();

	bd = NULL;
}

IBPP::Database& GuruFDBBase::conexion()
{
	try{
		if (bd == NULL) conectar();

		if (!bd->Connected()) conectar();
	}catch (IBPP::Exception& e)	{
		cerr << e.what() << endl;
		// throw string(e.what());
		throw ;
	}

	return bd;
}

IBPP::Statement GuruFDBBase::statement(IBPP::Statement& st)
{
	st = IBPP::StatementFactory(bd, IBPP::TransactionFactory(conexion()));
	return st;
}

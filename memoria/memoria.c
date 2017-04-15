#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/bitarray.h>
#include <commons/temporal.h>
#include <commons/process.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>

#define RUTA_ARCHIVO "./config_memoria.cfg"

t_config* llamarArchivo (){

	//Dirección para manejar el archivo de configuración.
	char* directorio;

	//Asignar ruta de acceso al archivo de configuración.
	directorio = RUTA_ARCHIVO ;
	return config_create(directorio);
}

int busquedaClaveNumerica (t_config* configuracion, char* palabraClave)
{
	int clave;
	if(config_has_property(configuracion, palabraClave))
		clave = config_get_int_value(configuracion, palabraClave);
	else
		perror("No se encontró.");
	return clave;
}

int main(int argc, char *argv[])
{
	t_config* configuracion;
	configuracion = llamarArchivo();

	int PUERTO = busquedaClaveNumerica(configuracion, "PUERTO");
	int MARCOS = busquedaClaveNumerica(configuracion, "MARCOS");
	int MARCO_SIZE = busquedaClaveNumerica(configuracion, "MARCO_SIZE");
	int ENTRADAS_CACHE = busquedaClaveNumerica(configuracion, "ENTRADAS_CACHE");
	int CACHE_X_PROC = busquedaClaveNumerica(configuracion, "CACHE_X_PROC");
	int RETARDO_MEMORIA = busquedaClaveNumerica(configuracion, "RETARDO_MEMORIA");

	printf("%d\n%d\n%d\n%d\n%d\n%d\n", PUERTO, MARCOS, MARCO_SIZE, ENTRADAS_CACHE, CACHE_X_PROC, RETARDO_MEMORIA);

	return EXIT_SUCCESS;
}
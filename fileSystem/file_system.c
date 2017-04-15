/*
 ============================================================================
 Name        : file_system.c
 Author      : Zero Gravity
 ============================================================================
 */


#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

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

#define RUTA_ARCHIVO "./config_file_system.cfg"

t_config* llamarArchivo() {

	//Dirección para manejar el archivo de configuración.
	char* directorio;

	//Asignar ruta de acceso al archivo de configuración.
	directorio = RUTA_ARCHIVO;
	return config_create(directorio);
}

int busquedaClaveNumerica(t_config* configuracion, char* palabraClave) {
	int clave;
	if (config_has_property(configuracion, palabraClave))
		clave = config_get_int_value(configuracion, palabraClave);
	else
		perror("No se encontró.");
	return clave;
}

char* busquedaClaveAlfanumerica(t_config* configuracion, char* palabraClave) {
	char* clave;
	if (config_has_property(configuracion, palabraClave))
		clave = config_get_string_value(configuracion, palabraClave);
	else
		perror("No se encontró.");
	return clave;
}

int main(int argc, char *argv[]) {

	//Estructura para manejar el archivo de configuración -- t_config*
	//Crear estructura de configuración para obtener los datos del archivo de configuración.
	t_config* configuracion;
	configuracion = llamarArchivo();

	int PUERTO = busquedaClaveNumerica(configuracion, "PUERTO");
	char* PUNTO_MONTAJE = busquedaClaveAlfanumerica(configuracion,"PUNTO_MONTAJE");

	/**********************************************************
	int sockDeEspera; //Socket para esperar conexion con el kernel
		int longitudDatosEnviados;
		int sockAlKernel;//Socket para entablar conexion con el kernel
		int yes = 1;
		char buffer[1024];
		memset (buffer,'\0',1024);
		char* handshake = "La conexion al proceso fileSystem fue exitosa";
		struct sockaddr_in espera,datosDelKernel;


		sockDeEspera = socket(AF_INET,SOCK_STREAM,0);
		if (sockDeEspera == -1)
		{
			perror("Fallo en la creacion del socket de espera");
			exit(1);
		}

		if (setsockopt(sockDeEspera, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
		perror("Error en setsockopt en el socket de espera");
		exit(1);
		}

		printf("Setsockopt correcto\n");
		puts("Socket de espera creado\n");

		//Direccion para esperar la conexion con el kernel
		espera.sin_family = AF_INET;
		espera.sin_port = htons(PUERTO);
		espera.sin_addr.s_addr = INADDR_ANY;
		bzero(&(espera.sin_zero), 8);

		if (bind(sockDeEspera, (struct sockaddr *) &espera,sizeof(struct sockaddr)) == -1)
		{
			perror("Error en Bind");
			exit(1);
		}

		printf("Bind correcto\n");
		puts("Esperando la conexion del kernel\n");

		if (listen(sockDeEspera, 1) == -1)
		{
			perror("Error en listen");
			exit(1);
		}
	    int longitudEstructuraSocket= sizeof (datosDelKernel);
		if ((sockAlKernel = accept(sockDeEspera,(struct sockaddr *) &datosDelKernel,&longitudEstructuraSocket)) == -1)

		{
		 perror("Error en accept");
		}
		close(sockDeEspera);

		if((longitudDatosEnviados = send(sockAlKernel,handshake, strlen(handshake),0)) == -1)
		{
		 perror ("Fallo en el handshake");
		}
		puts ("La conexion al proceso kernel fue exitosa\n");
		puts ("Esperando mensajes\n");

		while(1)
		{
		int bytesRecibidos = recv(sockAlKernel,buffer,sizeof(buffer),0);
		if(bytesRecibidos == -1)
		{
		  perror ("Error en la recepcion");
		}
		if(bytesRecibidos == 0)
		{
		  perror("El kernel se ha desconectado");
		  exit(1);
		}
		printf ("Buffer: %s\n",buffer);
	    memset (buffer,'\0',1024);
		}

		*****************************************************************************/

	return 0;
}
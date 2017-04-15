/*
 ============================================================================
 Name        : consola.c
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


#define RUTA_ARCHIVO "./config_consola.cfg"

// llamarArchivo(): Genera una estructura de datos de configuración.
t_config* llamarArchivo() {

	//Dirección para manejar el archivo de configuración.
	char* directorio;

	//Asignar ruta de acceso al archivo de configuración.
	directorio = RUTA_ARCHIVO;

	return config_create(directorio);
}

// busquedaClaveNumerica(): Devuelve un dato numerico en función de una
// palabra clave que se encuentre en el archivo de configuración.
int busquedaClaveNumerica(t_config* configuracion, char* palabraClave) {
	int clave;
	if (config_has_property(configuracion, palabraClave))
		clave = config_get_int_value(configuracion, palabraClave);
	else
		perror("No se encontró");
	return clave;
}

// busquedaClaveAlfanumerica(): Devuelve un dato alfanumerico en función de una
// palabra clave que se encuentre en el archivo de configuración.
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

	//Obtener IP del Kernel del archivo de configuración y chequear que sea correcto.
	char* IP_KERNEL = busquedaClaveAlfanumerica(configuracion, "IP_KERNEL");

	//Obtener el puerto de Kernel del archivo de configuración y chequear que sea correcto.
	int PUERTO_KERNEL = busquedaClaveNumerica(configuracion, "PUERTO_KERNEL");


	 int sock, bytesRecibidos;
	        char datosEnviar[1024],datosRecibir[1024];
	        memset (datosEnviar,'\0',1024);
	        memset (datosRecibir,'\0',1024);
	       // struct hostent *host;
	        struct sockaddr_in kernel_dir;

			// IP_KERNEL
	       // host = gethostbyname(argv[1]);

	        //Se crea el socket para conectarse con el kernel
	        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	        {
	            perror("Socket");
	            exit(1);
	        }

	        //Direccion del servidor(kernel)
	        kernel_dir.sin_family = AF_INET;
	        //PUERTO_KERNEL
	        kernel_dir.sin_port = htons(PUERTO_KERNEL);
	        kernel_dir.sin_addr.s_addr = inet_addr(IP_KERNEL);
	        //*((struct in_addr *)host->h_addr);
	        bzero(&(kernel_dir.sin_zero),8);

	        //Conexion al kernel
	        if (connect(sock, (struct sockaddr *)&kernel_dir,sizeof(struct sockaddr)) == -1)
	        {
	            perror("Connect");
	            exit(1);
	        }

	        //El cliente espera un mensaje de parte del kernel (handshake)
	        bytesRecibidos=recv(sock,datosRecibir,1024,0);
	        datosRecibir[bytesRecibidos] = '\0';

	        while(1)
	        {
	           printf("\nDatos a enviar: ");
	           gets(datosEnviar);
	           send(sock,datosEnviar,strlen(datosEnviar), 0);

	               memset (datosEnviar,'\0',1024);
	        }



	        return 0;
}

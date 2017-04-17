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

#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"

#define RUTA_ARCHIVO "./config_consola.cfg"
#define SIZE_DATA 1024

int main(int argc, char *argv[]) {

//CODIGO PARA LLAMAR AL ARCHIVO

	//Estructura para manejar el archivo de configuración -- t_config*
	//Crear estructura de configuración para obtener los datos del archivo de configuración.

	t_config* configuracion;
	char* ruta = RUTA_ARCHIVO;
	configuracion = llamarArchivo(ruta);

//DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL

	int sock, bytesRecibidos;
	char datosEnviar[SIZE_DATA],datosRecibir[SIZE_DATA];
	memset (datosEnviar,'\0',SIZE_DATA);
	memset (datosRecibir,'\0',SIZE_DATA);
	struct sockaddr_in kernel_dir;

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION

	//*Obtener IP del Kernel del archivo de configuración y chequear que sea correcto.
	char* IP_KERNEL = busquedaClaveAlfanumerica(configuracion, "IP_KERNEL");

	//*Obtener el puerto de Kernel del archivo de configuración y chequear que sea correcto.
	int PUERTO_KERNEL = busquedaClaveNumerica(configuracion, "PUERTO_KERNEL");

//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

		int valorRtaConnect = 0;

// CODIGO PRINCIPAL DE LA CONSOLA

	       // host = gethostbyname(argv[1]);

	        //*Se crea el socket para conectarse con el kernel

			sock = socket(AF_INET, SOCK_STREAM, 0);
			esErrorConSalida(sock,"Error en el Socket");


	        //ASIGNACION DE DATOS DEL KERNEL

	        //*Direccion del servidor - KERNEL
	        kernel_dir.sin_family = AF_INET;
	        kernel_dir.sin_port = htons(PUERTO_KERNEL);
	        kernel_dir.sin_addr.s_addr = inet_addr(IP_KERNEL);
	        bzero(&(kernel_dir.sin_zero),8);

	        //CONEXION A KERNEL

	        valorRtaConnect = connect(sock, (struct sockaddr *)&kernel_dir,sizeof(struct sockaddr));
	        esErrorConSalida(valorRtaConnect,"Error en el connect");


	        //*El cliente espera un mensaje de parte del kernel (handshake)

	        bytesRecibidos=recv(sock,datosRecibir,SIZE_DATA,0);

	        datosRecibir[bytesRecibidos] = '\0';

	        while(1)
	        {
	           printf("\nDatos a enviar: ");
	           scanf("s%",datosEnviar);
	           send(sock,datosEnviar,strlen(datosEnviar), 0);

	               memset (datosEnviar,'\0',1024);
	        }



	        return 0;
}

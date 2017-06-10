/*
 * funcionesMemoria.h
 *
 *  Created on: 24/04/2017
 *      Author: Zero_Gravity
 */

#ifndef FUNCIONESMEMORIA_H_
#define FUNCIONESMEMORIA_H_

#include <netinet/in.h>
#include <commons/log.h>
#include <string.h>

#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/serializador.h"

#define RUTA_ARCHIVO "./config_memoria.cfg"
#define LIBRE -7
#define OCUPADO_x_STR -14
#define NOT_FOUND -28


//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION
int PUERTO_MEMORIA;
int FRAME_SIZE;
int FRAMES;
int ENTRADAS_CACHE;
int CACHE_X_PROC;
int RETARDO_MEMORIA;

t_log *archivoLog,*archivoSP;


typedef struct {
	int pid;
	int nroPagina;
	int frame;
} strAdministrativa;

typedef struct {
	int pid;
	int nroPagina;
	void *contenido;
} strCache;

//  ******* DECLARACION DE FUNCIONES  *******
//estoy podrida de los warnings

void asignarDatosDeConfiguracion();
int* reservarMemoria();
void  crearArchivo(char* ruta);
void inicializarStrAdm(int* bloqueMemoria);
void imprimirStrAdm(int* bloqueMemoria);
int strLibre(int* ptr);
void liberarStrAdm(int pid, int* ptr);
int iniciarStrAdmDePrc(int pid, int frame, int* ptr);

int asignacionDePaginas(int cantPaginas,int pid,int *ptr);
int paginasPrc(int pid, int*ptr);
int compararPID(strAdministrativa aux,int pid);
int paginasLibresEnMemoria(int *ptr);
int strVacia(int *ptr);

int getSizeStrAdm();

// ******* INICIO DE CODIGO  *******

void asignarDatosDeConfiguracion() {
	t_config* configuracion;
	char* ruta = RUTA_ARCHIVO;
	configuracion = llamarArchivo(ruta);

	PUERTO_MEMORIA = busquedaClaveNumerica(configuracion, "PUERTO");
	FRAME_SIZE = busquedaClaveNumerica(configuracion, "MARCO_SIZE");
	FRAMES = busquedaClaveNumerica(configuracion, "MARCOS");
	ENTRADAS_CACHE = busquedaClaveNumerica(configuracion, "ENTRADAS_CACHE");
	CACHE_X_PROC = busquedaClaveNumerica(configuracion, "CACHE_X_PROC");
	RETARDO_MEMORIA = busquedaClaveNumerica(configuracion, "RETARDO_MEMORIA");
}

//Reservo la memoria que necesito
int* reservarMemoria() {
	int* memoria;
	memoria = malloc(FRAME_SIZE * FRAMES);
	if (memoria == NULL)
		esErrorConSalida(-1,
				"Error en la solicitud de bloque de memoria (Malloc)");
	return memoria;
}

//  ******* FUNCIONES DE ARCHIVO LOG  *******

void crearArchivo(char* ruta) {

	archivoLog= log_create(ruta, "Memoria", true, LOG_LEVEL_INFO);
	archivoSP= log_create(ruta, "Memoria", false, LOG_LEVEL_INFO);
}

t_log* getArchivoLog(){
	return archivoLog;
}

t_log* getArchivoSP(){
	return archivoSP;
}

//  ******* FUNCIONES DE CONTROL DE STR ADMINISTRATIVA  *******

//Seteo todas las estructuras con el valor predefinido PID=-14 de esta manera puedo saber que el frame no tiene un proceso asignado
void inicializarStrAdm(int* bloqueMemoria) {

	int paginaOcupada=0;
	int sizeStrAdm;
	int sizeBlockAdm;
	int contador_frame = 0; // contador para los frames dentro del for
	strAdministrativa auxiliar;

	//El tamaño de la estructura por la cantidad de FRAMES 500*256B = 6000 B
	//Si hacemos 6000/FRAMES nos da 12, por lo cual le sumamos dos marcos 6.512
	// y le restamos 12 para que al dividirlo nos de 13 marcos
	sizeStrAdm = sizeof(strAdministrativa);
	sizeBlockAdm = ((sizeStrAdm * FRAMES) + FRAME_SIZE*2-12) / FRAMES;

	auxiliar.pid = LIBRE;
	auxiliar.frame = contador_frame;
	auxiliar.nroPagina = LIBRE;

	//mientras la cantidad de frames del contador sea menor a 500 (FRAMES)
	while (contador_frame < FRAMES) {

		//Con este if nos aseguramos que hay 12 frames utilizados para las estructuras administrativas
		//de esta manera cuando guardemos info no vamos a pisar nada
		if (contador_frame >= FRAMES - sizeBlockAdm) {
			auxiliar.pid = OCUPADO_x_STR;
			auxiliar.nroPagina = paginaOcupada;
			paginaOcupada++;
		} else {
			auxiliar.pid = LIBRE;
			auxiliar.nroPagina = LIBRE;
		}
		//Esta funcion copia los datos The memcpy(DEST,SRC,BYTES) function copies n bytes from memory area src to memory area dest.
		memcpy(bloqueMemoria + contador_frame * sizeStrAdm, &auxiliar,
				sizeStrAdm);
		contador_frame++;
		auxiliar.frame = contador_frame;
	}

	log_info(archivoLog, "El bloque de memorias administrativas ocupa %d frames",
			sizeBlockAdm);
	log_info(archivoLog, "Son %d estructuras con un peso individual de %d Bytes\n*******************************************",
			FRAMES, sizeStrAdm);
	log_info(archivoLog,"");
}

//Como su nombre lo indica imprime todas las estructuras adm en pantalla
void imprimirStrAdm(int* bloqueMemoria) {
	strAdministrativa aux;
	int c = 0;
	log_info(archivoLog,"*******************************************");
	while (c < FRAMES) {
		memcpy(&aux, bloqueMemoria + c * sizeof(strAdministrativa),
				sizeof(strAdministrativa));
		//printf("\nPID: %d, NroPagina: %d, Frame: %d", aux.pid, aux.nroPagina,	aux.frame);

		log_info(archivoLog,"PID: %d, NroPagina: %d, Frame: %d\n-----------------------------------------------------------------------------------------",aux.pid,aux.nroPagina,aux.frame);
		c++;
	}
	log_info(archivoLog,"*******************************************");
}

//La estructura se encuentra libre siempre y cuando su PID sea igual a -7
int strLibre(int* ptr) {
	strAdministrativa aux;
	memcpy(&aux, ptr, sizeof(strAdministrativa));
	if (aux.pid == LIBRE)
		return 1;
	else
		return 0;

}

//Al liberar una estructura administrativa se le asigna al pid y al nro pagina el nro -7
void liberarStrAdm(int pid, int* ptr) {
	int c=0;
	strAdministrativa aux;

	//Mientras contador sea menor a la cant de frames,
	//el ptero se sigue moviendo de a una estructura adm
	while(c<FRAMES){

		//Copio los datos apuntos en la direccion de memoria a un auxiliar de estructura
		memcpy(&aux, ptr + c * sizeof(strAdministrativa),sizeof(strAdministrativa));

		//Si el pid de la estructura apuntada es el indicado, seteo los datos de aux
		//para que su pid y nro de pagina sean -7 indicador de la str libre
		//el nro de frame lo dejo intacto por que no es necesario modificarlo

		if(compararPID(aux,pid)){
		aux.pid = LIBRE;
		aux.nroPagina = LIBRE;

		//copio los nuevos datos a la direccion de memoria
		memcpy(ptr + c * sizeof(strAdministrativa), &aux,sizeof(strAdministrativa));
		}

		// sumo uno para encontrar todos las str con el pid necesario
		c++;
	}



}


//Inicia la str adm de un proceso, con un pid indicado en un determinado frame
int iniciarStrAdmDePrc(int pid, int c, int* ptr) {

	strAdministrativa aux;
	aux.pid=pid;

	//Se fija la cantidad de paginas que tiene el proceso en todo la EA (estructura Adm)
	aux.nroPagina = paginasPrc(pid, ptr);

	aux.frame = c ;
	memcpy(ptr,&aux,sizeof(strAdministrativa));

 return 1;
}


//  ******* FUNCIONES DE ASIGNACION  *******


//Esta funcion asigna las paginas una por una, busca la primera estructura vacia y le asigna los datos, asi sucesivamente hasta que llega todas las paginas requeridas
int asignacionDePaginas(int cantPaginas,int pid,int *ptr){

	int c=0,paginasAsignadas=0,iniciarPrograma;

	//1ro. Mientras las pag Asignadas sean menos que las pag que necesito se repite el ciclo
	while(paginasAsignadas<cantPaginas){

		//2do. Si el frame al que apunto esta vacio entonces asigno los datos
		if(strVacia(ptr+c*sizeof(strAdministrativa))){
			iniciarPrograma=0;

			//Se le envia a iniciar... el nro de pid, el nro de frame con el que trabajara, y el puntero al frame vacio de la strAdm,
			//devuelve un 1 en caso de que la operacion haya podido realizarse y un 0 en caso contrario
			iniciarPrograma=iniciarStrAdmDePrc(pid,c,ptr+c*sizeof(strAdministrativa));

			if(iniciarPrograma)
				paginasAsignadas++;

		}//Cierro If Frame Vacio

		//3ro. Me muevo hacia la siguiente str Adm. y vuelvo a hacer los controles 1 y 2
		c++;
	}
	return paginasAsignadas;
}


//  ******* FUNCIONES DE BUSQUEDA  *******

//Compara dos pid, el de una EA aux y el pid que le pasa
int compararPID(strAdministrativa aux,int pid){

	if(aux.pid==pid)
		return 1;
	else
		return 0;

}

//Cuenta la cantidad de paginas que tiene asignado un proceso en memoria
int paginasPrc(int pid, int*ptr){
	int paginasPrc=0,c=0;
	strAdministrativa aux;
	size_t size=sizeof(strAdministrativa);
	while(c<FRAMES){

		memcpy(&aux,ptr+c*sizeof(strAdministrativa),size);
		if(compararPID(aux,pid)){
			paginasPrc++;
		}
		c++;

	}

	return paginasPrc;
}
//Devuelve la cantidad de paginas libres que hay en toda la memoria
int paginasLibresEnMemoria(int *ptr){

		int c=0, paginasLibres=0,frame=-1;
		size_t size=sizeof(strAdministrativa);
		strAdministrativa aux;
		while(c<FRAMES){
			memcpy(&aux, ptr+c*sizeof(strAdministrativa),size);
			if(compararPID(aux,LIBRE)){
				paginasLibres++;
			}
			c++;
		}

		return paginasLibres;

}

//Controla si una estructura administrativa esta vacia, no tiene asignada ningun proceso
int strVacia(int *ptr){

	//C hace referencia a la posicion en la que se encuentra el frame vacio
	int c=0;
	int frameLibre=NOT_FOUND;
	strAdministrativa aux;
	size_t size=sizeof(strAdministrativa);
	while(c<FRAMES){
		memcpy(&aux, ptr+c*sizeof(strAdministrativa),size);
		if(compararPID(aux,LIBRE)){
			frameLibre=c;
		}
		c++;
	}
	return frameLibre;
}

//Buscar el frame correspondiente a un pid
int buscarFrame(int pid, int *ptr){
	int c=0;
	strAdministrativa aux;
	while(c<FRAMES){
		memcpy(&aux,ptr+c*getSizeStrAdm(),getSizeStrAdm());
		if(compararPID(aux, pid))
			return aux.frame;
		c++;
	}

	return NOT_FOUND;
}

int buscarFrameDePagina(int pid, int *ptr, int pagina){
	int contador;
	strAdministrativa aux;

	while (contador<FRAMES){

		memcpy(&aux, ptr+contador*sizeof(strAdministrativa),sizeof(strAdministrativa));
		if(compararPID(aux,pid)&&aux.nroPagina==pagina){
			return aux.frame;
		}
		contador++;
	}
	return NOT_FOUND;
}



//  ******* FUNCIONES DE CONSOLA DE LA MEMORIA  *******

//Modificar la cantidad de tiempo que espera el proceso memoria antes de responder una solicitud
void retardo(int tiempoEspera) {
	RETARDO_MEMORIA = tiempoEspera;
}

//Genera un reporte en pantalla y en un archivo en disco del estado actual del
//cache, estructuras de memoria, contenido de memoria
void dump() {

}

//Este comando debera limpiar el contenido de la cache
void flush(int *ptrCache) {
}

//MEMORY: Indicara el tamaño de la memoria en cant. de frames, la cant. de frames ocupados y los libres
//PID: Indicara el tamaño total de un procoeso
void size() {


}

//  ******* ENVIAR Y RECIBIR DATOS  *******
int recibirSeleccionOperacion(int socket) {
	int *operacion;
	read(socket,operacion,sizeof(int));
	printf("Orden de operacion recibido:  %d",*operacion);
	return *operacion;
}



// ******* GETS *******

int getPuertoMemoria(){
	return PUERTO_MEMORIA;
}
int getFrameSize(){
	return FRAME_SIZE;
}

int getSizeStrAdm(){
	return sizeof(strAdministrativa);
}

int getRetardo(){
	return RETARDO_MEMORIA;
}




#endif /* FUNCIONESMEMORIA_H_ */
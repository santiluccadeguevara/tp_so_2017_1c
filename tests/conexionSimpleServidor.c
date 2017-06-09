/*
 * conexionSimpleServidor.c
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#include "../librerias/conexionSocket.h"
#include "../librerias/serializador.h"

// Línea para compilar:
// gcc conexionSimpleServidor.c -o conexionSimpleServidor -lcommons
// ./conexionSimpleServidor



// Estructuras de datos

struct estructura{
	u_int32_t numero;
	char letra;
};

struct nodoFijo{
	char letra;
	u_int32_t entero;
	float flotante;
} __attribute__((packed));



// Variables globales

int puertoServidor = 5100;

int socketCliente;

int testServidor = 0;



// Para incrementar tests

void proximoTest(char* enunciado);



// Tests

void pruebaServidor();

void deserializar();

void deserializarPorLasDudas();

void deserializarTamanioVariableFeo();

void deserializarTamanioVariable();

void deserializarListaFija();


// Implementación de tests

void pruebaServidor(){
	u_int32_t datoRecibido;

	recv(socketCliente, &datoRecibido, sizeof(u_int32_t), 0);

	if(datoRecibido == 4)
		printf("\nSe recibió el número 4 correctamente.\n");
	else
		printf("\nQue en paz descanses...\n");
}

void deserializar(){
	struct estructura receptor;

	recibirPaquete(socketCliente, &receptor.numero, sizeof(receptor.numero));

	recibirPaquete(socketCliente, &receptor.letra, sizeof(receptor.letra));



	// Aserciones

	if(receptor.numero == 5)
		printf("Se obtuvo el número correcto 5.\n");
	else
		printf("Se esperaba 5 y obtuviste %d\n", receptor.numero);

	if(receptor.letra == 'b')
		printf("Se obtuvo la letra correcta 'b'.\n\n");
	else
		printf("Se esperaba b y obtuviste %c.\n\n", receptor.letra);
}

void deserializarPorLasDudas(){
	float numero;

	recibirPaquete(socketCliente, &numero, sizeof(float));



	// Aserciones

	if(numero == 30.0)
		printf("Se obtuvo el número correcto 30.0.\n");
	else
		printf("Se esperaba 30.0 y obtuviste %f\n", numero);
}

void deserializarTamanioVariableFeo(){
	u_int32_t* vector;
	size_t tamanio;

	recibirPaquete(socketCliente, &tamanio, sizeof(size_t));

	vector = malloc(tamanio);
	recibirPaquete(socketCliente, vector, tamanio);



	// Aserciones

	if(vector[0] == 300 && vector[1] == 500)
		printf("Se obtuvo el vector correcto (300, 500).\n\n");
	else
		printf("Se esperaba el vector (300, 500) y obtuviste (%d, %d).\n\n", vector[0], vector[1]);

}

void deserializarTamanioVariable(){
	char* recibo;
	char* cadenaEsperada = "Ripinpin";

	recibirPaqueteVariable(socketCliente, &recibo);



	// Aserciones

	if(strcmp(recibo, cadenaEsperada) == 0)
		printf("Se obtuvo la cadena correcta %s.\n\n", cadenaEsperada);
	else
		printf("Se esperaba Ripinpin y obtuviste %s.\n\n", recibo);
}

void deserializarListaFija(){
	struct nodoFijo obtenido1;
	struct nodoFijo obtenido2;
	struct nodoFijo obtenido3;

	size_t tamanioNodo = sizeof(struct nodoFijo);

	t_list* lista = list_create();

	recibirLista(socketCliente, lista, tamanioNodo);

	obtenido1 = *(struct nodoFijo*)list_get(lista, 0);
	obtenido2 = *(struct nodoFijo*)list_get(lista, 1);
	obtenido3 = *(struct nodoFijo*)list_get(lista, 2);



	// Aserciones

	// Nodos esperados

	struct nodoFijo elemento1;
	elemento1.letra = 'a';
	elemento1.entero = 1;
	elemento1.flotante = 30.0;

	int proposicion1 = elemento1.letra == obtenido1.letra && elemento1.entero == obtenido1.entero && elemento1.flotante == obtenido1.flotante;

	struct nodoFijo elemento2;
	elemento2.letra = 'b';
	elemento2.entero = 500;
	elemento2.flotante = 0.5;

	int proposicion2 = elemento2.letra == obtenido2.letra && elemento2.entero == obtenido2.entero && elemento2.flotante == obtenido2.flotante;

	struct nodoFijo elemento3;
	elemento3.letra = 'z';
	elemento3.entero = 666;
	elemento3.flotante = 676.1;

	int proposicion3 = elemento3.letra == obtenido3.letra && elemento3.entero == obtenido3.entero && elemento3.flotante == obtenido3.flotante;

	int veredicto = proposicion1 && proposicion2 && proposicion3;

	if(veredicto)
		printf("Se obtuvo la lista [(a, 1, 30.0) , (b, 500, 0.5), (z, 666, 676.1)]\n\n");
	else
		printf("Jaja saludos. Ni en pedo te hago un printf con lo que te tuvo que haber dado.\n\n");

}

void proximoTest(char* enunciado){
	testServidor++;
	printf("\n\nTest %d\n\n", testServidor);
	printf("%s\n\n", enunciado);
}



// Resultado de las pruebas

int main(){
	socketCliente = servir(puertoServidor);

	proximoTest("Que se pueda recibir el número 4 por socket.");
	pruebaServidor();

	proximoTest("Que se pueda deserializar la estructura compuesta (int = 5, char ='b').");
	deserializar();

	proximoTest("Para que vean que no mandé fruta con el tema de recibir paquetes.");
	deserializarPorLasDudas();

	proximoTest("Para deserializar el vector (300, 500) de manera fea.");
	deserializarTamanioVariableFeo();

	proximoTest("Que se pueda deserializar el string Ripinpin.");
	deserializarTamanioVariable();

	proximoTest("Que se pueda deserializar la lista [(a, 1, 30.0) , (b, 500, 0.5), (z, 666, 676.1)].");
	deserializarListaFija();
}

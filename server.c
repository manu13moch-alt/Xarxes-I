// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	int sock, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};
	char send_buffer[BUFFER_SIZE] = {0};
	int port;
	char *endptr;
	char *comando;

	if (argc == 1) {
		// Cap argument -> port per defecte
		port = DEFAULT_PORT;
		printf("Cap port indicat. Utilitzant valor per defecte: %d\n", port);
	}
	else if (argc == 2) {
		errno = 0;
		port = strtol(argv[1], &endptr, 10);

		// Comprovacions
		if (errno != 0 || *endptr != '\0' || port < 1 || port > 65535) {
			fprintf(stderr, "Error: el port ha de ser un enter entre 1 i 65535.\n");
			exit(EXIT_FAILURE);
		}
	}
	else {
		fprintf(stderr, "Ús: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	// Crear el socket
	// Afegiu comentari explicant els arguments
	

	// - AF_INET: el que fa es indicar que el protocol que utilitzarem és IPv4
	// - SOCK_STREAM: utilitza uns fluxes de dades fiables i orientats a la conexió, indica que utilitza el protocol TCP
	// - 0: especifica que volem utilitzar el protocol predeterminat dels dos paràmetres anteriors, que en aquest cas es TCP
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Error en crear el socket");
		exit(EXIT_FAILURE);
	}


	// Afegiu comentari explicant què són aquests 3 paràmetres,
	// per què s'utilitzen htons() i htonl(),
	// i per què s'utilitzen els valors que hi ha

	// - address.sin_family: ens diu quina família d'adreces utilitzarà l'estructura. En igualar-ho a AF_INET diu que aquesta 
	// estructura té una adreça IPv4.
	// - address.sin_port: ens diu el número exacte de port al que ens volem connectar.
	// El que fa htons() es canviar l'ordre d'organització, normalment tenim un ordre Little-Endian i amb aquesta funció passa 
	// a ser Big-Endian, ja que aquest es l'ordre estàndar i universal de la xarxa.

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);	// Per a INADDR_ANY no faria falta htonl(), però li posem per coherència
	address.sin_port = htons(port);

	// Enllaçar el socket al port especificat
	// Afegiu comentari explicant els arguments

	// - bind(): assigna una adreça i un port al socket creat anteriorment.
	// - sock: és el sock que hem creat i que volem enllaçar.
	// - (struct sockaddr *)&address: es el punter a l'estructura on té l'adreça IP i el port del servidor, adaptat al tipus genèric 
	// sockaddr. 
	// - sizeof(address): és la mida de l'estructura address en bytes.

	if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Error en fer el bind");
		close(sock);
		exit(EXIT_FAILURE);
	}

	// Escoltar connexions entrants
	// Afegiu comentari explicant els arguments
	
	// - listen(): prepara el socket perquè pugui començar a rebre connexions del client
	// - sock: es el sock que hem creat anteriorment.
	// - 3: indica el número màxim de conexions entrants que es poden guardar a la cua.

	if (listen(sock, 3) < 0) {
		perror("Error en escoltar");
		close(sock);
		exit(EXIT_FAILURE);
	}

	printf("Servidor en funcionament, esperant connexions...\n");

	while (1) {
		// Acceptar connexions de clients
		// Afegiu comentari explicant els arguments i per què hi ha i cal new_socket si ja tenim sock
		
		// - accept(): treu la primera petició de la cua i crea un nou socket.
		// - sock: és el sock que hem creat.
		// - (struct sockaddr *)&address: es el punter a l'estructura on té l'adreça IP i el port del servidor, adaptat al tipus genèric 
		// sockaddr. 
		// - (socklen_t *)&addrlen): és un punter a la mida de l'estructura de l'adreça.
		// sock és la porta d'entrada que només serveix per rebre les peticions de connexió inicials, en canvi new_socket es el que creem 
		// amb la funció accept() que serveix per la comunicació amb el client concret.
		// Son necessaries totes dues perquè new_socket s'utilitza per intercanviar dades amb el client actual i socket continua 
		// escoltant i acceptant futurs clients.

		if ((new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
			perror("Error en acceptar la connexió");
			close(sock);
			exit(EXIT_FAILURE);
		}

		 while (1) {
			memset(buffer, 0, BUFFER_SIZE);
			
			// Llegir missatge del client
			if (recv(new_socket, buffer, BUFFER_SIZE-1, 0) <= 0) {
				printf("Client desconnectat (PID: %d)\n", getpid());
				break;
			}

			// Aquí haureu d'implementar l'anàlisi de la cadena rebuda per saber l'operació,
			// i si n'hi ha els arguments, executar-la i tornar el(s) resultat(s)

			// A continuació hi ha el codi corresponent a l'opció d'Enviar missatge, amb el retorn d'una cadena fixa,
			// i la de Sortida de la connexió/bucle si és el cas.
			// Modifiqueu la cadena de retorn per tal que sigui la que diu l'enunciat.

			printf("Missatge rebut del client (PID: %d): %s\n", getpid(), buffer);
			char resposta [BUFFER_SIZE];
			memset(resposta,0,sizeof(resposta));

			if(strcmp(buffer,"EXIT")==0){
				printf("Tancant conexió amb el client \n");
				strcpy(resposta, "Connexió tancada pel servidor \n");
				send(new_socket, resposta, strlen(resposta),0);
				break;
			} 
			
			// Comprovar si el client vol tancar la connexió
			if (strcmp(buffer, "EXIT") == 0) {
				printf("Tancant connexió amb el client (PID: %d)...\n", getpid());
				send(new_socket, "Connexió tancada\n", strlen("Connexió tancada\n"), 0);
				break;	// Sortir del bucle
			} 

			// nuevo
			buffer[strcspn(buffer, "\r\n")] = 0; // Eliminar salt de línia final
			comando = strtok(buffer, "|"); // Separar la cadena rebuda en comanda i arguments


			if(strcmp(comando, "CARTELLERA")==0){  // Comprovar si la comanda és CARTELLERA
				send(new_socket, "OK|1:Inception|2:Interstellar \n",32,0); // Enviar resposta al client 
			} else if (strcmp(comando, "HORARIS")==0){ 
				char *id_peli =strtok(NULL,"|"); // Obtenir l'argument de la comanda HORARIS
				if(id_peli != NULL){
					char resposta[BUFFER_SIZE];
        			snprintf(resposta, sizeof(resposta), "OK|%s|16:30|19:45\n", id_peli);
       				send(new_socket, resposta, strlen(resposta), 0);
				}else{
					send(new_socket,"ERR|Falta ID\n",13,0);
				}
			} 
		}
		

		close(new_socket); // Tancar la connexió amb el client
	}

	close(sock);
	return 0;
}

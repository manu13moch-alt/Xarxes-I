// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_PORT 51887
#define DEFAULT_DOMAIN "localhost"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	int sock = 0;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
	char cadena[BUFFER_SIZE] = "";
	char send_buffer[BUFFER_SIZE] = {0};
	char *domini;
	int port;
	char *endptr;
	long parsed_port;

	if (argc == 1) {
		// Cap argument -> valors per defecte
		domini = DEFAULT_DOMAIN; // asignem la adreça del servidor per defecte Localhost
		port = DEFAULT_PORT; // asignem el port del servidor per defecte 51887
		printf("Cap argument indicat. Utilitzant valors per defecte: %s %d\n", domini, port);
	} 
	else if (argc == 3) {
		domini = argv[1]; // assignem la adreça del servidor que ens passa l'usuari

		errno = 0;
		parsed_port = strtol(argv[2], &endptr, 10);

		// Comprovacions de validesa
		if (errno != 0 || endptr == argv[2] || *endptr != '\0' ||
				parsed_port < 1 || parsed_port > 65535) {
			fprintf(stderr, "Error: el port ha de ser un enter entre 1 i 65535.\n");
			exit(EXIT_FAILURE);
		}
		port = (int)parsed_port;
	} 
	else {
		fprintf(stderr, "Ús: %s [nom_de_domini port]\n", argv[0]); // Qualsevol altre combinació d'arguments és incorrecta, mostrem un missatge d'error i sortim del programa.
		exit(EXIT_FAILURE);
	}



	// Crear el socket
	// Afegiu comentari explicant els arguments, i de quines altres opcions hi ha
	// per al segon d'ells (ara SOCK_STREAM)
	// - AF_INET: el que fa es indicar que el protocol que utilitzarem és IPv4
	// - SOCK_STREAM: utilitza uns fluxes de dades fiables i orientats a la conexió, indica que utilitza el protocol TCP
	// - 0: especifica que volem utilitzar el protocol predeterminat dels dos paràmetres anteriors, que en aquest cas es TCP
	// - Una altre opció que podriem tenir per al segon argument es SOCK_DGRAM que aquest utilitza el protocol UDP que es més ràpid però 
	// menys fiable i també hi ha SOCK_RAW que és per accedir directament a la interfície
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\nError en crear el socket\n");
		return -1;
	}

	// Afegiu comentari sobre què són aquests 3 paràmetres,
	// per què s'utilitzen els valors que hi ha,
	// i per què s'utilitza htons()

	// - serv_addr.sin_family: ens diu quina família d'adreces utilitzarà l'estructura. En igualar-ho a AF_INET diu que aquesta estructura té una adreça IPv4.
	// - serv_addr.sin_port: ens diu el número exacte de port al que ens volem connectar.
	// El que fa htons() es canviar l'ordre d'organització, normalment tenim un ordre Little-Endian i amb aquesta funció passa a ser Big-Endian, ja que aquest es l'ordre estàndar i universal de la xarxa.
	// - inet_pton(): el que fa es convertir una adreça IP escrita en format de text a binari de xarxa de 32 bits.

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port); 
	if (inet_pton(AF_INET, domini, &serv_addr.sin_addr) != 1) {
		fprintf(stderr, "Error: l'adreça IPv4 no és vàlida: %s\n", domini);
		close(sock);
		return EXIT_FAILURE;
	}
   

	// Connectar amb el servidor
	// Afegiu comentari explicant els arguments
	// - sock: es l'identificador del socket que hem creat al principi. La via de la comunicació on s'enviaran totes les dades.
	// - (struct sockaddr *)&serv_addr: es el punter a l'estructura on té l'adreça IP i el port del servidor. 
	// - sizeof(serv_addr): es la mida en bytes de l'estructura serv_addr.

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnexio fallida\n");
		return -1;
	}

	printf("Connectat al servidor. Podeu començar a enviar missatges.\n");

	int option;

	while (1) {
		memset(buffer, 0, sizeof(buffer)); // Netegem el buffer
		memset(send_buffer, 0, sizeof(send_buffer)); 
		// Menú principal
		// Cal que implementeu un petit servei remot amb almenys 4 funcionalitats noves
		// Definiu vosaltres mateixos les dades a enviar (demanar en el client, i analitzar al servidor) i rebre
		// Si per donar més sentit al servei cal alguna opció més, la podeu afegir
		// Deixeu l'opció 1 com a Enviar missatge i l'última per Sortir

		printf("Menú principal:\n");
		printf("1. Enviar missatge\n");
		printf("2. Consulta Cartellera\n");
		printf("3. Consulta Horaris\n");
		printf("4. Consulta Aforament\n");
		printf("5. Consulta Preu\n");
		printf("6. Sortir\n");
		printf("Opció: ");

		scanf("%d", &option);
		while (getchar() != '\n');  // buida el buffer fins al salt de línia

		switch (option)
		{
		case 1:
		case 6:
			if (option==1){
				printf("Introdueix el missatge a enviar ('EXIT' per tancar el servidor i sortir): ");
				fgets(cadena, BUFFER_SIZE, stdin);
				cadena[strcspn(cadena, "\n")] = '\0';  // Eliminar \n final
			} else {
				strcpy(cadena, "EXIT");
			}

			// Enviar missatge al servidor
			// Afegiu control d'errors
			// Afegiu comentari explicant els arguments
			// Utilitzem el send per enviar dades a traves d'una conexió de xarxa activa.
			// Posem una condició i si aquest send es menor que 0 (el número de flags de transmissió per defecte) salta un error.
			// - sock: és el descriptor socket actiu connectat al servidor.
			// - cadena: és el punter a les dades que volem enviar.
			// - strlen(cadena): és la mida exacta en bytes a enviar de cadena.
			// - 0: indica que no hi ha cap flag especial, només volem enviar les dades.

			if (send(sock, cadena, strlen(cadena), 0) != (ssize_t)strlen(cadena) ||
				send(sock, "\n", 1, 0) != 1) {
				printf("Error en enviar dades al servidor \n");
				close(sock);
				return 1;
			}

			// Llegir resposta del servidor
			// Afegiu un control d'errors al recv()
			// Afegiu comentari explicant què fa i per què s'utilitza memset
			// Afegiu comentari explicant els arguments de la crida a recv()
			// memset el que fa es omplir tot el buffer de zeros, això ens serveix per a que no ens quedi cap altre dada d'operacions anterior a la que estem utilitzant.
			// Utilitzem recv per rebre les dades a traves d'una conexió de xarxa activa.
			// - sock: és el descruptor socket actiu que ens envia les dades i d'on volem llegir-les.
			// - buffer: és un punter al bloc de memòria on es guardaran les dades rebudes.
			// - BUFFER_SIZE: és la mida màxima en bytes que accepta llegir.
			// - 0: indica que no hi ha cap flag especial, només volem llegir les dades.

			memset(buffer, 0, BUFFER_SIZE);
			if(recv(sock, buffer, BUFFER_SIZE-1,0)<0){
				printf("Error en rebre dades del servidor \n");
				close(sock);
				return 1;
			} else if (recv(sock, buffer, BUFFER_SIZE-1,0)==0){
				printf("El servidor ha tancat la connexió \n");
				return 0;
			}
			
			printf("Resposta del servidor: %s\n",buffer);

			if (strcmp(cadena, "EXIT") == 0) {
				close(sock);
				return 0;
			}
			break;

		case 2: // CONSULTA CARTELLERA
			if (send(sock, "CARTELLERA\n", 11, 0) != 11) {
				printf("Error en enviar dades al servidor \n");
				close(sock);
				return 1;
			}

			memset(buffer, 0, BUFFER_SIZE);
  		  	if (recv(sock, buffer, BUFFER_SIZE - 1, 0) > 0) {
       			printf("Resposta del servidor: %s\n", buffer);
    		} else {
      			printf("Error en rebre els horaris.\n");
   				}
   			 break;
	

		case 3: { // CONSULTA HORARIS
			char id[50];
			printf("Introdueix l'ID de la pel·lícula: ");
			scanf("%49s", id);
			while (getchar() != '\n');

			int request_length = snprintf(send_buffer, sizeof(send_buffer),
				"HORARIS|%s\n", id);
			if (request_length < 0 || (size_t)request_length >= sizeof(send_buffer) ||
				send(sock, send_buffer, (size_t)request_length, 0) != request_length) {
				printf("Error en enviar dades al servidor \n");
				close(sock);
				return 1;
			}
			memset(buffer, 0, BUFFER_SIZE);
  		  	if (recv(sock, buffer, BUFFER_SIZE - 1, 0) > 0) {
       			 printf("Resposta del servidor: %s\n", buffer);
    		} else {
      			printf("Error en rebre els horaris.\n");
   				 }
			break;
		}

		case 4: // CONSULTA AFORAMENT
			char id[50];
			printf("Introdueix l'ID de la pel·lícula: ");
			scanf("%49s", id);
			while (getchar()!='\n');

			int request_length = snprintf (send_buffer,sizeof(send_buffer),"AFORAMENT|%s\n",id);
			if(request_length <0 ||(size_t)request_length >= sizeof(send_buffer)||send(sock,send_buffer,(size_t)request_length,0) != request_length){
				printf("Error en enviar dades al servidor \n");
				close(sock);
				return 1;
			}

			memset(buffer, 0, BUFFER_SIZE);
  		  	if (recv(sock, buffer, BUFFER_SIZE - 1, 0) > 0) {
       			printf("Resposta del servidor: %s\n", buffer);
    		} else {
      			printf("Error en rebre l'aforament.\n");
   				}
			
			break;

		case 5: { // CONSULTA PREU
			char tipus_client[50];
			int quantitat;
			int request_length;

			printf("Tipus de client (adult, jove o jubilat): ");
			if (scanf("%49s", tipus_client) != 1) {
				printf("No s'ha pogut llegir el tipus de client.\n");
				break;
			}
			printf("Nombre d'entrades: ");
			if (scanf("%d", &quantitat) != 1) {
				printf("La quantitat ha de ser un nombre enter.\n");
				while (getchar() != '\n' && !feof(stdin));
				break;
			}
			while (getchar() != '\n' && !feof(stdin));

			request_length = snprintf(send_buffer, sizeof(send_buffer),
				"PREU|%s|%d\n", tipus_client, quantitat);
			if (request_length < 0 ||
				(size_t)request_length >= sizeof(send_buffer) ||
				send(sock, send_buffer, (size_t)request_length, 0) != request_length) {
				printf("Error en enviar dades al servidor.\n");
				close(sock);
				return 1;
			}

			memset(buffer, 0, sizeof(buffer));
			if (recv(sock, buffer, sizeof(buffer) - 1, 0) > 0) {
				printf("Resposta del servidor: %s", buffer);
			} else {
				printf("Error en rebre el preu del servidor.\n");
			}
			break;
		}

		default:
			printf("Opció invàlida\n");
			break;
		}
	}

	close(sock);// Tancar el socket abans de sortir del programa
	return 0;
}
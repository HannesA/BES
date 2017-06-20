/**
 * @file sender.c
 * Betriebssysteme sender File.
 * Beispiel 3
 *
 * @author Dominik Himmler <ic16b071@technikum-wien.at>
 * @author Alexander Oesterreicher <ic16b011@technikum-wien.at>
 * @author David Sattler <ic16b036@technikum-wien.at>
 * @date 2017/06/16
 *
 * @version 1
 *
 *
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "sem.h"

/**
 *
 * \brief main of sender
 *
 * Checks if semaphores and shared memory exist.
 * If they don't exist, it creates them.
 * Reads data from STDIN and writes it to shared memory.
 *
 * \param argc number of arguments
 * \param argv given arguments
 *
 * \return EXIT_SUCCESS on success, EXIT_FAILURE on error
 *
 */
 
int main(int argc, const char* argv[])
{
	int semid, semid2, shmid, ch, count = 0;
	/*###FB_SG7: pointer sollte = NULL initialisiert werden*/
	int *shmptr;
	long long buffer = 0;	

	/* Überprüfe Argumente */
	buffer = check_params(argc, argv);
	if(buffer == -1)
	{
		return EXIT_FAILURE;
	}
	
	/* Prüfe, ob Semaphore 1 existiert bzw. erstelle einen neuen */
	semid = create_semaphore(KEY, 0);
	if(semid == -1)
	{
		fprintf(stderr, "Fehler seminit()\n");
		return EXIT_FAILURE;
	}
	
	/* Prüfe, ob Semaphore 2 existiert bzw. erstelle einen neuen */
	semid2 = create_semaphore(KEY2, buffer);
	/*###FB_SG7: Hier solltet ihr semid2 pruefen*/
	if(semid == -1)
	{	
		fprintf(stderr, "Fehler seminit2()\n");
		semrm(semid);
		return EXIT_FAILURE;
	}

	/* Erstelle Shared Memory, IPC_EXCL wurde absichtlich nicht angegeben, damit die Reihenfolge des Aufrufs von Sender und Empfänger egal ist */
	errno = 0;
	if((shmid = shmget(KEY, (buffer * sizeof(int)), 0660|IPC_CREAT)) == -1)
	{//Inspect: Hier genau herschauen, sieht bei uns anders aus
		if(errno != EEXIST)
		{
			fprintf(stderr, "Fehler shmget()\n");
			semrm(semid);
			semrm(semid2);
			return EXIT_FAILURE;
		}
	}

	/* Blende Shared Memory in Adressbereich des Prozesses ein */
	shmptr = shmat(shmid, NULL, 0);
	if(shmptr == (int *) -1)
	{
		fprintf(stderr, "Fehler shmat()\n");
		semrm(semid);
		semrm(semid2);
		shmctl(shmid, IPC_RMID, NULL);
		return EXIT_FAILURE;
	}
	
	/* Schleife bis EOF von STDIN gelesen wird */
	while(1)
	{
		/* Lese von STDIN */
		if((ch = fgetc(stdin)) == EOF) break;
		//Inspect: Fehlerbehandlung vollständig?
		errno = 0;
		
		/* Critical Region */
		/* Prüfe auf Freigabe des Semaphores und Unterbrechung des Systemcalls */
		while((P(semid2) == -1 ) && (errno == EINTR))
		{//Inspect: Was ist bei anderen Fehlern?
			errno = 0;			
		}
		
		/* Schreibe eingelesenen Wert auf Shared Memory */
		shmptr[count] = ch;
		
		count = (count + 1)%buffer;
		//Inspect: Was ist bei anderen Fehlern? Was ist wenn V Interrupted wird (DANIEL: Wird in der whileschleife gemacht?)*/
		V(semid);
		/* End of Critical Region */
	}
	//Inspect: Genau anschauen ob das hier sinn macht. Speziell wegen fehlender sync
	
	
	/* Senden von EOF */
	/*###FB_SG7:Fehlerbehanung P() und V() fehlt. */
	P(semid2);
	shmptr[count] = ch;
	/*###FB_SG7:Fehlerbehanung P() und V() fehlt. */
	V(semid);
	
		
	/*###FB_SG7: Hier sollte man wieder detachen*/	
		
	/* Beenden des Programms */
	printf("\n");
	return EXIT_SUCCESS;
}

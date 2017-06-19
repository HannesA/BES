/**
 * @file empfaenger.c
 * Betriebssysteme empfaenger File.
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
 * \brief main of empfaenger
 *
 * Checks if semaphores and shared memory exist.
 * If they don't exist, it creates them.
 * Reads data from shared memory and prints it to STDOUT.
 *
 * \param argc number of arguments
 * \param argv given arguments
 *
 * \return EXIT_SUCCESS on success, EXIT_FAILURE on error
 *
 */
 
int main(int argc, const char* argv[])
{
	int semid, semid2, shmid, count = 0;
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
	if(semid == -1)
	{
		fprintf(stderr, "Fehler seminit2()\n");
		semrm(semid);
		return EXIT_FAILURE;
	}

	/* Erstelle Shared Memory, IPC_EXCL wurde nicht angegeben, damit die Reihenfolge des Aufrufs von Sender und Empfänger egal ist */
	errno = 0;
	if((shmid = shmget(KEY, (buffer * sizeof(int)), 0660|IPC_CREAT)) == -1)
	{
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
	
	/* Schleife bis EOF von Shared Memory gelesen wird */
	while(1)
	{		
		errno = 0;
		
		/* Critical Region */
		/* Prüfe auf Freigabe des Semaphores und Unterbrechung des Systemcalls */
		while((P(semid) == -1 ) && (errno == EINTR))
		{
			errno = 0;			
		}
		
		/* Prüfe auf EOF */
		if(shmptr[count] == EOF) break;
		
		/* Gib Wert von Shared Memory aus */
		fputc(shmptr[count], stdout);
		
		count = (count + 1)%buffer;
		V(semid2);
		/* End of Critical Region */
	}
	
	/* Nur der Empfaenger entfernt den Shared Memory und die Semaphoren */
	semrm(semid);
	semrm(semid2);
	shmctl(shmid, IPC_RMID, NULL);
	
	/* Beenden des Programms */
	return EXIT_SUCCESS;
}

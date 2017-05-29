/**
 * @file sharedfile.c
 * Betriebsysteme Beispiel 3
 *
 * @author Daniel Scheidl 
 * @author Hannes Aurednik 
 * @author Eva Gergely 
 * @author Raphael Szabo 
 *
 * @date 2017
 *
 * @version 123
 *
 */

#include <limits.h>
#include <sys/shm.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>

#include <sem182.h>
#include "sharedfile.h" 
 
 
 static int ringbuffer = -1;							//buffer - wird hinter -m beim programmaufruf angegeben
 char *FILENAME; 							//wird durch empfänger und sender gesetzt
 static int semid[2]; 								//Semaphoren für Write[0] und Read[1]
 static int shmid = -1;
 
 //static key_t key[2] = {getuid()* 1000, getuid() * 1000 + 1}; 	//key für die semaphoren
 static key_t key[2] = {1932005, 1932006};
 static key_t shmkey = 1932004;
 
 
 /*
	Im Header:
	int do_ringbuffersize(int argc, char const argv[]);
	void do_semaphorinit(void);	
	int do_cleanup(void);
	void gotanerror(char *message);
 */

 
 /**
 *
 * \brief Sucht nach Argumenten hinter '-m' und gibt sie als int Wert an Aufrufer retour
 *
 * \param argc Argument Counter
 * \param argv Pointer auf den type-Parameter
 *
 * \return int 
 * \retval -1 im Fehlerfall
 *
 */
 int do_ringbuffersize(int argc, char* const argv[]) /*analysiert zeichen hinter -m*/
 {
	 int optret = 0; //Retrun Value of getopt()
	 char *endptr = NULL;
	 int foundargments = 0;
	 
	 
	errno = 0;
	while((optret = getopt(argc, argv, "m:")) != -1){
	
		if(optret == 'm'){
			
			ringbuffer = strtol(optarg, &endptr, 10);
			if((errno == ERANGE || (ringbuffer == LONG_MAX || ringbuffer == LONG_MIN)) || (*endptr != '\0') || (errno != 0 && ringbuffer == 0)){
				gotanerror("WRONG ARGUMENTS - Usage: -m <ringbuffer elements>\n ");
				return -1;
			}else{
				foundargments = 1;
			}
			
		}else{
		
			if(optopt == 'm'){  //Wenn   getopt()   ein   Optionszeichen   nicht   erkennt,   wird   eine Fehlernachricht  nach  stderr  ausgegeben,  das   Zeichen   in   optoptgespeichert  und  `?'  zurückgegeben
				gotanerror("WRONG ARGUMENTS - Usage: -m <ringbuffer elements>\n ");
				return -1; 			
			}
			gotanerror("COULD NOT READ ARGUMENTS - Usage: -m <ringbuffer elements>\n");
			return -1;
		
		}
		
	}
		
	if(foundargments != 1){
		gotanerror("NO ARGUMENT BEHIND -m - Usage: -m <ringbuffer elements>\n ");
		return -1;
	}	
	if(optind < argc )
	{
		gotanerror("ADDITIONAL ARGUMENT BEHIND -m - Usage: -m <ringbuffer elements>\n ");
		return -1;
	}

	if (ringbuffer <= 0)
	{
		gotanerror("RINGBUFFERSIZE MUST BE >0 - Usage: -m <ringbuffer elements>\n ");
		return -1;
	}			
			
			
	/*if (ringbuffer > SHMMAX)
	{
		gotanerror("RINGBUFFERSIZE IS TOO BIG");
		return -1;
	}*/
	
	return ringbuffer;
 }
	


/**
 *
 * \brief legt semaphor für lese & schreibvorgang an
 *	lesesemaphor wird im 2. schleifendurchlauf auf 0 gesetzt
 * \param argc Argument Counter
 * \param argv Pointer auf den type-Parameter
 *
 * \return int? 
 * \retval -1 im Fehlerfall
 *
 */
 
int do_semaphorinit(void) /*initalisiert bzw. holt semaphor (geholt wird nur im empfänger)*/
{	
	int startbuffer = ringbuffer;
	int i = 0;
	
	for(i = 0; i < 2; i++){
		
		if(((semid[i] = seminit(key[i], 0660, startbuffer)) == -1)){
			
			if(errno != EEXIST){
				
				if((semid[i] = semgrab(key[i])) ==1){
					
					gotanerror("ERROR WHILE GRABING SEMAPHOR - already existing, but not grabable");
					do_cleanup();
					return -1;
				}
				gotanerror("ERROR WHILE INITIALISING SEMAPHOR");
				do_cleanup();
				return -1;
			}
		}
		startbuffer = 0;
	}	
	return 0;	
}
	
	
	
int do_sharedmemory(void) /*legt shared memory mit individuellem key an*/
{
	if((shmid = shmget(shmkey, ringbuffer, 0660|IPC_CREAT)) == -1)  // eröffne den shared memory mit rechten 0660
	{
		gotanerror("ERROR WHILE GETTING SHARED MEMORY");
		do_cleanup();
		return EXIT_FAILURE;
	}
	return 0;
}	




	
int do_cleanup(void)
{
	int i = 0;
	//räume semaphot weg
	for (i = 0; i<2 ; i++){
		if (semrm(semid[i]) == -1) {
			gotanerror("Removal was not successfull");
			return EXIT_FAILURE;
		}
		
		return EXIT_FAILURE; //damits kompiliert
	}
	return EXIT_FAILURE; //damits kompiliert
	
	//TO-DO addressbereich wegräumen
	//TO-DO räume shm weg	
	
}

void gotanerror(char *message)
{																	/*Kein Fehlercode in errno*/
	if(errno == 0) fprintf(stderr,"%s: %s\n", FILENAME, message);
																	/*Wenn Fehlercode in errno dann Ausgabe inkl. genauerer Fehlerinformation*/
	else fprintf(stderr,"%s: FAILURE:%s // MSG:%s\n", FILENAME, strerror(errno), message);
			
}	
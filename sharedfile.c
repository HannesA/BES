 /**
 * @file sharedfile.c
 * Betriebssysteme Ringbuffer
 * Beispiel 3
 *
 * @author Hannes Aurednik <ic15b004@technikum-wien.at>
 * @author Eva Dorottya Gergely <ic16b064@technikum-wien.at>
 * @author Daniel Scheidl <ic16b073@technikum-wien.at>
 * @author Raphael Szabo <ic16b062@technikum-wien.at>
 * 
 * @date 2017/06/02
 *
 * @version 0.2
 *
 *
 */
/*
 * -------------------------------------------------------------- includes --
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
#include <sys/types.h>
#include <sem182.h>
#include "sharedfile.h" 
 /*
 * --------------------------------------------------------------- defines --
 */
#define SENDERINDEX 0
#define RECEIVERINDEX 1
 /*
 * --------------------------------------------------------------- globals --
 */
 static int ringbuffer = -1;							//buffer - wird hinter -m beim programmaufruf angegeben
 char *FILENAME; 							//wird durch empfänger und sender gesetzt
 static int semid[2]; 								//Semaphoren für Write[0] und Read[1]
 static int shmid = -1;
 static int *shmptr = NULL;
 //static key_t key[2] = {getuid()* 1000, getuid() * 1000 + 1}; 	//key für die semaphoren
 static key_t key[2] = {1932005, 1932006}; /*[0] = Sender, [1] = Empfaenger*/
 static key_t shmkey = 1932004;
 
 /*
 * ------------------------------------------------------------ prototypes --
 */
 static void do_KeyInit(void);
 /*
	Im Header:
	int do_ringbuffersize(int argc, char const argv[]);
	void do_semaphorinit(void);	
	int do_cleanup(void);
	void gotanerror(char *message);
 */
 /*
 * ------------------------------------------------------------- functions --
 */
 /**
 *
 * \brief Initialisiert die Keys abhaengig vom Aufrufer
 *
 * \param void
 *
 * \return void 
 */
 
static void do_KeyInit(void){
	int tmp = (int) getuid(); /*Manpage: "These Functions are always successful"*/
	tmp *= 1000;
	key[SENDERINDEX]=tmp+0;
	key[RECEIVERINDEX]=tmp+1;
	shmkey = tmp+2;
}
 
 
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
				gotanerror("usage: ./PROGRAMM -m <buffersize 1 to x> - WRONG ARGUMENTS");
				return -1;
			}else{
				foundargments = 1;
			}
			
		}else{
		
			if(optopt == 'm'){  //Wenn   getopt()   ein   Optionszeichen   nicht   erkennt,   wird   eine Fehlernachricht  nach  stderr  ausgegeben,  das   Zeichen   in   optoptgespeichert  und  `?'  zurückgegeben
				gotanerror("usage: ./PROGRAMM -m <buffersize 1 to x> - WRONG ARGUMENTS");
				return -1; 			
			}
			gotanerror("usage: ./PROGRAMM -m <buffersize 1 to x> - COULD NOT READ ARGUMENTS");
			return -1;
		
		}
		
	}
		
	if(foundargments != 1 || optind < argc || ringbuffer <= 0){
		gotanerror("usage: ./PROGRAMM -m <buffersize 1 to x> - INVALID ARGUMENT BEHIND -m");
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
 *
 * \param void
 *
 * \return Integer
 * \retval -1 im Fehlerfall
 * \retval 0 wenn erfolgreich
 *
 */
int do_semaphorinit(void) /*initalisiert bzw. holt semaphor (geholt wird nur im empfänger)*/
{	
	int startbuffer = ringbuffer;
	int i = 0;
	
	do_KeyInit(); //Hier werden die Keys wirklich initialisiert
	
	for(i = 0; i < 2; i++){
		
		if(((semid[i] = seminit(key[i], 0660, startbuffer)) == -1)){
			
			if(errno == EEXIST){
				
				if((semid[i] = semgrab(key[i])) == -1){
					
					gotanerror("ERROR WHILE GRABING SEMAPHOR - already existing, but not grabable");
					do_cleanup();
					return EXIT_FAILURE;
				}

			}else {
			gotanerror("ERROR WHILE INITIALISING SEMAPHOR - no EEXIST message");
			do_cleanup();
			return EXIT_FAILURE;
			}
		}
		startbuffer = 0;
	}	
	return 0;	
}
	
	
/**
 *
 * \brief Legt den Shared Memory an
 *	
 * \param void
 *
 * \return int
 * \retval 0 wenn erfolgreich
 * \retval EXIT_FAILURE im Fehlerfall
 *
 */	
int do_sharedmemory(void) 
{
	if((shmid = shmget(shmkey, ringbuffer, 0660|IPC_CREAT)) == -1)  // eröffne den shared memory mit rechten 0660
	{
		gotanerror("ERROR WHILE GETTING SHARED MEMORY");
		do_cleanup();
		return EXIT_FAILURE;
	}
	return 0;
}
	
/**
 *
 * \brief Bindet den Shared Memory ein
 *	
 * \param access_mode steuert ob read & write oder read only Zugriff
 *
 * \return integer
 * \retval 0 wenn erfolgreich
 * \retval EXIT_FAILURE im Fehlerfall
 *
 */	
int do_attachSM(int access_mode) 
{
		shmptr = shmat(shmid, NULL, (access_mode == 1 ? 0 : SHM_RDONLY)); /*access_mode == 1 --> r&w sonst read only*/
		if(shmptr == (int *) -1)
		{
			gotanerror("ERROR WHILE ATTACHING SHARED MEMORY");
			do_cleanup();
			return EXIT_FAILURE;
		}

	return 0;
}	



/**
 *
 * \brief 
 *	
 * \param void
 *
 * \return int
 * \retval EXIT_FAILURE im Fehlerfall
 *
 */	
int do_cleanup(void)//TODO: Return Wert notwendig?
{
	int i = 0;
	//räume semaphot weg
	for (i = 0; i<2 ; i++){
		if (semrm(semid[i]) == -1) {
			gotanerror("ERROR WHILE REMOVING SEMAPHOR");
			return EXIT_FAILURE;
		}
		
		return EXIT_FAILURE; //damits kompiliert
	}
	
	//Blende SHM Adressbreich aus
	if (shmdt(shmptr) == -1){
		gotanerror("ERROR WHILE HIDING SHARED MEMORY SEGMENT");
		return EXIT_FAILURE;
	}
	
	//Entferne Shared Memory
	if (shmctl(shmid, IPC_RMID, NULL) == 1){ 
		gotanerror("ERROR WHILE REMOVING SHARED MEMORY SEGMENT");
		return EXIT_FAILURE;
	}
	
	return 0;
		

}

/**
 *
 * \brief Schreibt ein Zeichen in den Shared Memory
 *	
 * \param data zu schreibendes Zeichen
 *
 * \return Integer zur Fehlererkennung
 *
 * \retval EXIT_FAILURE im Fehlerfall
 * \retval 0 wenn erfolgreich
 */	
int do_writeSM(int data){
	
	static int senderIndex = 0;
	
	while(P(semid[SENDERINDEX])== -1)
	{
		if(errno == EINTR) continue;
		gotanerror("ERROR P-ing Sender-Semaphor");
		do_cleanup();
		return EXIT_FAILURE;
		
	}
	/*Critical Region*/
	
	shmptr[senderIndex] = data;
	
	
	while(V(semid[RECEIVERINDEX])==-1)
	{
		if(errno == EINTR) continue;
		gotanerror("ERROR V-ing Receiver-Semaphor");
		do_cleanup();
		return EXIT_FAILURE;
		
	}
	senderIndex++;
	senderIndex%=ringbuffer;
	
	return 0;
}


/**
 *
 * \brief Liest ein Zeichen in den Shared Memory
 *	
 *
 * \return Daten die gelesen wurden
 *
 */	
int do_readSM(void){
	
	static int readIndex = 0;
	int data = 0;
	
	while(P(semid[RECEIVERINDEX])==-1)
	{
		if(errno == EINTR) continue;
		gotanerror("ERROR P-ing Receiver-Semaphor");
		do_cleanup();
		return EXIT_FAILURE;	
	}
	/*Critical Region*/
	data = shmptr[readIndex];
	
	readIndex++;
	readIndex%=ringbuffer;
	
	while(V(semid[SENDERINDEX])==-1)
	{
		if(errno == EINTR) continue;
		gotanerror("ERROR V-ing Sender-Semaphor");
		do_cleanup();
		return EXIT_FAILURE;
	}
	
	
	return data;
}


/**
 *
 * \brief zentrale Fehlerausgabe
 *	
 * \param *message Auszugebende Fehlermeldung
 *
 * \return void
 *
 */	
void gotanerror(char *message)
{																	/*Kein Fehlercode in errno*/
	if(errno == 0) fprintf(stderr,"%s: %s\n", message, FILENAME);
																	/*Wenn Fehlercode in errno dann Ausgabe inkl. genauerer Fehlerinformation*/
	else fprintf(stderr,"%s: FAILURE:%s, %s\n", message, strerror(errno), FILENAME);
			
}	
/*
 * =================================================================== eof ==
 */
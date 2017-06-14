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
 * @date 2017/06/14
 *
 * @version 1.0
 *
 *
 */
/*
 * -------------------------------------------------------------- includes --
 */
#include <time.h> //TODO: Wofuer brauchen wir time.h??
/*string, stdio, errno, unistd, stdlib im header*/
#include <stdint.h> //TODO: Sind die alle notwendig??
#include <limits.h>
#include <sys/shm.h>
#include <getopt.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sem182.h>
#include "sharedfile.h"

/*
 * --------------------------------------------------------------- defines --
 */
#define SENDERINDEX 0
#define RECEIVERINDEX 1
#define SHMMAX_SYS_FILE "/proc/sys/kernel/shmmax" //TODO nie benutzt, raus?
#define SHMALL_SYS_FILE "/proc/sys/kernel/shmall"
#define RIGHTS  0660
/*
 * --------------------------------------------------------------- globals --
 */
char *FILENAME; /*wird durch Empfaenger und Sender gesetzt*/ 

static unsigned long ringbuffer = 0;
static int semid[2] = {-1, -1}; /*Semaphoren für Write-[0] und Read-[1]*/
static int shmid = -1; 
static int *shmptr = NULL;
static key_t key[2] = {-1, -1}; /*[0] = Sender, [1] = Empfaenger*/
static key_t shmkey = -1;

/*
 * ------------------------------------------------------------ prototypes --
 */
static void do_KeyInit(void);
static unsigned long get_shmall(void);
/*
 * ------------------------------------------------------------- functions --
 */
 
/**
 *
 * \brief Initialisiert die Keys abhaengig von der uid des Aufrufers
 *
 *
 * \return void
 */
static void do_KeyInit(void){
    int tmp = (int) getuid(); /*Manpage: "These Functions are always successful"*/
    tmp *= 10000;
    key[SENDERINDEX]=tmp+0;
    key[RECEIVERINDEX]=tmp+1;
    shmkey = tmp+2;
}

/**
 *
 * \brief Holt den SHMALL Wert -> Maximale Groesse eines Shared Memories
 *
 * \param void
 *
 * \return SHMALL Value
 */
static unsigned long get_shmall(void)
{
    unsigned long shmall;
    FILE *f = fopen(SHMALL_SYS_FILE,"r");
    if(!f){
        gotanerror("Unable to read /proc/sys/shmall");
        return -1;
    }
    if(fscanf(f,"%lu", &shmall) !=1){
        gotanerror("Unable to read /proc/sys/shmall");
        fclose(f);
        return -1;
    }
    fclose(f);
	
    return shmall*4;
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
int do_ringbuffersize(int argc, char* const argv[]) 
{
	int optret = 0; /*Return Value of getopt()*/
    char *endptr = NULL;
    errno = 0;
    while((optret = getopt(argc, argv, "m:")) != -1){
        
        
        switch (optret){
            case 'm':
                ringbuffer = strtoul(optarg, &endptr, 10);
                if((errno == ERANGE || ringbuffer == ULONG_MAX || (*endptr != '\0') || (errno != 0 && ringbuffer <= 0)||ringbuffer<=0))
                {
                    gotanerror("Usage: ./PROGRAMM -m <buffersize 1 to x> - WRONG ARGUMENT");
                    return -1;
                }
                else
                {
                    if(optind < argc){
                        gotanerror("Usage: ./PROGRAMM -m <buffersize 1 to x> - TOO MANY ARGUMENTS");
                        return -1;
                    }
                    else
                    {
                        if (ringbuffer>=(get_shmall())){
                            gotanerror("Usage: ./PROGRAMM -m <buffersize 1 to x> - BUFFER SIZE TOO LARGE");
                            return -1;
                        }
                        return ringbuffer;/*Alle Checks bestanden*/
                    }
                }
                
            case '?':
                if (optopt=='m')
                {
                    gotanerror("Usage: ./PROGRAMM -m <buffersize 1 to x> - OPTION REQUIRES NUMERIC ARGUMENT");
                    return -1;
                }
            default:
                gotanerror("Usage: ./PROGRAMM -m <buffersize 1 to x> - INVALID ARGUMENTS");
                return -1;
        }
        
    }

    gotanerror("Usage: ./PROGRAMM -m <buffersize 1 to x> FATAL ERROR WHEN GETTING RINGBUFFERSIZE!!");
    return -1;
	
}



/**
 *
 * \brief legt semaphor fuer lese & schreibvorgang an
 *	Lesesemaphor wird im 2. Schleifendurchlauf auf 0 gesetzt
 *
 * \return Integer
 * \retval -1 im Fehlerfall
 * \retval 0 wenn erfolgreich
 *
 */
int do_semaphorinit(void)
{
    unsigned int startbuffer = ringbuffer;
    int i = 0;
    errno = 0;
	
    do_KeyInit(); /*Hier werden die Keys initialisiert*/
    
    for(i = 0; i < 2; i++){
        
        if(((semid[i] = seminit(key[i], RIGHTS, (unsigned int) startbuffer)) == -1)){
            
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
 * \return int
 * \retval 0 wenn erfolgreich
 * \retval EXIT_FAILURE im Fehlerfall
 *
 */
int do_sharedmemory(void)
{
    if((shmid = shmget(shmkey,sizeof(int)*ringbuffer, RIGHTS|IPC_CREAT)) == -1)
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
 * \param access_mode steuert ob read & write oder read_only Zugriff
 *
 * \return integer
 * \retval 0 wenn erfolgreich
 * \retval EXIT_FAILURE im Fehlerfall
 *
 */
int do_attachSM(int access_mode)
{
	shmptr = shmat(shmid, NULL, (access_mode == 1 ? 0 : SHM_RDONLY)); /*access_mode == 1 --> r&w sonst read only*/
    if(shmptr ==  (int*)-1)
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
 * \return int
 * \retval EXIT_FAILURE im Fehlerfall
 *
 */
int do_cleanup(void)//TODO: Return Wert notwendig? Wird nirgends abgefragt
{
    int tmp_counter = 0;
    
	
    /*Blende SHM Adressbereich aus*/
    if(shmptr != NULL){
        if (shmdt(shmptr) == -1){
            gotanerror("ERROR WHILE HIDING SHARED MEMORY SEGMENT");
            return EXIT_FAILURE;
        }
    }
    
    /*Entferne Shared Memory*/
    if (shmid != -1){
        if (shmctl(shmid, IPC_RMID, NULL) == -1){
            gotanerror("ERROR WHILE REMOVING SHARED MEMORY SEGMENT");
            return EXIT_FAILURE;
        }
    }
    
    /*Semaphoren wegrauemen*/
    
    for (tmp_counter = 0; tmp_counter<2 ; tmp_counter++){
        if(semid[tmp_counter] != -1){
            if (semrm(semid[tmp_counter]) == -1) {
                gotanerror("ERROR WHILE REMOVING SEMAPHOR");
                return EXIT_FAILURE;
            }
        semid[tmp_counter] = -1;
        }
        
    }
    shmid = -1;
    shmptr = NULL;
    
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
    errno = 0;
	
    while(P(semid[SENDERINDEX])== -1)
    {
        if(errno!= EINTR){
			
			gotanerror("ERROR P-ing Sender-Semaphor");
			do_cleanup();
			return EXIT_FAILURE;
        }
		errno = 0;
        
    }
    /*Critical Region*/
    
    shmptr[senderIndex] = data;
    
    
    while(V(semid[RECEIVERINDEX])==-1)
    {
        if(errno!= EINTR){
			
			gotanerror("ERROR V-ing Receiver-Semaphor");
			do_cleanup();
			return EXIT_FAILURE;
        }
		errno = 0;
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
    errno = 0;
    
    while(P(semid[RECEIVERINDEX])==-1)
    {	
        if(errno != EINTR){
			
			gotanerror("ERROR P-ing Receiver-Semaphor");
			do_cleanup();
			return -2;
        }
		
		errno = 0;
    }
    /*Critical Region*/
    data = shmptr[readIndex];
    
    readIndex++;
    readIndex%=ringbuffer;
   
    /*Ende - Critical Region*/
    while(V(semid[SENDERINDEX])==-1)
    {
        if(errno != EINTR){
			
			gotanerror("ERROR V-ing Sender-Semaphor");
			do_cleanup();
			return -2;
        }
		errno = 0;
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
{   /*Kein Fehlercode in errno*/
    if(errno == 0) fprintf(stderr,"%s: %s\n", message, FILENAME);
    /*Wenn Fehlercode in errno dann Ausgabe inkl. genauerer Fehlerinformation*/
    else fprintf(stderr,"%s: FAILURE:%s, %s\n", message, strerror(errno), FILENAME);
    
}	
/*
 * =================================================================== eof ==
 */

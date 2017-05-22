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
 * @version 1.0.0.0
 *
 */

#include "sharedfile.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sem182.h>

 
 static int ringbuffer = 0;		//buffer - wird hinter -m beim programmaufruf angegeben
 static const char *FILENAME; //wird durch empfänger und sender gesetzt
 static int semid[2] = 0; //Semaphoren für Write[0] und Read[1]
 static int key[2] = {getpwuid()*1000,getpwuid()*1000 + 1};	//key für die semaphoren
 
 
 

 
 
 
 /*Sucht nach Argumenten hinter '-m' und gibt sie als int wert an aufrufer retour*/
 int do_ringbuffersize(int argc, char const argv[])
 {
	 int optret = 0;
	 char *endptr = NULL;
	 int foundargments = 0;
	 
	 
	errno = 0;
	while((optret = getopt(argc, argv, "m:")) != -1){
	
		if(optret == 'm'){
			
			ringbuffersize = strtol(optarg, &endptr, 10)
			if((errno == ERANGE || (ringbuffersize == LONG_MAX || ringbuffersize == LONG_MIN)) || (*endptr != '\0') || (errno != 0 && size == 0)){
				gotanerror("WRONG ARGUMENTS - Usage: -m <ringbuffer elements>\n ");
				return -1;
			}else{
				argumentsfound = 1;
			}
			
		}else{
		
			if(optopt == 'm'){  //Wenn   getopt()   ein   Optionszeichen   nicht   erkennt,   wird   eine Fehlernachricht  nach  stderr  ausgegeben,  das   Zeichen   in   optoptgespeichert  und  `?'  zurückgegeben
				gotanerror("WRONG ARGUMENTS - Usage: -m <ringbuffer elements>\n ")
				return -1; 			
			}
			gotanerror("COULD NOT READ ARGUMENTS - Usage: -m <ringbuffer elements>\n");
			return -1;
		
		}
		
	}
		
	if(mfound != 1){
		gotanerror("COULD NOT FIND ARGUMENT BEHIND -m - Usage: -m <ringbuffer elements>\n ");
		return -1;
	}	
	if( optind < argc )
	{
		gotanerror("THERE WAS AN ADDITIONAL ARGUMENT BEHIND -m - Usage: -m <ringbuffer elements>\n ");
		return -1;
	}

	if (ringbufferSize <= 0)
	{
		fprintf("RINGBUFFERSIZE MUST BE <0 - Usage: -m <ringbuffer elements>\n ");
		return -1;
	}		
			
	return ringbuffer;
 }
	
//legt semaphor für lese & schreibvorgang an
//lesesemaphor wird im 2. schleifendurchlauf auf 0 gesetzt
void do_semaphorinit()
{	
	startbuffer = ringbuffer;

	for(int i = 0, i < 2, i++){
		if(((semid[i] = seminit(key[i], 0660, startbuffer)) == -1){
			if(errno != EEXIST){
				if((semid[i] = semgrap(key[i])) ==1){
					gotanerror("ERROR WHILE GRABING SEMAPHOR - already existing, but not grabable");
					//wegräumen des semaphors?
					return -1;
				}
				gotanerror("ERROR WHILE INITIALISING SEMAPHOR");
				//wegräumen des semaphors?
				return -1;
			}
		}
		startbuffer = 0;
	}			
}
	
	

static void gotanerror(char *message)
{																	/*Kein Fehlercode in errno*/
	if(errno == 0){
		fprintf(stderr,"%s: %s\n", FILENAME, message);
	}
																	/*Wenn Fehlercode in errno dann Ausgabe inkl. genauerer Fehlerinformation*/
	else{
		fprintf(stderr,"%s: FAILURE:%s // MSG:%s\n", FILENAME, strerror(errno), message);
		
	}
	
}	
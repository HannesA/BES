 /**
 * @file empfaenger.c
 * Betriebssysteme Ringbuffer
 * Beispiel 3
 *
 * @author Hannes Aurednik <ic15b004@technikum-wien.at>
 * @author Eva Dorottya Gergely <ic16b064@technikum-wien.at>
 * @author Daniel Scheidl <ic16b073@technikum-wien.at>
 * @author Raphael Szabo <ic16b062@technikum-wien.at>
 * 
 * @date 2017/06/16
 *
 * @version Abgabe
 *
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
/*string, stdio, errno, unistd, stdlib im header*/
#include "sharedfile.h"

/*
 * --------------------------------------------------------------- defines --
 */

/*
 * --------------------------------------------------------------- globals --
 */

/*
 * ------------------------------------------------------------- functions --
 */
	

/**
 *
 * \brief Main Entry empaenger.c - Liest aus dem Shared Memory
 *
 * Liest aus dem Shared Memory bis er EOF erhaelt
 *
 * \param argc Argument Counter
 * \param argv[] Argument Values
 *
 * \return 0 bei Erfolg
 *
 * \retval EXIT_FAILURE im Fehlerfall
 */
int main (int argc, char* argv[])
{
	FILENAME = argv[0];
	int data = -1;
	
	/*----Umgebung anlegen und einbinden----*/
	if (do_ringbuffersize(argc, argv) == -1) return EXIT_FAILURE; 
	
	if (do_semaphorinit() != 0 ) return EXIT_FAILURE;
	
	if (do_sharedmemory() != 0) return EXIT_FAILURE;
	
	if (do_attachSM(0) != 0) return EXIT_FAILURE; /*access_mode == 1 --> r&w sonst read only*/
	
	/*----Eigentliche Verarbeitung----*/
	do{
		errno = 0;
		if (ferror(stdin))
		{
			gotanerror("ERROR in stdin");
			do_cleanup();
			return EXIT_FAILURE;
		}
        
		if((data=do_readSM())==-2){
			
			gotanerror("ERROR reading from Shared Memory");
            do_cleanup();
			return EXIT_FAILURE;
		}
		
		if(data==EOF) break;
		
		if(putchar(data)==EOF){
			if(ferror(stdout)!=0)
			{ 
				gotanerror("ERROR writing to stdout");
				do_cleanup();
				return EXIT_FAILURE;		
			}
		}
	}while(data != EOF);
    
	if(fflush(stdout) == EOF)
	{	do_cleanup();
		gotanerror("ERROR fflush-ing stdout");
		return EXIT_FAILURE; 
	}
	/*----Umgebung wegraeumen----*/
    do_cleanup();
    
    return 0;
}
	
/*
 * =================================================================== eof ==
 */

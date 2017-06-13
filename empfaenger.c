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
 * @date 2017/06/12
 *
 * @version 0.9
 *
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */


#include <errno.h>
#include <stdlib.h>//TODO: Notwendigkeit aller Includes pruefen
#include <string.h>
#include <stdio.h>
#include <unistd.h>
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
	



int main (int argc, char* argv[])
{
	FILENAME = argv[0];
	int data = -1;
	//TODO: In einen Aufruf packen?
	/*----Umgebung anlegen und einbinden----*/
	if (do_ringbuffersize(argc, argv) == -1) return EXIT_FAILURE; /*EXIT_FAILURE gehört zur stdlib.h*/
	
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
	
	if (fflush(stdout) == EOF)
	{
		gotanerror("ERROR fflush");
		return EXIT_FAILURE; 
	}
	//TODO: Rückgabewert abfragen oder bei Cleanup rausnehmen??
    do_cleanup();
    
    return 0;
}
	
/*
 * =================================================================== eof ==
 */

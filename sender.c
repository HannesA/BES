 /**
 * @file sender.c
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>//TODO: Notwendigkeit aller Includes prüfen
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
	
	/*----Umgebung anlegen und einbinden----*/
	if (do_ringbuffersize(argc, argv) == -1) return EXIT_FAILURE; /*EXIT_FAILURE gehoert zur stdlib.h*/
	
	if (do_semaphorinit() != 0 ) return EXIT_FAILURE;
	
	if (do_sharedmemory() != 0) return EXIT_FAILURE;
	
	/*Shared Memory einbinden*/
	if (do_attachSM(1) != 0) return EXIT_FAILURE; /*access_mode == 1 --> r&w sonst read only*/
	
    /*----Eigentliche Verarbeitung----*/
	do{
		errno=0;
		data = fgetc(stdin);
		/*Senden = in Shared Memory schreiben*/
		
		if(ferror(stdin)!=0)
		{
			gotanerror("ERROR IN ferror()");
			do_cleanup();
			return EXIT_FAILURE;
		}
		
		if(do_writeSM(data)==EXIT_FAILURE){
			
			gotanerror("ERROR Writing to Shared Memory");
			return EXIT_FAILURE;
		}
		
		
	}while(data !=  EOF);
	
	//TODO: Detach vom Shared Memory
	
	//TODO: Errorhandling genug bzw an richtiger stelle?
	return 0;

}
	
/*
 * =================================================================== eof ==
 */

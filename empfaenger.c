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
	char *FILENAME = argv[0];
	int data = -1;
	
	/*----Umgebung anlegen und einbinden----*/
	if (do_ringbuffersize(argc, argv) == -1) return EXIT_FAILURE; /*EXIT_FAILURE gehört zur stdlib.h*/
	
	if (do_semaphorinit() == -1 ) return EXIT_FAILURE;
	
	if (do_sharedmemory() != 0) return EXIT_FAILURE;
	
	
	if (do_attachSM(0) != 0) return EXIT_FAILURE; /*access_mode == 1 --> r&w sonst read only*/
	
	/*----Eigentliche Verarbeitung----*/
	
	/*while(int data = funktion die noch wer schreiben muss und zeichenweise aus dem shared memory liest)!=EOF{*/
	while(1){
	
	// Fehlerbehandlung?
	
	// Ausgabe nach stdout
	//TODO: Fehlerbehandlung vollstaendig?
		if(fputc(data, stdout)==EOF){
			if(ferror(stdout)!=0)
			{ 
				do_cleanup();
				return EXIT_FAILURE;		
			}
		}
	
	}
	// Semaphore und Shared Memory wegräumen
	do_cleanup();
	
	fprintf(stderr, "Error in %s: %s\n", FILENAME, "fgetc() returned error");	//damits kompiliert (FILENAME UNUSED)
	return 0;
}
	
/*
 * =================================================================== eof ==
 */

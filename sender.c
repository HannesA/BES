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
 * \brief Main Entry sender.c
 *
 * \param argc Argument Counter
 * \param argv[] Argument Values
 *
 * \return 0 bei Erfolg
 * \retval EXIT_FAILURE im Fehlerfall
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
		
		
		if(ferror(stdin)!=0)
		{
			gotanerror("ERROR in ferror()");
			do_cleanup();
			return EXIT_FAILURE;
		}
		/*Senden = in Shared Memory schreiben*/
		if(do_writeSM(data)==EXIT_FAILURE){
			do_cleanup();
			gotanerror("ERROR Writing to Shared Memory");
			return EXIT_FAILURE;
		}
		
		
	}while(data !=  EOF);
	
	return 0;

}
	
/*
 * =================================================================== eof ==
 */

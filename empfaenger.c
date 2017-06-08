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
	FILENAME = argv[0];
	int data = -1;
	
	/*----Umgebung anlegen und einbinden----*/
	if (do_ringbuffersize(argc, argv) == -1) return EXIT_FAILURE; /*EXIT_FAILURE gehört zur stdlib.h*/
	
	if (do_semaphorinit() != 0 ) return EXIT_FAILURE;
	
	if (do_sharedmemory() != 0) return EXIT_FAILURE;
	
	
	if (do_attachSM(0) != 0) return EXIT_FAILURE; /*access_mode == 1 --> r&w sonst read only*/
	
	if (ferror(stdin))
    {
        gotanerror("ERROR in stdin");
        do_cleanup();
		return EXIT_FAILURE;
    }
	//while(data!= (int) EOF){
	do{
		
		if((data=do_readSM())==EXIT_FAILURE){
		
			if(errno == ENFILE) gotanerror("ERROR Too many shared memory objects are currently open in the system");
			if(errno == EEXIST) gotanerror("ERROR O_CREAT and O_EXCL are set and the named shared memory object already exists.");
			if(errno == EMFILE) gotanerror("ERROR Too many file descriptors are currently in use by this process.");
			if(errno == EACCES) gotanerror("ERROR The shared memory object exists and the permissions specified by oflag are denied,");
        
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
	}while(data!= EOF);
	//}
	/*----Eigentliche Verarbeitung----*/
	/*
	while(int data = funktion die noch wer schreiben muss und zeichenweise aus dem shared memory liest)!=EOF{
	do{
	
	// Fehlerbehandlung?
		if((data = do_readSM())==EXIT_FAILURE){
			do_cleanup();
			return EXIT_FAILURE;
		}
	// Ausgabe nach stdout
	//TODO: Fehlerbehandlung vollstaendig?
		if(fputc(data, stdout)==EOF){
			if(ferror(stdout)!=0)
			{ 
				do_cleanup();
				return EXIT_FAILURE;		
			}
		}
	
	}while(data!=EOF);
	*/
	if (fflush(stdout) == EOF)
	{
		gotanerror("ERROR fflush");
		return EXIT_FAILURE; 
	}
	// Semaphore und Shared Memory wegräumen
	
	
	//fprintf(stderr, "Error in %s: %s\n", FILENAME, "fgetc() returned error");	//damits kompiliert (FILENAME UNUSED)
	return do_cleanup();;
}
	
/*
 * =================================================================== eof ==
 */

 /**
 * @file empfeanger.h
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
 * --------------------------------------------------------------- defines --
 */

 
#ifndef SHAREDFILE_H
#define SHAREDFILE_H
 
/*
 * -------------------------------------------------------------- includes --
 */





/*
 * --------------------------------------------------------------- globals --
 */
//Wirklich sinnvoll?
extern char *FILENAME; //extern weils im sharedfile.c definiert und in den empfänger und sender c files initialisiert wird
 
/*
 * ------------------------------------------------------------- functions --
 */

int do_ringbuffersize(int argc, char* const argv[]);
int do_semaphorinit(void);	
int do_cleanup(void);
void gotanerror(char *message);
int do_sharedmemory(void);
int do_attachSM(int access_mode); 
int do_writeSM(int data);
int do_readSM(void);
//Was braucht man hier noch??


#endif
/*
 * =================================================================== eof ==
 */
 

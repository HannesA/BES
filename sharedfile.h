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
 * @date 2017/05/21
 *
 * @version 0.1
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

 extern char *FILENAME;
 
/*
 * ------------------------------------------------------------- functions --
 */

int do_ringbuffersize(int argc, char* const argv[]);
int do_semaphorinit(void);	
int do_cleanup(void);
void gotanerror(char *message);



#endif
/*
 * =================================================================== eof ==
 */
 

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
 * @date 2017/06/16
 *
 * @version Abgabe
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
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * --------------------------------------------------------------- globals --
 */
extern char *FILENAME; /*Wird hier definiert und von Sender und Empfaenger gesetzt*/
 
/*
 * -------------------------------------------------------------prototypes--
 */
int do_ringbuffersize(int argc, char* const argv[]);
int do_semaphorinit(void);	
int do_sharedmemory(void);
int do_attachSM(int access_mode); 
int do_writeSM(int data);
int do_readSM(void);
void gotanerror(char *message);
void do_cleanup(void);

#endif
/*
 * =================================================================== eof ==
 */
 

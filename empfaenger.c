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
 * @date 2017/05/21
 *
 * @version 0.1
 *
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */



#include <errno.h>

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
 * \brief Prueft die Aufrufparameter von mypopen
 *
 * Ueberprueft die Aufrufparameter auf Korrektheit und setzt die Errno entsprechend
 *
 * \param *command Pointer auf den command-Parameter
 * \param *type Pointer auf den type-Parameter
 *
 * \return int als Fehlercode
 * \retval 0 wenn Aufrufparameter ok
 * \retval 1 wenn Parameter fehlerhaft
 *
 */
static int docheckinOpen(const char *command, const char *type)
{
	if(myPid!=-1){/*Erneuten Aufruf ohne vorherigen pclose Aufruf abfangen*/
		errno = EAGAIN;
		return 1;
	}
	if(type == NULL || ((type[0] != MODE_READ && type[0] != MODE_WRITE) || type[1] != 0)){	/*Check ob Korrekter Type*/
		errno = EINVAL;			/*errno auf EINVAL setzen -->  "INVALID ARGUMENT"*/
		return 1;
	}	
		
	if(command[0] == '\0' || command == NULL){		/*Check ob Command vorhanden*/
		errno = EINVAL;			/*errno auf EINVAL setzen --> "INVALID ARGUMENT"*/
		return 1;
	}
	return 0;/*Default return Wert*/
}
	
/*
 * =================================================================== eof ==
 */

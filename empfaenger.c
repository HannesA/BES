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
}
	
/*
 * =================================================================== eof ==
 */

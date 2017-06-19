/**
 * @file sem.c
 * Betriebssysteme sem File.
 * Beispiel 3
 *
 * @author Dominik Himmler <ic16b071@technikum-wien.at>
 * @author Alexander Oesterreicher <ic16b011@technikum-wien.at>
 * @author David Sattler <ic16b036@technikum-wien.at>
 * @date 2017/06/16
 *
 * @version 1
 *
 *
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "sem.h"

/**
 *
 * \brief function check_params of sender and empfaenger
 *
 * Function for checking the arguments given to sender and empfaenger and reading the buffersize for the shared memory
 *
 * \param argc number of arguments
 * \param argv given arguments
 *
 * \return buffersize on success, -1 on error
 *
 */
 
long long check_params(int argc, const char* argv[])
{
	int opt;
	long long buffer = 0;
	unsigned long long tmpbuffer = 0;
	char* endptr;
	
	/* Überprüfung, ob kein bzw. zu viele Argumente eingegeben wurden */
	if(argc < 2 || argc > 3)
	{
		fprintf(stderr, "usage: sender -m <ring buffer size>\n");
		return -1;
	}
	
	/* Prüfe, ob -m als Argument angegeben wurde */
	while((opt = getopt(argc, (char* const*)argv, "m:")) != -1)
	{
		/* Argument -m wurde angegeben */
		if(opt == 'm')
		{
			/* Wandle eingegebenen Bufferwert in Dezimalwert um */
			tmpbuffer = strtoull(optarg, &endptr, 10);
			
			/* Prüfe, ob eingegebener Bufferwert > 0 ist */
			if(strncmp(optarg, "-", 1) == 0 || tmpbuffer == 0)
			{
				fprintf(stderr, "%s: ring buffer size must be > 0 and not %lli\n", argv[0], tmpbuffer);
				fprintf(stderr, "usage: sender -m <ring buffer size>\n");
				return -1;
			}
			else
			{
				/* Prüfe, ob ein Bufferwert angegeben wurde */
				if(*endptr != '\0' || argv[optind] != '\0')
				{
					fprintf(stderr, "usage: sender -m <ring buffer size>\n");
					return -1;
				}
				
				/* Prüfe, ob eingegebener Bufferwert zu groß ist */
				if(tmpbuffer > LLONG_MAX || (tmpbuffer * sizeof(int)) > SIZE_MAX)
				{
					fprintf(stderr, "%s: ring buffer size too big\n", argv[0]);
					fprintf(stderr, "usage: sender -m <ring buffer size>\n");
					return -1;
				}
				else
				{
					buffer = tmpbuffer;
				}
			}			
		}
		
		/* Argument != -m wurde angegeben */
		else
		{
			fprintf(stderr, "usage: sender -m <ring buffer size>\n");
			return -1;
		}
	}
	
	/* Gib Buffer zurück */
	return buffer;
}


/**
 *
 * \brief function create_semaphore of sender and empfaenger
 *
 * Function for checking if a semaphore with the given key exists and if it doesn't exist, it creates a new one
 *
 * \param semkey key used for the semaphore
 * \param init initialisation value for the semaphore
 *
 * \return id of the semaphore on success, -1 on error
 *
 */

int create_semaphore(key_t semkey, int init)
{
	int semid;
	
	/* Prüfe, ob bereits ein Semaphore mit dem Key existiert */
	if((semid = semgrab(semkey)) == -1)
	{
		/* Erstelle neuen Semaphore */
		semid = seminit(semkey, 0660, init);
	}
	
	/* Gib ID des Semaphores zurück */
	return semid;
}
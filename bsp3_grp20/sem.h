/**
 * @file sem.h
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

#ifndef SEM_H
#define SEM_H

/* includes */
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
#include <error.h>
#include <limits.h>
#include <sem182.h>
#include <stdint.h>

/* defines */
#define KEY (1000 * getuid())
#define KEY2 (1000 * getuid() + 1)

/* prototypes */
long long check_params(int argc, const char* argv[]);
int create_semaphore(key_t semkey, int init);

#endif
/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>
using namespace std;


# define SEM_KEY           0x55 //can be changed as needed
# define SPACE_SEM_ID      0
# define ITEM_SEM_ID       1 
# define MUTEX_SEM_ID      2

# define TIMEOUT_DURATION  20


union semun {
  int val;               /* used for SETVAL only */
  struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
  ushort *array;         /* used for GETALL and SETALL */
};


int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
int sem_timed_wait (int id, short unsigned int num);
void sem_wait (int, short unsigned int);
void sem_signal (int, short unsigned int);
int sem_close (int);

/* print errors */
void print_conv_err(string str);
void print_limit_err(string str);

/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_timed_wait - Waits on a semaphore (until the time expires)
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

# include "helper.h"

int check_arg (char *buffer)
{
  int i, num = 0, temp = 0;
  if (strlen (buffer) == 0)
    return -1;
  for (i=0; i < (int) strlen (buffer); i++)
    {
      temp = 0 + buffer[i];
      if (temp > 57 || temp < 48)
	return -1;
      num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
    }
  return num;
}

int sem_create (key_t key, int num)
{
  int id;
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0)
    return -1;
  return id;
}


int sem_init (int id, int num, int value)
{
  union semun semctl_arg;
  semctl_arg.val = value;
  if (semctl (id, num, SETVAL, semctl_arg) < 0)
    return -1;
  return 0;
}


void sem_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  semop (id, op, 1);
}


int sem_timed_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  
  const timespec _timeout = {TIMEOUT_DURATION, 0};
  int err = semtimedop (id, op, 1, &_timeout);

  if(err == 0) return 0;
  else return err;
}


void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };

  semop (id, op, 1);
}


int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}


/* print errors */
void print_conv_err(string str){
  fprintf(stderr, "Invalid character input for: %s \n", str.c_str());
}

void print_limit_err(string str){
  fprintf(stderr, "%s must be greater than zero \n", str.c_str());
}

/******************************************************************
 * Producer-consumer problem is a good example of a multi-process 
 * synchronization problem. The problem consists of two types of 
 * processes, namely producer and consumer, who share a common 
 * fixed-size buffer used as a queue. In this program I have used 
 * the concept of concurrent execution using threads for every 
 * consumer and producer. To limit simultaneous access to the queue
 * semaphores have been used, which serve as inter-process signals.
 ****************************************************************** 
 * The program takes 4 command line arguments:
 * 1) queue length
 * 2) nr of jobs per producer
 * 3) nr of of producers
 * 4) nr of consumers
 ******************************************************************
 * Author: Iurie Tarlev
 ******************************************************************/

#include "helper.h"

// producer&consumer functions
void *producer(void *id);

void *consumer(void *id);

// data structure def
struct JobsInfo {
    int jobid;
    int duration;
};

struct ThreadInfo {
    int *buffer_counter;
    int *sem_set_id;
    JobsInfo *Queue;
    int *nr_jobs;
    int *buffer_length;
};

struct AgentThreadInfo {
    int id;
    pthread_t **thread_id;
    ThreadInfo *shared_thread_info;
};

// needed constant values  
# define PRODUCE_TIME      5
# define MAX_JOB_DURATION  10
# define MIN_JOB_DURATION  1


/* ===================== MAIN ===================== */
int main(int argc, char **argv) {
    srand(time(NULL)); //generate random seed
    int buffer_counter = -1;

    //check for correct input
    if (argc < 5) {
        fprintf(stderr, "Insufficient input parameters \n");
        return 0;
    }

    if (argc > 5) {
        fprintf(stderr, "Too many input parameters \n");
        return 0;
    }

    // read 4 command line args
    int buffer_length = check_arg(argv[1]);
    int nr_jobs = check_arg(argv[2]);
    int nr_producers = check_arg(argv[3]);
    int nr_consumers = check_arg(argv[4]);

    if (buffer_length < 0) {
        print_conv_err("Queue length (first argument)");
        return 0;
    }
    if (nr_jobs < 0) {
        print_conv_err("Number of jobs (second argument)");
        return 0;
    }
    if (nr_producers < 0) {
        print_conv_err("Number of producers (third argument)");
        return 0;
    }
    if (nr_consumers < 0) {
        print_conv_err("Number of consumers (fourth argument)");
        return 0;
    }


    if (buffer_length < 1) {
        print_limit_err("Queue length (first argument)");
        return 0;
    }
    if (nr_jobs < 1) {
        print_limit_err("Number of jobs (second argument)");
        return 0;
    }

    int sem_set_id = sem_create(SEM_KEY, 3); //space, item, mutex
    if (sem_set_id < 0) {
        fprintf(stderr, "Failed to set semaphore set key \n");
        return 0;
    }

    // create three semaphores
    int sem1 = sem_init(sem_set_id, SPACE_SEM_ID, buffer_length);
    int sem2 = sem_init(sem_set_id, ITEM_SEM_ID, 0);
    int sem3 = sem_init(sem_set_id, MUTEX_SEM_ID, 1);

    if (sem1 < 0 || sem2 < 0 || sem3 < 0) {
        fprintf(stderr, "One of the semaphores failed to initialise");
        return 0;
    }

    //populate the data structure
    pthread_t producer_thread_id[nr_producers];
    pthread_t consumer_thread_id[nr_producers];

    AgentThreadInfo ProducerThreadInfo[nr_producers];
    AgentThreadInfo ConsumerThreadInfo[nr_consumers];

    ThreadInfo *SharedInfo = new ThreadInfo();
    SharedInfo->buffer_counter = &buffer_counter;
    SharedInfo->sem_set_id = &sem_set_id;
    SharedInfo->Queue = new JobsInfo[buffer_length];
    SharedInfo->nr_jobs = &nr_jobs;
    SharedInfo->buffer_length = &buffer_length;

    int threads_errno = 0;

    //create producer threads

    for (int i = 0; i < nr_producers; i++) {
        ProducerThreadInfo[i].shared_thread_info = SharedInfo;
        ProducerThreadInfo[i].id = i + 1;
        threads_errno = pthread_create(&producer_thread_id[i], NULL,
                                       producer, (void *) &ProducerThreadInfo[i]);
        if (threads_errno != 0) break;
    }

    if (threads_errno == 0)
        for (int i = 0; i < nr_consumers; i++) { //create consumer threads
            ConsumerThreadInfo[i].shared_thread_info = SharedInfo;
            ConsumerThreadInfo[i].id = i + 1;
            threads_errno = pthread_create(&consumer_thread_id[i], NULL, consumer,
                                           (void *) &ConsumerThreadInfo[i]);
            if (threads_errno != 0) break;
        }

    if (threads_errno == 0)
        for (int i = 0; i < nr_producers; i++) { //join producer threads
            threads_errno = pthread_join(producer_thread_id[i], NULL);
            if (threads_errno != 0) break;
        }

    if (threads_errno == 0)
        for (int i = 0; i < nr_consumers; i++) { //join consumer threads
            threads_errno = pthread_join(consumer_thread_id[i], NULL);
            if (threads_errno != 0) break;
        }

    if (threads_errno != 0)
        fprintf(stderr, "Threads error: %s\n", strerror(threads_errno));

    int sem_exit = sem_close(sem_set_id);
    if (sem_exit < 0)
        fprintf(stderr, "Semaphores failed to close");

    //free memory
    delete[] SharedInfo->Queue;
    delete SharedInfo;

    return 0;

}

/* ===================== PRODUCER ===================== */
void *producer(void *Info_ptr) {
    AgentThreadInfo *Info = (AgentThreadInfo *) Info_ptr;

    int sem_set_id = *(Info->shared_thread_info->sem_set_id);
    int *buffer_counter = Info->shared_thread_info->buffer_counter;
    int buffer_length = *(Info->shared_thread_info->buffer_length);
    int nr_jobs = *(Info->shared_thread_info->nr_jobs);
    int duration, jobid;

    for (int i = 0; i < nr_jobs; i++) {

        /* ----- Create Item ----- */
        duration = rand() % MAX_JOB_DURATION + MIN_JOB_DURATION;
        if (buffer_length < nr_jobs)
            jobid = (i) % buffer_length + 1;
        else jobid = i + 1;

        //time to produce a job
        sleep(PRODUCE_TIME);


        /* ----- Semaphore Down ----- */
        if (sem_timed_wait(sem_set_id, SPACE_SEM_ID)) {
            fprintf(stderr,
                    "Producer (%i): Timed out, before it could produce all jobs\n",
                    Info->id);
            pthread_exit(0);
        }
        sem_wait(sem_set_id, MUTEX_SEM_ID);

        /* ----- Critical Section ----- */
        *buffer_counter += 1;

        //place jobid and duration in the queue array
        Info->shared_thread_info->Queue[*buffer_counter].duration = duration;
        Info->shared_thread_info->Queue[*buffer_counter].jobid = jobid;

        /* ----- Semaphore Up ----- */
        sem_signal(sem_set_id, MUTEX_SEM_ID);
        sem_signal(sem_set_id, ITEM_SEM_ID);

        fprintf(stderr, "Producer (%i): Job id %i duration %i\n",
                Info->id, jobid, duration);
    }

    fprintf(stderr, "Producer (%i): No more jobs to generate. \n", Info->id);
    pthread_exit(0);
}


/* ===================== CONSUMER ===================== */
void *consumer(void *Info_ptr) {
    AgentThreadInfo *Info = (AgentThreadInfo *) Info_ptr;
    int sem_set_id = *(Info->shared_thread_info->sem_set_id);
    int buffer_length = *(Info->shared_thread_info->buffer_length);
    int *buffer_counter = Info->shared_thread_info->buffer_counter;
    int duration; // of the item to be fetched
    int jobid;    // of the item to be fetched

    while (1) {
        /* ----- Semaphore Down ----- */
        if (sem_timed_wait(sem_set_id, ITEM_SEM_ID))
            break;

        sem_wait(sem_set_id, MUTEX_SEM_ID);

        /* ----- Critical Section ----- */
        //fetch item
        duration = Info->shared_thread_info->Queue[0].duration;
        jobid = Info->shared_thread_info->Queue[0].jobid;

        //remove item from queue and other items accordingly
        if (buffer_length > 0)
            for (int i = 1; i < buffer_length; i++) {
                Info->shared_thread_info->Queue[i - 1] = \
      Info->shared_thread_info->Queue[i];
            }

        *buffer_counter -= 1;

        /* ----- Semaphore Up ----- */
        sem_signal(sem_set_id, MUTEX_SEM_ID);
        sem_signal(sem_set_id, SPACE_SEM_ID);

        /* ----- Consume item ----- */
        fprintf(stderr, "Consumer (%i): Job id %i executing sleep duration %i\n",
                Info->id, jobid, duration);
        sleep(duration);
        fprintf(stderr, "Consumer (%i): Job id %i completed \n", Info->id, jobid);
    }

    fprintf(stderr, "Consumer (%i): No more jobs left (timeout) \n", Info->id);
    pthread_exit(0);
}


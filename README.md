# Producer Consumer Problem

Producer-consumer problem is a good example of a multi-process 
synchronization problem. The problem consists of two types of 
processes, namely producer and consumer, who share a common 
fixed-size buffer used as a queue. In this program I have used 
the concept of concurrent execution using threads for every 
consumer and producer. To limit simultaneous access to the queue
semaphores have been used, which serve as inter-process signals.

 The program takes 4 command line arguments:
 1) queue length
 2) nr of jobs per producer
 3) nr of of producers
 4) nr of consumers

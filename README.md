# Producer Consumer Problem

This is an operating system excercise, with the focus on learning
how semaphores work in ensuring atomicity when multiple threads are executed at the same time. The producers are creating jobs and consumers
are consuming them, but there should be mutual exclusion ensuring 
that none of the entities enter the critical section at the same time,
furthermore there is also a semaphore established to restrict access, 
if the queue is full.
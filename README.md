# Linked-List
This repository contains the implementation of a linked list using serial and parallel programming.

1. Compile the files using the following commands.

 	- serial.c 	: 	gcc -g -Wall -o serial serial.c
 	- mutex.c 	: 	gcc -g -Wall -o mutex mutex.c -pthread -lm
	- rw_lock.c 	: 	gcc -g -Wall -o rw_lock rw_lock.c -pthread -lm

2. Run the files using the follwing commands

	- serial.c 	:	**./serial <n> <m> <mMember> <mInsert> <mDelete> <sample size>**
	- mutex.c 	:	./mutex  <thread_count> <n> <m>  <mMember> <mInsert> <mDelete> <sample size>
	- rw_lock.c 	:	./rwlock <thread_count> <n> <m> <mMember> <mInsert> <mDelete> <sample size>

- *n is the number of initial unique values in the Link List.
- *m is number of random Member, Insert, and Delete operations on the link list.
- *mMember is the fractions of operations of Member operation.
- *mInsert is the fractions of operations of Insert operation.
- *mDelete is the fractions of operations of Delete operation.
- *sample_size is the number of iterations the main code is run.

3. After successful run, program will display 
	- the execution time of each iteration 
	- the average execution time 
	- the standard deviation 

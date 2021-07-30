/*
Author: W.A.S.D.Abeygunawardhana

Compile:  gcc -g -Wall -o mutex mutex.c -pthread -lm

Run:./mutex <number of threads> <n> <m> <mMember> <mInsert> <mDelete> <sample_size>
*           n is the number of initial unique values in the Link List.
*           m is number of random Member, Insert, and Delete operations on the link list.
*           mMember is the fractions of operations of Member operation.
*           mInsert is the fractions of operations of Insert operation.
*           mDelete is the fractions of operations of Delete operation.
*			sample_size is the number of iterations the main code is run
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

struct list_node_s
{
	int data;
	struct list_node_s* next;
};
struct list_node_s* head;

const int MAX_THREADS = 1024;

long thread_count;
pthread_mutex_t mutex;

int n;
int m;
float mMember;
float mInsert;
float mDelete;
float mMember_count = 0.0;
float mInsert_count = 0.0;
float mDelete_count = 0.0;
int count_member;
int count_insert;
int count_delete;

//the sufficient number of samples for the performance results to be within an accuracy of ±5% and 95 % confidence level
int sample_size;

// main three operations
int Member(int value, struct  list_node_s* head_p);
int Insert(int value, struct list_node_s** head_pp);
int Delete (int value, struct list_node_s** head_pp);

//thread function
void* thread_oparation(void* rank);

//other functions

void Get_args(int argc, char* argv[]);



/* Main function */
int main(int argc, char* argv[])
{
	int i = 0;
	long       thread;  /* Use long in case of a 64-bit system */
	pthread_t* thread_handles;
	struct timespec start, finish;
	double elapsed;
	double sum_elapsed = 0.0;
	double avg = 0.0;
	double std = 0.0;

	// read command line arguments 
	Get_args(argc, argv);

	//calcutaing opration counts from the input fractions
	mMember_count = mMember * m;
	mInsert_count = mInsert * m;
	mDelete_count  = mDelete * m;

	//an array to store time for each iteration
	double time_array[sample_size];

	int iter = 0;
	while (iter < sample_size){

		struct list_node_s* head = NULL;

		count_member = 0;
		count_insert = 0;
		count_delete = 0;
	
		//initially populating the link list
		for (i = 0; i < n; i++)
		{
			int ins_value = rand() % 65536; //value should be between 2^16 - 1

			//insert unique values
			if (!Insert(ins_value, &head)){
				i--;
			}
		}

		thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));

		// initialize the mutex
		pthread_mutex_init(&mutex, NULL);

		//start clock
		clock_gettime(CLOCK_MONOTONIC, &start);

		//create threads
		for (thread = 0; thread < thread_count; thread++)
		{
			pthread_create(&thread_handles[thread], NULL, thread_oparation, (void*)thread);
		}

		//join child threads to the main thread
		for (thread = 0; thread < thread_count; thread++)
		{
			pthread_join(thread_handles[thread], NULL);
		}

		// end clock
		clock_gettime(CLOCK_MONOTONIC, &finish);

		//destroy the mutex
		pthread_mutex_destroy(&mutex);

		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;//ensure that precision is not lost when timing very short intervals

		printf("Elapsed time = %.6f seconds\n", elapsed);

		//storing values for further calculations
		sum_elapsed += elapsed;
		time_array[iter] = elapsed;

		iter++;

	}

	//calculating average
	avg = sum_elapsed / sample_size;
	printf("Average Time = %.10f seconds\n", avg);

	//calculating standard deviation
	double diff;
	for (i = 0; i < sample_size; ++i) {
		diff += pow(time_array[i] - avg, 2);
	}

	std = sqrt(diff / sample_size);// dividing by (sample size-1) bcs the sample size < 50
	printf("Std = %.10f \n", std);


	return 0;
}/*main*/


/*Thread Function*/
void* thread_oparation(void* rank)
{
	int local_m = m / thread_count; // dividing the operations equality to threads
	int count = 0; 

	int member_completed = 0;
	int insert_completed = 0;
	int delete_completed = 0;

	while (count < local_m) // as local_m is local for each thread, it is compared with the local variable 'count'
	{
		int random_value = rand() % 65536;
		int random_select = rand() % 3;

		//member operations
		if (random_select == 0 && member_completed == 0){

			//lock the mutex here as we are accessing the global variable count_member and the function Member()
			pthread_mutex_lock(&mutex);

			if (count_member < mMember_count){
				Member(random_value, head);
				count_member++; //increament the global variable 
				count++;  //increament the local variable to get the total operations done by each thread
			}
			else{
				member_completed = 1;//this variable is used to reduce unwanted mutex locking
			}

			pthread_mutex_unlock(&mutex);
		
		}

		//insert operations
		else if (random_select == 1 && insert_completed == 0){

			pthread_mutex_lock(&mutex);

			if (count_insert< mInsert_count){
				Insert(random_value, &head);
				count_insert++;
				count++;
			}
			else{
				insert_completed = 1;
			}

			pthread_mutex_unlock(&mutex);

		}

		//delete operations
		else if (random_select == 2 && delete_completed == 0){

			pthread_mutex_lock(&mutex);

			if (count_delete< mDelete_count){
				Delete(random_value, &head);
				count_delete++;
				count++;
			}
			else{
				delete_completed = 1;
			}

			pthread_mutex_unlock(&mutex);

		}

		// count is incremented inside the if clauses to make sure it is incremented only when an operation is executed.
	
	}

	return NULL;
}  



/* Member Function */
int Member(int value, struct  list_node_s* head_p)
{
	struct list_node_s* curr_p = head_p;

	while (curr_p != NULL && curr_p->data < value)
	{
		curr_p = curr_p->next;
	}

	if (curr_p == NULL || curr_p->data > value)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}/* member */


/*Insert Function*/
int Insert(int value, struct list_node_s** head_pp)
{
	struct list_node_s* curr_p = *head_pp;
	struct list_node_s* pred_p = NULL;
	struct list_node_s* temp_p = NULL;

	while (curr_p != NULL && curr_p->data < value)
	{
		pred_p = curr_p;
		curr_p = curr_p->next;
	}

	if (curr_p == NULL || curr_p->data > value)
	{
		temp_p = malloc(sizeof(struct list_node_s));
		temp_p->data = value;
		temp_p->next = curr_p;

		if (pred_p == NULL) //new first node
		{
			*head_pp = temp_p;
		}
		else
		{
			pred_p->next = temp_p;
		}
		return 1;

	}
	else  //value already exists
	{
		return 0;
	}
}   /*insert*/


/* Delete Function*/
int Delete (int value, struct list_node_s** head_pp)
{
	struct list_node_s* curr_p = *head_pp;
	struct list_node_s* pred_p = NULL;

	while (curr_p != NULL && curr_p->data < value)
	{
		pred_p = curr_p;
		curr_p = curr_p->next;
	}

	if (curr_p != NULL && curr_p->data == value)
	{
		if (pred_p == NULL){  //deleting head node
			*head_pp = curr_p->next;
			free(curr_p);
		}
		else
		{
			pred_p->next = curr_p->next;
			free(curr_p);
		}
		return 1;

	}
	else //value is not in the list
	{
		return 0;
	}

}   /*delete*/


/*Other Functions*/

void Get_args(int argc, char* argv[]) {

	//validate the number of arguments
	if (argc != 8)
	{
		fprintf(stderr, "error in input format. correct format: <number of threads> <n> <m> <mMember> <mInsert> <mDelete> <sample_size>\n");
		exit(0);
	}

	//get thread count and validate
	thread_count = strtol(argv[1], NULL, 10);

	if (thread_count <= 0 || thread_count > MAX_THREADS)
	{
		fprintf(stderr, "error. thread count should be greater than 0\n");
		exit(0);
	}

	//declaring the arguments
	n = (int)strtol(argv[2], (char **)NULL, 10);
	m = (int)strtol(argv[3], (char **)NULL, 10);

	mMember = (float)atof(argv[4]);
	mInsert = (float)atof(argv[5]);
	mDelete = (float)atof(argv[6]);

	sample_size = (int)atof(argv[7]);

	float total = mMember + mInsert + mDelete;

	if (n <= 0 || m <= 0 || mMember < 0 || mInsert < 0 || mDelete < 0 || total != 1.0) {
		//error
		fprintf(stderr, "error in input values.\n");
		fprintf(stderr, "n: no of initial values in the linkedlist.\n");
		fprintf(stderr, "m: no. of total random operations on the linked list\n");
		fprintf(stderr, "mMember: fraction of operations for member function\n");
		fprintf(stderr, "mInsert: fraction of operations for insert function\n");
		fprintf(stderr, "mDelete: fraction of operations for delete function\n");
		exit(0);
	};

}  /* Get_args */


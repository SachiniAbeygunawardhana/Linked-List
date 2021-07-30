/*
Author: W.A.S.D.Abeygunawardhana

Compile:  gcc -g -Wall -o serial serial.c 

Run:./serial <n> <m> <mMember> <mInsert> <mDelete> <sample_size>
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
#include <time.h>

struct list_node_s
{
	int data;
	struct list_node_s* next;
};

const int MAX_THREADS = 1024;

long thread_count;


int n;
int m;
float mMember;
float mInsert;
float mDelete;
float mMember_count = 0.0;
float mInsert_count = 0.0;
float mDelete_count = 0.0;

//the sufficient number of samples for the performance results to be within an accuracy of ±5% and 95 % confidence level
int sample_size;

// main three operations
int Member(int value, struct  list_node_s* head_p);
int Insert(int value, struct list_node_s** head_pp);
int Delete(int value, struct list_node_s** head_pp);


//other functions

void Get_args(int argc, char* argv[]);



/* Main function */
int main(int argc, char* argv[])
{
	int i = 0;
	struct timespec start, finish;
	double elapsed;
	double sum_elapsed = 0.0 ;
	double avg = 0.0;
	double std = 0.0;

	// read command line arguments 
	Get_args(argc, argv);

	//calcutaing opration counts from the input fractions
	mMember_count = mMember * m;
	mInsert_count = mInsert * m;
	mDelete_count = mDelete * m;
	
	//an array to store time for each iteration
	double time_array[sample_size];

	int iter = 0;
	while (iter < sample_size){

		//printf(" %i \n", iter);

		struct list_node_s* head = NULL;

		int count_member = 0;
		int count_insert = 0;
		int count_delete = 0;

		//initially populating the link list
		for (i = 0; i < n; i++)
		{
			int ins_value = rand() % 65536; //value should be between 2^16 - 1

			//insert unique n values
			if (!Insert(ins_value, &head)){
				i--;
			}
		}

		//start clock
		clock_gettime(CLOCK_MONOTONIC, &start);

		int count = 0;
		while (count < m)
		{

			int random_value = rand() % 65536;
			int random_select = rand() % 3;

			if (random_select == 0 && count_member<mMember_count)
			{

				Member(random_value, head);
				count_member++;

			}
			else if (random_select == 1 && count_insert< mInsert_count)
			{

				Insert(random_value, &head);
				count_insert++;

			}
			else if (random_select == 2 && count_delete< mDelete_count)
			{

				Delete(random_value, &head);
				count_delete++;

			}
			count = count_member + count_insert + count_delete;
		}

		// end clock
		clock_gettime(CLOCK_MONOTONIC, &finish);

		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;//ensure that precision is not lost when timing very short intervals

		printf("Execution Time = %.6f seconds\n", elapsed);

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

	std = sqrt(diff / sample_size ); 
	printf("Std = %.10f \n", std);


	return 0;
}/*main*/


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
int Delete(int value, struct list_node_s** head_pp)
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
	if (argc != 7)
	{
		fprintf(stderr, "error in input format. correct format: <n> <m> <mMember> <mInsert> <mDelete> <sample_size>\n");
		exit(0);
	}


	//declaring the arguments
	n = (int)strtol(argv[1], (char **)NULL, 10);
	m = (int)strtol(argv[2], (char **)NULL, 10);

	mMember = (float)atof(argv[3]);
	mInsert = (float)atof(argv[4]);
	mDelete = (float)atof(argv[5]);

	sample_size = (int)atof(argv[6]);

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


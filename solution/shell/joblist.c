/*
 * joblist.c
 * Author: Zach Kissel
 * Date: 2/8/15
 * Course: CSC3320
 * Description: This file provides an implementation of the joblist
 * 		data structure as described in the joblist interface
 * 		joblist.h
 */
#include <stdio.h>
#include <stdlib.h> /* For malloc(3). */
#include <errno.h>  /* For error numbers. */
#include <string.h> /* For string manipulation operations. */
#include <assert.h> /* For assert(3). */
#include <unistd.h>

#include <joblist.h>

/* Node Type */
struct node
{
	pid_t pid;		/* The string command to display. */
	struct node* next; 	/* Pointer to the next node. */
};

/* List Type */
typedef struct list
{
	int size;		/* Stores the size of the list. */
	struct node* head;	/* The head of the list. */
	struct node* tail;	/* The tail of the list. */
} list_t;

/*
 * joblist_create
 * Description: Allocates a new joblist structure and returns
 * 		a handle to the joblist.
 * Pre: max_size > 0.
 * Post: A new list object completely initialized is returned. In
 * 	 the case of failure, NULL is returned.
 */
joblist_hdl_t* joblist_create(int max_size)
{
	list_t *new_list = NULL;
	
	assert(max_size > 0);	/* Make sure list upper bound is > 0. */

	/* Allocate the new list as a zeroed section of memory. */
	new_list = (list_t*) calloc(1, sizeof (list_t));
	
	return (joblist_hdl_t *)new_list;
}

/*
 * joblist_add
 * Description: Adds a new string to the end of the tail of the joblist.
 * Pre: the cmd is not NULL.
 * Post: Returns 0 on success and non-zero on failure. In particular,
 * 	 ENOMEM - allocation failed.
 * Side-Effects: The memory needed to store cmd is allocated by this
 * 		 routine.
 */ 
int joblist_add_job(joblist_hdl_t *hdl, pid_t pid)
{
	/* Cast the handle to the correct type. */
	list_t *list = (list_t  *)hdl;

	/* Allocate a new node to hold the new job. */
	struct node* new_node = (struct node*) calloc(1, sizeof (pid_t));

	assert (hdl != NULL);	/* Handle can't be null! */

	/* If the new node could not be allocated return ENOMEM. */
	if (new_node == NULL)
		return ENOMEM;

	/* Assign the the pid to the new node. */
	new_node->pid = pid;

	/* Add to the head of the list if our list is empty */
	if (list->head == NULL)
	{
		list->head = new_node;
		list->tail = list->head;
	}
	else	/* Append to the tail. */
	{
		list->tail->next = new_node;
		list->tail = list->tail->next;
	}

	/* Update the size of the list */
	list->size++;

	/* At this point everything was successful, return 0. */
	return 0;
}

/*
 * joblist_destroy
 * Description: This function destroys a list. 
 * Pre: A non-null list handle
 * Post: the list handle is invalidated.
 */
void joblist_destroy(joblist_hdl_t *hdl)
{
	list_t *list = (list_t *)hdl;

	if (list->size > 0)
		joblist_empty(hdl);
	free(list);	/* Detroy the structure. */
	list = NULL;
}

/*
 * joblist_print
 * Description: This function displays the contents of the 
 * 		joblist. One command per line from head to tail.
 * Pre: hdl is not NULL.
 * Post: none.
 */
void joblist_print(joblist_hdl_t *hdl)
{
	list_t *list = (list_t *)hdl;
	struct node* walker = NULL;

	assert(hdl != NULL);	/* The list handle can not be NULL. */

	/* 
	 * Walk the list displaying each command on a 
	 * line by themselves.
	 */
	for(walker = list->head; walker != NULL; walker = walker->next)
		printf("PID: %d\n", walker->pid);
}

/*
 * joblist_remove_job
 * Description: This function searches through the list and
 * 		removes the job being tracked.
 * Pre: hdl != NULL, and pid != 0.
 * Post: None.
 */
void joblist_remove_job(joblist_hdl_t* hdl, pid_t key_pid)
{
	list_t *list = (list_t *)hdl;
	struct node *prev = NULL;
	struct node *curr = list->head;

	assert(hdl != NULL);	/* The hdl can't be NULL */
	assert(key_pid != 0);	/* The PID can't be zero. */

	/* Find the job to remove. */
	for (curr = list->head; curr != NULL && curr->pid != key_pid;
	    curr = curr->next)
		prev = curr;
	
	/* Remove the necessary node. */
	if (curr != NULL)
	{
		list->size--;	/* Shrink the size of the list. */

		/* We are removing from the head of the list. */
		if (prev == NULL)
		{
			if (list->head == list->tail)
				list->tail = list->head->next;
			list->head = list->head->next;
			free (curr);
			return;
		}

		/* Handle the tail assignemnt. */
		if (list->tail == curr)
			list->tail = prev;

		/* 
		 * hook the previous nodes next pointer to
 		 * to the node after curr.
 		 */
		prev->next = curr->next;
	
		/* Free the allocated memory. */	
		free(curr);
	}
}

/*
 * joblist_empty
 * Description: This funciton empties the entire joblist without
 * 		freeing the handle.
 * Pre: hdl != NULL.
 * Post: None.
 */
void joblist_empty(joblist_hdl_t* hdl)
{
	list_t *list = (list_t *)hdl;
	struct node* walker = list->head;
	struct node* next = NULL;

	assert(hdl != NULL);	/* The handle can not be NULL. */

	/* The list is empyt don't try to empyt it. */
	if (joblist_is_empty(hdl))
		return;

	/* 
 	 * Walk the list removing each element as
	 * we traverse.
	 */
	while (walker != NULL)
	{
		next = walker->next;
		free(walker);
		list->size--;	/* Shrink the size of the list. */
		walker = next;
	}
}

/*
 * joblist_is_empty
 * Description: Checks to see if the joblist is empyt.
 * Pre: hdl is not NULL.
 * Post: 1 is returned if the joblist is empty and 0
 *       otherwise.
 */
int joblist_is_empty(joblist_hdl_t *hdl)
{
	assert(hdl != NULL);

	return (((list_t *)hdl)->size == 0);
}

/*
 * history.c
 * Author: Zach Kissel
 * Date: 2/8/15
 * Course: CSC3320
 * Description: This file implements a history data structure as
 * 		defined by the interface in history.h
 */
#include <stdio.h>
#include <stdlib.h> /* For malloc(3). */
#include <errno.h>  /* For error numbers. */
#include <string.h> /* For string manipulation operations. */
#include <assert.h> /* For assert(3). */

#include <history.h>

/* Node Type */
struct node
{
	char *cmd;		/* The string command to display. */
	struct node* next; 	/* Pointer to the next node. */
};

/* List Type */
typedef struct list
{
	int size;		/* Stores the size of the list. */
	int max_size;		/* Stores the maximum size of the history */
	struct node* head;	/* The head of the list. */
	struct node* tail;	/* The tail of the list. */
} list_t;

/*
 * history_create
 * Description: Allocates a new history structure and returns
 * 		a handle to the history.
 * Pre: max_size > 0.
 * Post: A new list object completely initialized is returned. In
 * 	 the case of failure, NULL is returned.
 */
history_hdl_t* history_create(int max_size)
{
	list_t *new_list = NULL;
	
	assert(max_size > 0);	/* Make sure list upper bound is > 0. */

	/* Allocate the new list as a zeroed section of memory. */
	new_list = (list_t*) calloc(1, sizeof (list_t));
	
	/* Store the limit of the list size. */
	if (new_list != NULL)
		new_list->max_size = max_size;

	return (history_hdl_t *)new_list;
}

/*
 * history_add
 * Description: Adds a new string to the end of the tail of the history.
 * Pre: the cmd is not NULL.
 * Post: Returns 0 on success and non-zero on failure. In particular,
 * 	 ENOMEM - allocation failed.
 * Side-Effects: The memory needed to store cmd is allocated by this
 * 		 routine.
 */ 
int history_add(history_hdl_t *hdl, char* cmd)
{
	/* Cast the handle to the correct type. */
	list_t *list = (list_t  *)hdl;
	struct node* rem_node = NULL;	

	/* Allocate a new node to hold the new command. */
	struct node* new_node = (struct node*) calloc(1, sizeof (struct node));

	assert (hdl != NULL);	/* Handle can't be null! */

	/* If the new node could not be allocated return ENOMEM. */
	if (new_node == NULL)
		return ENOMEM;
	/* 
 	 * Allocate the memory for the command and then
 	 * copy the message into the list.
 	 */
	if ((new_node->cmd = (char *)calloc(strlen(cmd) + 1, 
	    sizeof (char))) == NULL)
		return ENOMEM;

	/* Copy the cmd into the newly allocated memory. */
	strncpy(new_node->cmd, cmd, strlen(cmd));

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

	/* 
	 * Enforce the list size by removing the oldest command
 	 * which is at the head of the list.
 	 */
	if (list->size > list->max_size)
	{
		/* Don't loose the next link. */
		rem_node = list->head;
		list->head = list->head->next;

		/* Free the memory associated with the head. */
		free(rem_node->cmd);
		free(rem_node);
		rem_node->cmd = NULL;
		rem_node = NULL;
	}
		
	/* At this point everything was successful, return 0. */
	return 0;
}

/*
 * history_destroy
 * Description: This function destroys a list. 
 * Pre: A non-null list handle
 * Post: the list handle is invalidated.
 */
void history_destroy(history_hdl_t *hdl)
{
	list_t *list = (list_t *)hdl;
	struct node* walker = list->head;
	struct node* next = NULL;

	assert(hdl != NULL);	/* The handle can not be NULL. */

	/* 
 	 * Walk the list removing each element as
	 * we traverse.
	 */
	while (walker != NULL)
	{
		next = walker->next;
		free(walker->cmd);
		free(walker);
		walker = next;
	}

	free(list);	/* Detroy the structure. */
}

/*
 * history_print
 * Description: This function displays the contents of the 
 * 		history. One command per line from head to tail.
 * Pre: hdl is not NULL.
 * Post: none.
 */
void history_print(history_hdl_t *hdl)
{
	list_t *list = (list_t *)hdl;
	struct node* walker = NULL;

	assert(hdl != NULL);	/* The list handle can not be NULL. */

	/* 
	 * Walk the list displaying each command on a 
	 * line by themselves.
	 */
	for(walker = list->head; walker != NULL; walker = walker->next)
		printf("%s\n", walker->cmd);
}

/*
 * history_get_cmd
 * Description: This function returns the entry-th command to the
 * 		caller.
 * Pre: hdl != NULL.
 * Post: NULL or the entry-th command.
 */
char* history_get_cmd(history_hdl_t *hdl, int entry)
{
	list_t *list = (list_t *)hdl;
	struct node* walker = NULL;
	int curr_node_num = 1;

	assert(hdl != NULL);	/* The list handl can not be NULL. */

	/* Make sure the entry number is valid. */
	if (entry > list->size || entry < 1)
		return NULL;

	/* Walk the list looking for the entry. */
	for (walker = list->head; walker != NULL && curr_node_num != entry;
	    walker = walker->next)
		curr_node_num++;

	/* We have found the command return it. */
	if (walker != NULL)
		return walker->cmd;
	
	/* No command is found, return NULL. */
	return NULL;
}

/*
 * history_get_last_cmd
 * Description: This function returns the last command added to the list.
 * Pre: hdl != NULL
 * Post: The command that was last added to the list.
 */
char* history_get_last_cmd(history_hdl_t *hdl)
{
	list_t *list = (list_t *)hdl;

	assert(hdl != NULL);	/* The handle can not be NULL. */

	return list->tail->cmd;
}

/*
 * history_is_empty
 * Description: This function deterines if a history is empty
 * 		or not.
 * Pre: hdl != NULL
 * Post: 1 is returned if the list is not empty and 0 otherwise.
 */
int history_is_empty(history_hdl_t *hdl)
{
	assert(hdl != NULL);	/* Make sure the list is not empty. */
	return (((list_t *)hdl)->size == 0);
}

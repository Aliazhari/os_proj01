/*
 * joblist.h
 * Author: Zach Kissel
 * Date: 2/8/15
 * Course: CSC3320
 * Description: This file provides a joblist data structure.
 */
#ifndef __JOBLIST_H
#define __JOBLIST_H

#include <unistd.h>

typedef void* joblist_hdl_t;

joblist_hdl_t* joblist_create();
int joblist_add_job(joblist_hdl_t*, int);
void joblist_destroy(joblist_hdl_t*);
void joblist_print(joblist_hdl_t*);
void joblist_remove_job(joblist_hdl_t*, pid_t);
void joblist_empty(joblist_hdl_t*);
int joblist_is_empty(joblist_hdl_t*);

#endif	/* __JOBLIST_H */

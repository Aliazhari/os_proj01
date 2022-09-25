/*
 * history.h
 * Author: Zach Kissel
 * Date: 2/8/15
 * Course: CSC3320
 * Description: This file provides an interface for a
 * 		history data structure.
 */
#ifndef __HISTORY_H
#define __HISTORY_H

typedef void* history_hdl_t;

history_hdl_t* history_create(int);
int history_add(history_hdl_t*, char*);
void history_destroy(history_hdl_t*);
void history_print(history_hdl_t*);
char* history_get_cmd(history_hdl_t*, int);
char* history_get_last_cmd(history_hdl_t*);
int history_is_empty(history_hdl_t*);
#endif	/* __HISTORY_H */

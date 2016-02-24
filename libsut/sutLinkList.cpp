#include <stdio.h>
#include <stdlib.h>
#include "sutDataStruct.h"
/* ------------------------------------------------------------------------------ */
int sutLinkingList::add(void *src_data, unsigned int src_key)
{
	int ret =0;

	lock();
	if (!is_full()) {
        sutLinkingListNode *tmp = (sutLinkingListNode *)malloc(sizeof(sutLinkingListNode));
		tmp->ID = ID_acc;
		tmp->data = src_data ;
        tmp->Key = src_key;
		tmp->next = NULL;

		if(head_node == NULL && tail_node == NULL) {
			tmp->prev = NULL;
			head_node = tmp;
		}
		else {
			tmp->prev = tail_node;
			tail_node->next = tmp ;
		}
		tail_node = tmp ;
		cost++ ;
		ID_acc++;
		ret = tmp->ID ;
	}
	unlock();
	return ret ;
}
/* ------------------------------------------------------------------------------ */
inline void sutLinkingList::remove_node(sutLinkingListNode *src_node)
{
	if(src_node->next != NULL) 
		src_node->next->prev = src_node->prev;
	else
		tail_node = src_node->prev;

	if(src_node->prev != NULL) 
		src_node->prev->next = src_node->next;
	else
		head_node = src_node->next;
}
/* ------------------------------------------------------------------------------ */
int sutLinkingList::remove_by_ID(int node_ID)
{
	int ret_value =0;
    sutLinkingListNode *tmp = head_node;

	lock();

	/* search */
	while(tmp != NULL) {
		if(tmp->ID == node_ID) break;
		tmp = tmp->next;
	}

	/* remove */
	if(tmp != NULL) {
		remove_node(tmp);
		free(tmp);
		ret_value =1;
		cost --;
	}
	unlock();
	return ret_value;
}
/* ------------------------------------------------------------------------------ */
int sutLinkingList::remove_by_key(unsigned int node_key)
{
	int ret_value =0;
    sutLinkingListNode *tmp = head_node;

	lock();

	/* search */
	while(tmp != NULL) {
        if(tmp->Key == node_key) break;
		tmp = tmp->next;
	}

	/* remove */
	if(tmp != NULL) {
		remove_node(tmp);
		free(tmp);
		ret_value =1;
		cost--;
	}
	unlock();
	return ret_value;
}
/* ------------------------------------------------------------------------------ */
int sutLinkingList::remove_by_sequence(int sequence)
{
	int ret_value =0;
    sutLinkingListNode *tmp = head_node;

	lock();

	/* search */
	while(tmp != NULL && sequence >0) {
		tmp = tmp->next;
		sequence--;
	}

	/* remove */
	if(tmp != NULL) {
		remove_node(tmp);
		free(tmp);
		ret_value =1;
		cost--;
	}
	unlock();
	return ret_value;
}
/* ------------------------------------------------------------------------------ */
void *sutLinkingList::fetch_by_key(unsigned int node_key)
{
	void *ret =NULL;
    sutLinkingListNode *tmp =head_node;

	lock();
	while(tmp != NULL) {
        if(tmp->Key == node_key) break;
		tmp = tmp->next;
	}
	if(tmp != NULL) {
		remove_node(tmp);
		ret = tmp->data;
		free(tmp);
	}
	unlock();
	return ret;
}
/* ------------------------------------------------------------------------------ */
void *sutLinkingList::search_by_sequence(int sequence)
{
    sutLinkingListNode *tmp =head_node;

	lock();
	while(tmp != NULL && sequence >0) {
		tmp = tmp->next;
		sequence--;
	}
	unlock();
	if(tmp != NULL)
		return tmp->data;
	else
		return tmp;
}
/* ------------------------------------------------------------------------------ */
void *sutLinkingList::search_by_ID(int node_ID)
{
    sutLinkingListNode *tmp =head_node;

	lock();
	while(tmp != NULL) {
		if(tmp->ID == node_ID) break;
		tmp = tmp->next;
	}
	unlock();
	if(tmp != NULL)
		return tmp->data;
	else
		return tmp;
}
/* ------------------------------------------------------------------------------ */
void *sutLinkingList::search_by_key(unsigned int node_key)
{
    sutLinkingListNode *tmp =head_node;

	lock();
	while(tmp != NULL) {
        if(tmp->Key == node_key) break;
		tmp = tmp->next;
	}
	unlock();
	if(tmp != NULL)
		return tmp->data;
	else
		return tmp;
}
/* ------------------------------------------------------------------------------ */
/* Requset */
/* ------------------------------------------------------------------------------ */
int sutLinkingList::count()
{
	int tmp;
	lock();
	tmp = cost ;
	unlock();
	return tmp ;
}
/* ------------------------------------------------------------------------------ */
inline void sutLinkingList::lock()
{
	if(pthread_mutex_lock(&mutex) != 0) {
        printf( "Lock error\n");
	}
}
/* ------------------------------------------------------------------------------ */
inline void sutLinkingList::unlock()
{
	pthread_mutex_unlock(&mutex);
}
/* ------------------------------------------------------------------------------ */
int sutLinkingList::is_empty()
{
	return (head_node == NULL && tail_node == NULL) ; // 0 or 1
}
/* ------------------------------------------------------------------------------ */
int sutLinkingList::is_full()
{
	return (capacity !=0) & (cost >= capacity) ; // 0 or 1
}
/* ------------------------------------------------------------------------------ */
sutLinkingList::sutLinkingList()
{
	ID_acc =0;
	cost =0;
	capacity =0;
	tail_node = NULL;
	head_node = NULL;
}
/* ------------------------------------------------------------------------------ */
sutLinkingList::~sutLinkingList()
{
	

}
/* ------------------------------------------------------------------------------ */
void sutLinkingList::init(int list_capacity)
{
	ID_acc =1;	/* ID is started from 1 */
	cost =0;
	capacity = list_capacity;
	tail_node = NULL;
	head_node = NULL;
	pthread_mutexattr_t mattr ;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK_NP);

	if(pthread_mutex_init(&mutex, &mattr) !=0) {
        printf("init error\n");
	}
}
/* ------------------------------------------------------------------------------ */
void sutLinkingList::release()
{
	lock();
	while (head_node != NULL) {
        sutLinkingListNode *tmp = head_node ;
		head_node = tmp->next ;
		free(tmp);
	}
	unlock();
}

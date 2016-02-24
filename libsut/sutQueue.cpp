#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sutDataStruct.h"
/* ------------------------------------------------------------------------------ */
#define C_NONE "\033[m"
#define C_GREEN "\033[0;32;32m"
#define C_RED "\033[0;32;31m"

#define P_FATAL C_RED "Fatal" C_NONE " : "
#define P_INFO C_GREEN "Info" C_NONE " : "
/* ------------------------------------------------------------------------------
 * Queue as Storage type [or called buffering queue]
 */
/* ------------------------------------------------------------------------------ */
sutPoolQueue::sutPoolQueue()
{
	Cost =0;
	QueueCapacity = 0;
	PageCapacity = 0;
	PageCount =0;
	RecyclePage = NULL;
	StorePage = NULL;
	Store_ptr = NULL;
	Fetch_ptr = NULL;
}
/* ------------------------------------------------------------------------------ */
sutPoolQueue::~sutPoolQueue()
{
	release();
}
/* ------------------------------------------------------------------------------ 
 * Init the queue
 *		@param mode : choose the mode of current queue.
 *						>ModeStatic  : The capacity of 
						>ModeDynamic :
 *		@param capacity
 */
int sutPoolQueue::init(int mode, int capacity)
{
	int ret =1;

	switch(mode) {
	case ModeStatic:
		if(capacity > 0) return 0;
		PageCapacity = capacity;
		break;
	case ModeAuto:
		PageCapacity = 1024;
		break;
	default :
		printf(P_FATAL "sutPoolQueue init failed\n");
		return 0;
		break;
	}

	/* alloc page and node */
	Cost =0;
	PageCount =1;
	RecyclePageCount =0;
	QueueCapacity = PageCount * PageCapacity;
	StorePage = (struct sutCarrierPageNode *)malloc(sizeof(struct sutCarrierPageNode));
	if(StorePage == NULL) {
		printf(P_FATAL "Alloc page node failed\n");
		return 0;
	}
	StorePage->page = (void **)malloc(sizeof(void *) *PageCapacity);
	if(StorePage->page == NULL) {
		printf(P_FATAL "Alloc page failed\n");
		free(StorePage);
		return 0;
	}
	StorePage->end = StorePage->page + PageCapacity;
	StorePage->next = NULL;
	FetchPage = StorePage;
	Store_ptr = StorePage->page;
	Fetch_ptr = StorePage->page;
	RecyclePage = NULL;

	/* init mutex */
	pthread_mutexattr_t mattr ;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK_NP);
	if(pthread_mutex_init(&mutex, &mattr) !=0) {
		printf(P_FATAL "sutPoolQueue mutex init error\n");
		ret =0;
	}
	printf(P_INFO "sutPoolQueue init finish\n");
	return ret ;
}
/* ------------------------------------------------------------------------------ */
void sutPoolQueue::release()
{
	struct sutCarrierPageNode *tmp = StorePage;

	if(pthread_mutex_lock(&mutex) != 0) {
		printf(P_FATAL "Lock error\n");
	}
	else {
		while(StorePage != NULL) {
			tmp = StorePage->next;
			free(StorePage->page);
			free(StorePage);
			StorePage = tmp;
		}
		while(RecyclePage != NULL) {
			tmp = RecyclePage->next;
			free(RecyclePage->page);
			free(RecyclePage);
			RecyclePage = tmp;
		}
        StorePage = NULL;
        RecyclePage = NULL;
		pthread_mutex_unlock(&mutex);
	}
}
/* ------------------------------------------------------------------------------
 * Enqueue the data into queue
 *		@param src_data : the input data.
 *
 *		@return : 1 as success .
 *				  0 as full.
 *				 -1 as error (memory leak or mutext error).
 */
int sutPoolQueue::enqueue(void *src_data)
{
	int ret =0;
	struct sutCarrierPageNode *tmp;

	if(pthread_mutex_lock(&mutex) != 0) {
		printf(P_FATAL "Lock error\n");
		return -1;
	}
	else if (!is_full()) {
		*Store_ptr = src_data;
		Store_ptr++;
		Cost++;

		if(Store_ptr == StorePage->end) {	/* page exhaust, add new page from memory or recycle pool */
			if(RecyclePageCount >0) {	/* get recycle */
				tmp = RecyclePage;
				RecyclePage = RecyclePage->next;
				RecyclePageCount--;	
			}
			else { /* create new page */
				tmp = (struct sutCarrierPageNode *)malloc(sizeof(struct sutCarrierPageNode));		
				if(tmp == NULL) return -1;

				tmp->page = (void **)malloc(sizeof(void *) *PageCapacity);
				if(tmp->page == NULL) {
					free(tmp);
					return -1;
				}
				tmp->end = tmp->page + PageCapacity;
			}
			tmp->next = NULL;
			Store_ptr = tmp->page;
			StorePage->next = tmp;
			StorePage = tmp;
			PageCount++;
			QueueCapacity = PageCount * PageCapacity;
		}
		ret =1;
	}
	else {
		ret = 0;
	}
	pthread_mutex_unlock(&mutex);
	return ret;
}
/* ------------------------------------------------------------------------------
 * Dequeue the data from queue
 *		@param src_data : the input data.
 *
 *		@return : NULL as empty, or the non-null pointer to the data.
 */
void *sutPoolQueue::dequeue()
{
	void *ret = NULL;
	struct sutCarrierPageNode *tmp;

	if(pthread_mutex_lock(&mutex) != 0) {
		printf(P_FATAL "Lock error\n");
		return NULL;;
	}
	else if (!is_empty()) {
		ret = *Fetch_ptr;
		Fetch_ptr++;
		Cost--;

		if(Fetch_ptr == FetchPage->end) {	/* page exhaust, add into recycle */
			tmp = FetchPage->next;
			FetchPage->next = RecyclePage;
			RecyclePage = FetchPage;
			FetchPage = tmp;
			Fetch_ptr = tmp->page;
			RecyclePageCount++;
			PageCount--;
			QueueCapacity = PageCount * PageCapacity;
		}
	}
	pthread_mutex_unlock(&mutex);
	return ret;
}
/* ------------------------------------------------------------------------------ */
inline int sutPoolQueue::is_empty()
{
	return (Cost == 0);
}
/* ------------------------------------------------------------------------------ */
inline int sutPoolQueue::is_full()
{
	return (Cost == QueueCapacity);
}
/* ------------------------------------------------------------------------------ 
 * Return the count of data in queue
 *	@return : >= 0 as the count of data , -1 as error.
 *
*/
int sutPoolQueue::count()
{
	int tmp = -1;
	if(pthread_mutex_lock(&mutex) != 0) {
		printf(P_FATAL "Lock error\n");
	}
	else {
		tmp = Cost ;
		pthread_mutex_unlock(&mutex);
	}
	return tmp ;
}

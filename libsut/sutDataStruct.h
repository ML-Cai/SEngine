#ifndef LIB_SUT_DS_H
#define LIB_SUT_DS_H
// ------------------------------------------------------------------
#include <pthread.h>
/* ------------------------------------------------------------------
 * basic element
 * ------------------------------------------------------------------
 */
struct sutCarrierPageNode {
	void **page;
	void **end;	/* the end of this page*/
	struct sutCarrierPageNode *next;
};
/* ----------------------------------------------------------- 
 * The Pool Queue unit in sut
 *		In some application, the queue will carry the block in each node, such as video stream system.
 *		The sutPoolQueue is provide an interface to handle that.
 *		In real program , if we could pre-define the count of buffer, the access of queue is simple to implement.
 *		But, in some application, if the buffer count is unable to pre-defined, we must dynamic alloc the node to handle that.
 *		To reduce the malloc/free (new/delete) time of unknow buffer bonund, the sutPoolQueue will alloc a page of node when node poll underflow.
 *		The mode of sutPoolQueue decide by ModeStatic and ModeDynamic.
 *
 *		In ModeStatic, the capacity must > 0.
 *		In ModeDynamic, the capacity will be consted as 1024 per page.
 */

class sutPoolQueue {
public:
	sutPoolQueue();
	~sutPoolQueue();
	int init(int mode, int capacity);
	void release() ;
	void *dequeue();
	int enqueue(void *src_data);
	int count();
	int is_empty();
	int is_full();
	const static int ModeStatic 	= 0;
	const static int ModeAuto 	= 1;

private :
	int Cost;	
	int QueueCapacity;
	int PageCapacity;
	int PageCount;
	int RecyclePageCount;
	struct sutCarrierPageNode *RecyclePage;
	void **Store_ptr;
	struct sutCarrierPageNode *StorePage;
	void **Fetch_ptr;
	struct sutCarrierPageNode *FetchPage;

	/* operator */
	pthread_mutex_t mutex;
};

/* -----------------------------------------------------------
 * The LinkList
 */
struct sutLinkingListNode
{
    int ID;
    int Key;
    void *data ;
    struct sutLinkingListNode *prev;
    struct sutLinkingListNode *next;
};

class sutLinkingList
{
public:
    sutLinkingList();
    ~sutLinkingList();

    void init(int list_capacity);
    void release();
    int count();
    int add(void *src_data, unsigned int key);
    void *search_by_ID(int node_ID);
    void *search_by_key(unsigned int node_key);
    void *search_by_sequence(int sequence);
    void *fetch_by_ID(int node_ID);
    void *fetch_by_key(unsigned int node_key);
    int remove_by_ID(int node_ID);
    int remove_by_key(unsigned int node_key);
    int remove_by_sequence(int sequence);

    int is_empty() ;
    int is_full() ;

private :
    sutLinkingListNode *head_node;
    sutLinkingListNode *tail_node;

    int ID_acc;
    int cost;
    int capacity ;
    pthread_mutex_t mutex;

    /* operator */
    inline void remove_node(sutLinkingListNode *src_node);
    inline void lock();		/* pthread mutex lock */
    inline void unlock();	/* pthread mutex unlock */
};
/* ------------------------------------------------------------------ */
#endif

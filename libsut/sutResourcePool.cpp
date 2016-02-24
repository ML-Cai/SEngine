#include <string.h>
#include <stdlib.h>
#include "sutResourcePool.h"
#ifdef DN
//--------------------------------------------------------------------------
class sutResourcePool {
public :
    unsigned char *allocResource(int rSize);
    void release();

private :
    struct sutResourcePage {
        int elements;
        int count;
        unsigned char *data;
        void *end;
        void *fetch;
        struct sutResourcePage *next;
    };
    // The resource handler
    struct sutResourcePage *pActivePool;
    struct sutResourcePage *pReadyPage;
    struct sutResourcePage *pRecyclePage;
    int allocPage(int elements, int count);
    void recyclePage(struct sutResourcePage *fPage);
    void freePage(struct sutResourcePage *fPage);
};
//--------------------------------------------------------------------------
int sutResourcePool::allocPage(int elements, int count)
{
    sutResourcePage *page = NULL;
    if(pRecyclePage != NULL) {   // check the recycle pool
        page = pRecyclePage;
        page->fetch = page->data;
        pRecyclePage = pRecyclePage->next;
    }
    else {  //alloc new page
        page = (sutResourcePage *)malloc(sizeof(sutResourcePage));
        if(page != NULL) {
            page->data = (unsigned char *)malloc(elements * count);
            if(page->data != NULL) {
                page->count = count;
                page->elements = elements;
                page->fetch = page->elements;
                page->end = page->elements + count * elements;
            }
            else {
                free(page);
                page = NULL;
            }
        }
    }
    if(page != NULL) {
        page->next = pReadyPage;
        pReadyPage = page;
        return 0;
    }
    else {
        return 1;
    }
}
//--------------------------------------------------------------------------
void sutResourcePool::recyclePage(struct sutBinarySearchTree<TYPE>::sutResourcePage *fPage)
{
    fPage->next = pRecyclePage;
    pRecyclePage = fPage;
}
#endif

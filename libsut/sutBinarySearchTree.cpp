#ifdef adse
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sutBinarySearchTree.h"
//--------------------------------------------------------------------------
template class sutBinarySearchTree<int>;
//--------------------------------------------------------------------------
template <class iTYPE>
sutBinarySearchTree<iTYPE>::sutBinarySearchTree()
{
}
template <class iTYPE>
sutBinarySearchTree<iTYPE>::~sutBinarySearchTree()
{

}
//--------------------------------------------------------------------------
template <class iTYPE>
void sutBinarySearchTree<iTYPE>::init()
{
    count = 0;
    root = NULL;
    pNodeList_Head = NULL;
    pNodeList_Tail = NULL;
}

//--------------------------------------------------------------------------
template <class iTYPE>
int sutBinarySearchTree<iTYPE>::add(iTYPE inKey, void *inData)
{
    struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode *node = allocNode();
    //struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode *node = new sutBinarySearchTree<iTYPE>::sutBinaryTreeNode;
    struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode *recRoot = root;
    struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode **tmpRoot = &root;

    if(node != NULL) {
        node->key = inKey;
        node->data = inData;
        while(recRoot != NULL) {
            if(inKey > recRoot->key) {
                tmpRoot = &recRoot->nodeL;
                recRoot = recRoot->nodeL;
            }
            else {
                tmpRoot = &recRoot->nodeR;
                recRoot = recRoot->nodeR;
            }
        }
        *tmpRoot = node;
        return 1;
    }
    return 0;
}
/** --------------------------------------------------------------------------
 *  Search the carried data by Key
 *      @param inKey : The key for search, the data type is defined when sutBinarySearchTree<> created.
 *
 *      @return : NULL when search missing, otherwise the data pointer associated by key is returned.
 */
template <class iTYPE>
void *sutBinarySearchTree<iTYPE>::search(iTYPE inKey)
{
    struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode *recRoot = root;
    while(recRoot != NULL) {
        if(inKey == recRoot->key)
            return recRoot->data;
        else if(inKey > recRoot->key)
            recRoot = recRoot->nodeL;
        else if(inKey < recRoot->key)
            recRoot = recRoot->nodeR;
    }
    return NULL;
}

//--------------------------------------------------------------------------
template <class iTYPE>
void sutBinarySearchTree<iTYPE>::recDump(struct sutBinaryTreeNode *inNode, int Level)
{
    if(inNode != NULL) {
        printf("Level %d, Key %d\n", Level, inNode->key);
        recDump(inNode->nodeL, Level +1);
        recDump(inNode->nodeR, Level +1);
    }
}

//--------------------------------------------------------------------------
template <class iTYPE>
void sutBinarySearchTree<iTYPE>::dump()
{
    recDump(root, 0);
}

//--------------------------------------------------------------------------
template <class iTYPE>
void sutBinarySearchTree<iTYPE>::freeNode(struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode *fnode)
{
    lock();
    if(fnode != NULL) {
        fnode->nodeR = pNodeList_Tail;
        pNodeList_Tail = fnode;
        if(pNodeList_Head == NULL) pNodeList_Head = fnode;
    }
    unlock();
}

//--------------------------------------------------------------------------
template <class iTYPE>
struct sutBinarySearchTree<iTYPE>::sutBinaryTreeNode *sutBinarySearchTree<iTYPE>::allocNode()
{
    struct sutBinaryTreeNode *ret = NULL;

    if(pNodeList_Head == NULL) allocPage();
    if(pNodeList_Head != NULL) {    // if malloc failed , the allocPage may cause the pNodeList_Head as NULL
        lock();
        ret = pNodeList_Head;
        pNodeList_Head = pNodeList_Head->nodeR;
        if(pNodeList_Head == NULL) pNodeList_Tail = NULL;
        ret->nodeL = NULL;
        ret->nodeR = NULL;
        unlock();
    }
    return ret;
}
//--------------------------------------------------------------------------
template <class iTYPE>
int sutBinarySearchTree<iTYPE>::allocPage()
{
    int i;
    struct sutBinaryTreeNode *nPage = (struct sutBinaryTreeNode *)malloc(sizeof(sutBinaryTreeNode) * NodeRepository);
    struct sutBinaryTreeNode *cPage = nPage;
    if(nPage == NULL) {
        printf("Fatal : malloc failed\n");
        return 0;
    }
    for(i = 0 ; i < NodeRepository -1 ; i++) {
        cPage->nodeR = cPage +1;
        cPage++;
    }
    lock();
    if(pNodeList_Tail == NULL) pNodeList_Tail = cPage;  //the cPage in this condition is the last node allocated
    cPage->nodeR = pNodeList_Head;
    pNodeList_Head = nPage;
    unlock();
    printf("Alloc page\n");
    return 1;
}

//--------------------------------------------------------------------------
template <class iTYPE>
void sutBinarySearchTree<iTYPE>::lock()
{

}
//--------------------------------------------------------------------------
template <class iTYPE>
void sutBinarySearchTree<iTYPE>::unlock()
{

}
#endif


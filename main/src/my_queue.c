#include "my_queue.h"
#include "stdlib.h"
#include <stddef.h>
#include "string.h"


static Node* newNode(void *data, size_t size) {
    Node* node = (Node*)malloc(sizeof(Node));

    node->data = (void*)malloc(size);
    memcpy(node->data, data, size);

    node->next = NULL;
    return node;
}


myQueue* createQueue(size_t element_size) {
    myQueue* q = (myQueue*)malloc(sizeof(myQueue));
    q->front = q->rear = NULL;
    q->currSize = 0;
    q->element_size = element_size;
    return q;
}


bool isEmpty(myQueue* q) {
    return q->front == NULL;
}


void enqueue(myQueue* q, void* data) {
    Node* node = newNode(data, q->element_size);
    if (isEmpty(q)) {
        q->front = q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    
    q->currSize++;
}

//dequeue pointer to heap - you must free that memory after dequeue
bool dequeue(myQueue* q, void *data) {
    if (isEmpty(q)) {
        return false;
    }
    
    Node* temp = q->front;
    void* removedData = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    
    q->currSize--;

    memcpy(data, removedData, q->element_size);

    free(removedData);
    return true;
}

// Get front element
void* getFront(myQueue* q) {
    if (isEmpty(q)) {
        return NULL;
    }
    return q->front->data;
}


#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


typedef struct Node {
    void* data;
    struct Node* next;
} Node;

typedef struct myQueue {
    uint8_t currSize;
    uint8_t maxSize;
    size_t element_size;
    Node* front;
    Node* rear;
} myQueue;

myQueue* createQueue(size_t element_size);
bool isEmpty(myQueue* q);
void enqueue(myQueue* q, void* data);
bool dequeue(myQueue* q, void *data) ;
void* getFront(myQueue* q);

#endif
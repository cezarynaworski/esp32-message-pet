#include "my_queue.h"
#include "stdlib.h"
#include "string.h"
#include <stddef.h>
#include <stdint.h>

static Node *newNode(void *data, size_t size) {

  if (data == NULL || size == 0)
    return NULL;
  Node *node = (Node *)malloc(sizeof(Node));

  if (node == NULL)
    return NULL;
  node->data = (void *)malloc(size);

  if (node->data==NULL){
    free(node);
    return NULL;
  }
  memcpy(node->data, data, size);
  node->next = NULL;
  return node;
}

myQueue *createQueue(size_t element_size, uint8_t max_size) {
  myQueue *q = (myQueue *)malloc(sizeof(myQueue));
  if (q == NULL)
    return NULL;
  q->front = q->rear = NULL;
  q->currSize = 0;
  q->element_size = element_size;
  q->maxSize = max_size;
  return q;
}

bool isEmpty(myQueue *q) { return q->front == NULL; }

bool enqueue(myQueue *q, void *data) {

  if(q==NULL || data==NULL)
    return false;
  if (q->currSize >= q->maxSize)
    return false;

  Node *node = newNode(data, q->element_size);
  if (node == NULL)
    return false;
  if (isEmpty(q)) {
    q->front = q->rear = node;
  } else {
    q->rear->next = node;
    q->rear = node;
  }

  q->currSize++;
  return true;
}

// dequeue pointer to heap - you must free that memory after dequeue
bool dequeue(myQueue *q, void *data) {
  if (isEmpty(q)) {
    return false;
  }

  Node *temp = q->front;
  void *removedData = temp->data;
  q->front = q->front->next;
  if (q->front == NULL)
    q->rear = NULL;
  free(temp);

  q->currSize--;

  memcpy(data, removedData, q->element_size);

  free(removedData);
  return true;
}

// Get front element
void *getFront(myQueue *q) {
  if (isEmpty(q)) {
    return NULL;
  }
  return q->front->data;
}

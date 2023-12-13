#pragma once

#include <stdio.h>
#include <malloc.h>

typedef int QuDataType;

typedef struct QListNode
{
    struct QListNode *_next;
    QuDataType _data;
} QueueNode;


typedef struct Queue
{
    QueueNode *_front;
    QueueNode *_rear;
} Queue;


void QueueInit(Queue *q);


void QueuePush(Queue *q, QuDataType data);


void QueuePop(Queue *q);


QuDataType QueueFront(Queue *q);


QuDataType QueueBack(Queue *q);


int QueueSize(Queue *q);


int QueueEmpty(Queue *q);


void QueueDestroy(Queue *q);

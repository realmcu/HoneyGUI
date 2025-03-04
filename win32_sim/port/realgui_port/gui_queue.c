#include "gui_queue.h"

QueueNode *BuyQueueNode(QuDataType x)
{
    QueueNode *cur = (QueueNode *)malloc(sizeof(QueueNode));
    cur->_data = x;
    cur->_next = NULL;
    return cur;
}
void QueueInit(Queue *q)
{
    q->_front = NULL;
    q->_rear = NULL;
}
void QueuePush(Queue *q, QuDataType x)
{
    QueueNode *cur = BuyQueueNode(x);
    if (q->_front == NULL)
    {
        q->_front = q->_rear = cur;
    }
    else
    {
        q->_rear->_next = cur;
        q->_rear = cur;
    }
}
void QueuePop(Queue *q)
{
    if (q->_front == NULL)
    {
        return;
    }
    QueueNode *tmp = q->_front->_next;
    free(q->_front);
    q->_front = tmp;
}
QuDataType QueueFront(Queue *q)
{
    return q->_front->_data;
}
QuDataType QueueBack(Queue *q)
{
    return q->_rear->_data;
}
int QueueEmpty(Queue *q)
{
    return q->_front == NULL;
}
int QueueSize(Queue *q)
{
    QueueNode *cur;
    int count = 0;
    for (cur = q->_front; cur; cur = cur->_next)
    {
        count++;
    }
    return count;
}
void QueueDestroy(Queue *q)
{
    if (q->_front == NULL)
    {
        return;
    }
    while (q->_front)
    {
        QueuePop(q);
    }
}

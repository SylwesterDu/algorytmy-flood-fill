//! \file queue.h Struktury kolejki.

#pragma once
#include <stdlib.h>
#include <stdint.h>

//! Struktura elementu kolejki, przechowuje współrzędne piksela.
typedef struct QueueNode {
    uint32_t position_x;
    uint32_t position_y;
    struct queue_node* next;
} QueueNode;

//! Struktura przechowująca głowę i ogon kolejki.
typedef struct QueuePointers {
    QueueNode* head;
    QueueNode* tail;
} QueuePointers;

bool enqueue(QueuePointers* queue, uint32_t, uint32_t);
bool dequeue(QueuePointers* queue, uint32_t*, uint32_t*);
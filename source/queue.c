//! \file queue.c Funkcje związane z kolejką.

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "queue.h"


/*!
* Funkcja ustawiająca element ze współrzędnymi piksela w kolejce. 
* W przypadku powodzenia zwraca true.
* \param QueuePointers* queue Wskaźniki kolejki.
* \param uint32_t position_x Pozycja X piksela.
* \param uint32_t position_y Pozycja Y piksela.
* \returns true dla powodzenia operacji, false dla niepowodzenia.
*/
bool enqueue(QueuePointers* queue, uint32_t position_x, uint32_t position_y) {
    QueueNode* new_node = (QueueNode*) malloc(sizeof(QueueNode));
    if (NULL != new_node) {
        new_node->position_x = position_x;
        new_node->position_y = position_y;
        new_node->next = NULL;
        if (NULL == queue->head) {
            queue->head = queue->tail = new_node;
        }
        else {
            queue->tail->next = new_node;
            queue->tail = queue->tail->next;
        }
        return true;
    }
    return false;
}


/*!
* Funkcja umieszczająca współrzędne piksela do zmiennych przekazywanych przez wskaźnik.
* Usuwa element z początku kolejki.
* W przypadku pustej kolejki zwraca false.
* \param QueuePointers* queue Wskaźniki kolejki.
* \param uint32_t* position_x Pozycja X piksela.
* \param uint32_t* position_y Pozycja Y piksela.
* \returns true dla powodzenia operacji, false dla niepowodzenia.
*/
bool dequeue(QueuePointers* queue, uint32_t* position_x, uint32_t* position_y) {
    if (queue->head == NULL) return false;
    QueueNode* tmp = queue->head->next;
    *position_x = queue->head->position_x;
    *position_y = queue->head->position_y;
    free(queue->head);
    queue->head = tmp;
    if (tmp == NULL) {
        queue->tail = NULL;
    }
    return true;
}
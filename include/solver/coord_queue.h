#ifndef COORD_QUEUE_H
#define COORD_QUEUE_H

#include "board/coordinate.h"

class CQNode {
    Coord value;
    CQNode *next;

public: 
    CQNode(Coord coord);
    friend class CoordQueue;
}; 

class CoordQueue {
    int size;
    CQNode *head;
    CQNode *tail;

public:
    CoordQueue();
    ~CoordQueue();

    int get_size();
    void enqueue(Coord coord);
    Coord dequeue();
};

#endif


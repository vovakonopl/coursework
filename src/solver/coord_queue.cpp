#include "solver/coord_queue.h"

CQNode::CQNode(Coord coord) {
    value = coord;
    next = nullptr;
} 

CoordQueue::CoordQueue() {
    size = 0;
    head = tail = nullptr;
}

CoordQueue::~CoordQueue() {
    CQNode *node = head;
    while (node) {
        CQNode *next = node->next;
        delete node;

        node = next;
    }
}

int CoordQueue::get_size() {
    return size;
}

void CoordQueue::enqueue(Coord coord) {
    size++;
    if (size == 1) {
        head = tail = new CQNode(coord);
        return;
    }

    tail->next = new CQNode(coord);
    tail = tail->next;
}

Coord CoordQueue::dequeue() {
    if (size == 0) {
        return Coord(-1, -1);
    }

    size--;
    Coord coord = head->value;
    CQNode *next = head->next;
    delete head;

    if (size == 0) {
        head = tail = nullptr;
    } else {
        head = next;
    }

    return coord;
}

#include "board/region.h"
#include <iostream>
#include <stdexcept>

Region::Region(int id, int target_size) {
    this->id = id;
    this->target_size = target_size;
}

int Region::get_id() {
    return id;
}

int Region::get_target_size() {
    return target_size;
}

int Region::get_size() {
    return static_cast<int>(coords.size());
}

void Region::push(const Coord &coord) {
    coords.push_back(coord);
}

Coord Region::pop() {
    if (coords.size() == 0) {
        return Coord(-1, -1);
    }
    
    Coord coord = coords.at(coords.size() - 1);
    coords.pop_back();
 
    return coord;
}

Coord Region::coord_at(int idx) {
    if (idx < 0 || idx >= static_cast<int>(coords.size())) {
        std::cerr << "coord_at(int): index out of range!";
        throw std::out_of_range("Index out of range!");
    }

    return coords.at(idx);
}

void Region::remove_at(int idx) {
    // remove last
    int size = static_cast<int>(coords.size());
    if (idx < 0 || idx >= size) {
        std::cerr << "remove_at(int): index out of range!";
        throw std::out_of_range("Index out of range!");
    }

    if (idx == size - 1) {
        coords.pop_back();
        return;
    }

    // if not last => shift items left and remove last
    for (int i = idx; i < size - 1; i++) {
        coords.at(i) = coords.at(i + 1);
    }

    coords.pop_back();
}

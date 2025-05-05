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

bool Region::push(const Coord &coord) {
    if (target_size != -1 && coords.size() >= static_cast<long unsigned int>(target_size)) {
        return false;
    }

    coords.push_back(coord);
    return true;
}

Coord Region::pop() {
    if (coords.size() == 0) {
        return Coord(-1, -1);
    }
    
    Coord coord = coords.at(coords.size() - 1);
    coords.pop_back();
 
    return coord;
}

Coord Region::coord_at(long unsigned int idx) {
    if (idx < 0 || idx >= coords.size()) {
        std::cout << "Idx error: " << idx << "\n";
        throw std::out_of_range("Index out of range!");
    }

        std::cout << "Idx: " << idx << "\n";
    return coords.at(idx);
}

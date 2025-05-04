#include "board/region.h"
#include <stdexcept>

Region::Region(int id, int target_size) {
    this->id = id;
}

int Region::get_id() {
    return id;
}

int Region::get_target_size() {
    return target_size;
}

int Region::get_size() {
    return coords.size();
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

    Coord coord = coords.back();
    coords.pop_back();
    return coord;
}

Coord Region::coord_at(long unsigned int idx) {
    if (idx < 0 || idx >= coords.size()) {
        throw std::out_of_range("Index out of range!");
    }

    return coords.at(idx);
}

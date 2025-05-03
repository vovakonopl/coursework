#include "board/region.h"

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
    if (coords.size() >= static_cast<long unsigned int>(target_size)) {
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

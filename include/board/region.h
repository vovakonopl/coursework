#ifndef REGION_H
#define REGION_H

#include "board/coordinate.h"
#include <vector>

class Region {
    int id; 
    int target_size; // -1 if it hasn't target size and always can grow 
    std::vector<Coord> coords;

public:
    Region (int id, int target_size);
    
    int get_id();
    int get_target_size();
    int get_size();
    
    void push(const Coord &coord);
    Coord pop(); // returns Coord {-1, -1} if empty
    Coord coord_at(int idx);
    void remove_at(int idx);
};

#endif


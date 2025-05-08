#ifndef COORDINATE_H
#define COORDINATE_H

#include <ostream>

class Coord {
public:
    int row;
    int col;

    Coord();
    Coord(int row, int col);

    bool operator ==(Coord coord);
 
    friend std::ostream &operator <<(std::ostream &stream, Coord coord);
};

#endif


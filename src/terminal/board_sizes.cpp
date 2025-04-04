#include "terminal/board_sizes.h"

const int sizes[][2] {
    // { width, height }
    { 2, 1 }, // Small
    { 3, 1 }, // Medium
    { 6, 3 }  // Large
};

void set_sizes(BoardSize size, int &width, int &height) {
    int idx = static_cast<int>(size);
    width = sizes[idx][0];
    height = sizes[idx][1];
}

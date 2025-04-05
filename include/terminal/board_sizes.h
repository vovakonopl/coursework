#ifndef BOARD_SIZES_H
#define BOARD_SIZES_H
    
enum class BoardSize {
    Small,
    Medium,
    Large
};

void set_sizes(BoardSize size, int &width, int &height);

#endif

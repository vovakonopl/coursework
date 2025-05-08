#ifndef SOLVER_H
#define SOLVER_H

#include "board/board.h"
#include "board/cell.h"
#include "board/region.h"

bool solve(Board &board);
bool solve(Board &board, Region *p_region, Cell &cell);

#endif

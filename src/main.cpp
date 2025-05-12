#include "solver/manual_solving.h"
#include "terminal/terminal_io.h"
#include "solver/solver.h"
#include "solver/solve_mode.h"
#include <iostream>

int main(void) {
    Terminal tm;
 
    int rows, cols;
    tm.ask_board_sizes(rows, cols);
    SolveMode mode = tm.select_mode_menu();

    Board board(rows, cols);
    tm.set_board(&board);

    if (mode == SolveMode::Computer) {
        tm.fill_fixed_cells();
        board.create_fixed_cells_list();
        
        bool result = solve(board);
        if (result) {
            board.fill_values_on_board();
            tm.render_board(true);
        } else {
            std::cout << "Can't solve\n";
        }

        return 0;
    } 
 
    while (true) {
        tm.fill_fixed_cells();
        board.create_fixed_cells_list();
        
        if (is_correctly_solved(board)) {
            board.fill_values_on_board();
            tm.render_board(true);
            break;
        } 
    }

    return 0;
}

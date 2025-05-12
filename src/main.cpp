#include "solver/manual_solving.h"
#include "terminal/codes/graphics.h"
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
 
    const char *wrong_msg = FONT_RED BOLD "Wrong solution. Try again!" RESET_ALL;
    bool is_solved = true;
    do {
        tm.fill_fixed_cells(is_solved ? nullptr : wrong_msg);
        board.create_fixed_cells_list();
        is_solved = is_correctly_solved(board);
        
        if (is_solved) {
            board.fill_values_on_board();
            tm.render_board(true);
            break;
        } 
    }  while (!is_solved);

    return 0;
}

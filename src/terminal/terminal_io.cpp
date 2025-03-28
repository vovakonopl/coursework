#include "terminal/terminal_io.h"
#include "terminal/codes/erase.h"
#include "terminal/codes/cursor.h"
#include <iostream>

using std::cout;
using std::cin;
using std::string;

void Terminal::clear_input() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Terminal::clear_terminal() {
    cout << ERASE_SCREEN CURSOR_HOME;
}

ArrowKey Terminal::read_arrow_key() {
    char ch = cin.get();
    if (ch != ESC_CHAR) {
        cin.putback(ch);
        return ArrowKey::None;
    } 

    ch = cin.get();
    if (ch != '[') {
        cin.putback(ch);
        cin.putback(ESC_CHAR);
        return ArrowKey::None;
    } 

    ch = cin.get();
    switch (ch) {
        case 'A': 
            return ArrowKey::Up;

        case 'B': 
            return ArrowKey::Down;
        
        case 'c': 
            return ArrowKey::Right;

        case 'D': 
            return ArrowKey::Left;

        default:
            cin.putback(ch);
            cin.putback('[');
            cin.putback(ESC_CHAR);
            return ArrowKey::None;
    }
}  

Board Terminal::read_board() {
    const string msg = "Enter the size of the board (n x n). n = "; 
    int board_size;
    
    clear_terminal();
    cout << msg;
    bool has_size = false;
    while (!has_size) {
        cin >> board_size;
        if (cin.fail()) {
            cin.clear();

            bool is_arrow_key = true;
            while (is_arrow_key) {
                is_arrow_key = read_arrow_key() != ArrowKey::None;
            }

            cin >> board_size;
            if (cin.fail()) {
                cin.clear();
                clear_input();
                clear_terminal();
                cout << "Invalid input! " << msg;
                continue;
            }
        }

        has_size = true;
    }

    clear_input();
    return Board(board_size, board_size);
}

void Terminal::render_board(Board &board) {
    
}

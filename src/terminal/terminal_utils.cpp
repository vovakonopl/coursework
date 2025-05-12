#include "terminal/codes/graphics.h"
#include "terminal/terminal_io.h"
#include "terminal/terminal_utils.h"
#include "terminal/codes/borders.h"
#include "terminal/codes/cursor.h"
#include "terminal/codes/erase.h"
#include <iostream>
#include <string>
#include <limits>
#include <termios.h>
#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;
using std::string_view;
using std::string;

void disable_canon_mode() {
    termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= ~ICANON;

    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

void enable_canon_mode() {
    termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag |= ICANON;

    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

void disable_echo() {
    termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= ~ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &settings);

}
void enable_echo() {
    termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag |= ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

void clear_input() {
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void clear_terminal() {
    cout << ERASE_SCREEN CURSOR_HOME;
}

ArrowKey read_arrow_key() {
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
        
        case 'C': 
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

bool ignore_esc_sequence() {
    char ch = cin.get();
    if (ch != ESC_CHAR) {
        cin.putback(ch);
        return false;
    }

    // Double check - most sequences have [ after ESC
    ch = cin.get();
    if (ch != '[') {
        cin.putback(ch);
        cin.putback(ESC_CHAR);
        return false;
    }

    // All escape sequences ends with char between @ and ~ in ASCII table
    do {
        ch = cin.get();
    } while (ch < '@' || ch > '~');
    
    return true;
}

// !* helper function *!
bool fits_on_screen(BoardSize size, Board *p_board, const winsize &win_size) {
    int cell_width; 
    int cell_height;
    set_sizes(size, cell_width, cell_height);
    
    // + 1 for boarders
    int board_width = 1 + (cell_width + 1) * p_board->get_cols();  
    int board_height = 1 + (cell_height + 1) * p_board->get_rows();  
  
    return 
        board_width <= win_size.ws_col &&
        board_height + 4 <= win_size.ws_row; // reserve rows under the board
}

// outputs a full border line (whole row)
// takes characters in params to output them on specified places 
void print_border_line(
    int cols,
    int cell_width,
    string_view main_char, // fills main space (e.g. ═══)
    string_view first_char, // char at the beginning of the line
    string_view split_char, // char at the place where the borders may somehow cross
    string_view last_char // char at the end of the line
) {
    cout << first_char;
    for (int col = 0; col < cols; col++) {
        for (int i = 0; i < cell_width; i++) {
            cout << main_char;
        }

        if (col != cols - 1) {
            cout << split_char; 
        }
    }
    cout << last_char << endl;
}

void display_finish_button(int board_width, bool is_active) {
    string finish_btn = "[ Finish ]";
    // left margin to center the button 
    int btn_left_margin = (board_width - finish_btn.length()) / 2; 
 
    cout << cursor_right(btn_left_margin); 
    cout << (is_active ? BG_ORANGE BOLD : FONT_ORANGE);
    cout << finish_btn << RESET_ALL << endl << endl;
}

string focuse_option(bool is_focused, string str) {
    string option;
    if (is_focused) {
        option += BG_ORANGE FONT_WHITE BOLD;
    }

    option += str;
    option += RESET_ALL FONT_ORANGE;

    return option;
}

void draw_menu(int focused_opt) {
    string title("Select solving mode");
    string manual_opt("Manual solving");    
    string computer_opt("Computer solving");    
    string exit_opt("Exit program");
    
    int max_len = title.length();
    int padding_x = 4;
    int padding_y = 1;
    int option_gap = 1;

    // +2 for borders
    int box_width = max_len + padding_x * 2 + 2; 
    int box_height = option_gap * 3 + padding_y * 2 + 2 + 4; // + 4 for every line of text
        
    string horiz_border_line;
    for (int i = 0; i < box_width - 2; i++) { // save 2 chars for corners
        horiz_border_line += HORIZ_BORDER;
    }

    // draw box
    cout << CURSOR_HOME TL_CORNER << horiz_border_line << TR_CORNER CURSOR_NEXT_LINE;
    for (int i = 0; i < box_height - 2; i++) {
        cout << VERT_BORDER << cursor_right(box_width - 2) << VERT_BORDER CURSOR_NEXT_LINE;
    }
    cout << BL_CORNER << horiz_border_line << BR_CORNER;

    // write title and options
    cout << FONT_ORANGE;
    int pos_y = padding_y + 2; // +2 - after top border
    int pos_x = padding_x + 2; // +2 - after left border
    cout << cursor_to(pos_y, pos_x) << title;
 
    pos_y += option_gap + 1;
    cout << cursor_to(pos_y, pos_x) << "1) "
        << focuse_option(focused_opt == 0, manual_opt);

    pos_y += option_gap + 1;
    cout << cursor_to(pos_y, pos_x) << "2) "
        << focuse_option(focused_opt == 1, computer_opt);

    pos_y += option_gap + 1;
    cout << cursor_to(pos_y, pos_x) << "3) "
        << focuse_option(focused_opt == 2, exit_opt);

    cout << RESET_ALL;
}


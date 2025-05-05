#include "logger/logger.h"
#include "board/board.h"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

void log_board(Board &board) {
    ofstream log_file(BOARD_LOG_FILE, ios::app);
    if (!log_file) {
        cout << "Can't open file for writing\n";
        return;
    }

    for (int row = 0; row < board.get_rows(); row++) {
        log_file << "|";
        for (int col = 0; col < board.get_cols(); col++) {
            Cell cell = board.cell_at(row, col);
            log_file << setw(2) << cell.get_value() << "|";
        }

        log_file << endl;
    }

    int line_width = board.get_rows() * 3 + 1;
    for (int i = 0; i < line_width; i++) {
        log_file << "~";
    }
    log_file << endl;
}

void clear_file() {
    ofstream board_log_file(BOARD_LOG_FILE);
    ofstream log_file(LOG_FILE);
}

void log_msg(string str) {
    ofstream log_file(LOG_FILE, ios::app);
    if (!log_file) {
        cout << "Can't open file for writing\n";
        return;
    }

    log_file << str << endl;
    for (int i = 0; i < 20; i++) {
        log_file << "~";
    }
    log_file << endl;
}

void log_board_regions(Board &board) {
    ofstream log_file(BOARD_LOG_FILE, ios::app);
    if (!log_file) {
        cout << "Can't open file for writing\n";
        return;
    }

    for (int row = 0; row < board.get_rows(); row++) {
        log_file << "|";
        for (int col = 0; col < board.get_cols(); col++) {
            Cell cell = board.cell_at(row, col);
            log_file << setw(2) << cell.region_id << "|";
        }

        log_file << endl;
    }

    int line_width = board.get_rows() * 3 + 1;
    for (int i = 0; i < line_width; i++) {
        log_file << "~";
    }
    log_file << endl;
}

void log_to_board_msg(string str) {
    ofstream log_file(BOARD_LOG_FILE, ios::app);
    if (!log_file) {
        cout << "Can't open file for writing\n";
        return;
    }

    log_file << str << endl;
    for (int i = 0; i < 20; i++) {
        log_file << "~";
    }
    log_file << endl;
}

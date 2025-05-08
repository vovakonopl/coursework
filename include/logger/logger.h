#ifndef LOGGER_H
#define LOGGER_H 

#include "board/board.h"
#include <string>

#define BOARD_LOG_FILE "log_board.txt"
#define LOG_FILE "log.txt"

void clear_file();
void log_board(Board &board);
void log_board_regions(Board &board);
void log_msg(std::string str);
void log_to_board_msg(std::string str);

#endif


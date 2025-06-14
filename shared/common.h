#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <utility>  // std::pair

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 4

// ANSI 컬러
#define BLACK 0
#define BLUE 36
#define RED 31
#define YELLOW 33

// 공용 변수들 (기존 client.cpp 전역 변수)
extern int num_of_red_tiles, num_of_yellow_tiles, num_of_black_tiles, num_of_blue_tiles, num_of_total_tiles;
extern std::map<int,int> red_tiles, blue_tiles, yellow_tiles, black_tiles;
extern int num_of_table_red_tiles, num_of_table_yellow_tiles, num_of_table_black_tiles, num_of_table_blue_tiles, num_of_table_total_tiles;
extern std::map<int,int> table_red_tiles, table_blue_tiles, table_yellow_tiles, table_black_tiles;

// 함수 원형 (기존 client.cpp)
void error_msg();
void show_ascii_art();
void init();
void add_tiles();
void add_table_tiles(bool is_user);
void print_table_tiles();
void print_user_tiles();
void print_specific_tile(const std::vector<std::pair<int,int>>& v);
void check_same_val();
void check_continue_val();
void game_end();

// 1) Python 스크립트로 ASCII Art 출력
inline void printAsciiArt() {
    // make_ascii_art.py가 shared/ 안에 있다고 가정
    system("python3 shared/make_ascii_art.py");
}

// 2) ANSI 컬러 코드 스타일 데모
inline void demoAnsiStyles() {
    for (int i = 0; i < 110; ++i) {
        // i 번호별로 텍스트에 i 스타일 적용
        printf("%3d --> \033[%dmText\033[0m\n", i, i);
    }
}

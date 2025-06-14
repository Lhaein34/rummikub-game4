#include <iostream>
#include <vector>
#include <utility> // pair
#include <bitset>
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <random>
#include <unistd.h>
#include <arpa/inet.h>
#include "../shared/common.h"


#define BLACK 0
#define BLUE 36
#define RED 31
#define YELLOW 33

using namespace std;
void error_msg();

static vector<pair<int,int>> deckVec;

int num_of_red_tiles,num_of_yellow_tiles,num_of_black_tiles,num_of_blue_tiles,num_of_total_tiles = 0;
map<int,int> red_tiles;
map<int,int> blue_tiles;
map<int,int> yellow_tiles;
map<int,int> black_tiles;

int num_of_table_red_tiles,num_of_table_yellow_tiles,num_of_table_black_tiles, num_of_table_blue_tiles,num_of_table_total_tiles =0;
map<int,int> table_red_tiles;
map<int,int> table_blue_tiles;
map<int,int> table_yellow_tiles;
map<int,int> table_black_tiles;


void show_ascii_art(void)
{
    cout<<
    "\n ____                                _  _            _       ____          _                    \n"<<
    "|  _ \\  _   _  _ __ ___   _ __ ___  (_)| | __ _   _ | |__   / ___|   ___  | |__   __  ___  _ __ \n"<<
    "| |_) || | | || '_ ` _ \\ | '_ ` _ \\ | || |/ /| | | || '_ \\  \\___ \\  / _ \\ | |\\ \\ / / / _ \\| '__|\n"<<
    "|  _ < | |_| || | | | | || | | | | || ||   < | |_| || |_) |  ___) || (_) || | \\ V / |  __/| |   \n"<<
    "|_| \\_\\ \\__,_||_| |_| |_||_| |_| |_||_||_|\\_\\ \\__,_||_.__/  |____/  \\___/ |_|  \\_/   \\___||_|   \n"<<
    "                                                                                                \n"<<endl;
    cout<<
    "                         ____ ____ _  _ ____    ____ ___ ____ ____ ___ \n"<<
    "                         | __ |__| |\\/| |___    [__   |  |__| |__/  |  \n"<<
    "                         |__] |  | |  | |___    ___]  |  |  | |  \\  | \n"<<endl;
}

void init(void)
{
    red_tiles.clear();
    yellow_tiles.clear();
    black_tiles.clear();
    blue_tiles.clear();
    table_red_tiles.clear();
    table_blue_tiles.clear();
    table_yellow_tiles.clear();
    table_black_tiles.clear();

    // ★ 조커(0번) 초기화
    red_tiles[0]   = 0;
    yellow_tiles[0]= 0;
    black_tiles[0] = 0;
    blue_tiles[0]  = 0;
    table_red_tiles[0]   = 0;
    table_yellow_tiles[0]= 0;
    table_black_tiles[0] = 0;
    table_blue_tiles[0]  = 0;


    for(int i = 1; i<= 13; i++)
    {
        red_tiles.insert(make_pair(i, 0));
        yellow_tiles.insert(make_pair(i, 0));
        black_tiles.insert(make_pair(i, 0));
        blue_tiles.insert(make_pair(i, 0));
        table_red_tiles.insert(make_pair(i, 0));
        table_yellow_tiles.insert(make_pair(i, 0));
        table_black_tiles.insert(make_pair(i, 0));
        table_blue_tiles.insert(make_pair(i, 0));
    }
    num_of_red_tiles = 0;
    num_of_yellow_tiles = 0;
    num_of_black_tiles = 0;
    num_of_blue_tiles = 0;
    num_of_total_tiles = 0;
    num_of_table_red_tiles = 0;
    num_of_table_yellow_tiles = 0;
    num_of_table_black_tiles = 0;
    num_of_table_blue_tiles = 0;
    num_of_table_total_tiles = 0;
}

void initDeck() {
    deckVec.clear();
    for(int n=1;n<=13;++n){
      for(int c=0;c<4;++c){
        int colorCode = (c==0? RED : c==1? BLUE : c==2? YELLOW : BLACK);
        deckVec.emplace_back(colorCode, n);
        deckVec.emplace_back(colorCode, n);
      }
    }
    // 조커 2장
    deckVec.emplace_back(0,0);
    deckVec.emplace_back(0,0);
    shuffle(deckVec.begin(), deckVec.end(),
        default_random_engine(random_device{}()));
}

// ➌ 첫 14장 분배 (기존 dealRandomTiles 수정)
void dealRandomTiles() {
    init();            // 맵·카운터 초기화
    if(deckVec.empty()) initDeck();
    num_of_total_tiles = 14;
    for(int i=0; i<14; ++i){
      auto [colorCode, number] = deckVec.back();
      deckVec.pop_back();
      if(number==0){
        black_tiles[0]++;    // 조커 보관
      } else {
        switch(colorCode){
          case RED:    red_tiles[number]++;    break;
          case BLUE:   blue_tiles[number]++;   break;
          case YELLOW: yellow_tiles[number]++; break;
          case BLACK:  black_tiles[number]++;  break;
        }
      }
    }
    cout<<"✅ 내 패를 랜덤으로 14장 분배했습니다.\n";
}
// ➍ 한 장만 랜덤 추가
void addRandomTile() {
    if(deckVec.empty()) initDeck();
    auto [colorCode, number] = deckVec.back();
    deckVec.pop_back();
    if(number==0) {
      black_tiles[0]++;
    } else {
      switch(colorCode){
        case RED:    red_tiles[number]++;    break;
        case BLUE:   blue_tiles[number]++;   break;
        case YELLOW: yellow_tiles[number]++; break;
        case BLACK:  black_tiles[number]++;  break;
      }
    }
    num_of_total_tiles++;
    cout<<"✅ 내 패에 랜덤 타일 1장 추가되었습니다.\n";
}

// void add_tiles(void)
// {
//     try
//     {
//         cout<<"소지한 타일을 입력하세요\n"<<endl;
//         cout<<"# 빨강 타일 개수 (예: 5): ";
//         cin>>num_of_red_tiles;
//         cout<<"R빨강 타일 번호 입력 (예: 1 3 5): ";
//         for(int i = 0; i< num_of_red_tiles; i++)
//         {
//             int tile;
//             cin>>tile;
//             red_tiles[tile]++;
//         }

//         cout<<"# 파랑 타일 개수 (예: 5): ";
//         cin>>num_of_blue_tiles;
//         cout<<"파랑 타일 번호 입력 (예: 1 3 5): ";
//         for(int i = 0; i< num_of_blue_tiles; i++)
//         {
//             int tile;
//             cin>>tile;
//             if(tile > 13) error_msg();
//             blue_tiles[tile]++;
//         }

//         cout<<"# 노랑 타일 개수 (예: 5): ";
//         cin>>num_of_yellow_tiles;
//         cout<<"R노랑 타일 번호 입력 (예: 1 3 5): ";
//         for(int i = 0; i< num_of_yellow_tiles; i++)
//         {
//             int tile;
//             cin>>tile;
//             if(tile > 13) error_msg();
//             yellow_tiles[tile]++;
//         }
        
//         cout<<"# 검정 타일 개수 (예: 5): ";
//         cin>>num_of_black_tiles;
//         cout<<"검정 타일 번호 입력 (예: 1 3 5): ";
//         for(int i = 0; i< num_of_black_tiles; i++)
//         {
//             int tile;
//             cin>>tile;
//             if(tile > 13) error_msg();
//             black_tiles[tile]++;
//         }
//         num_of_total_tiles += num_of_red_tiles;
//         num_of_total_tiles += num_of_yellow_tiles;
//         num_of_total_tiles += num_of_blue_tiles;
//         num_of_total_tiles += num_of_black_tiles;
//     }
//     catch(int err)
//     {
//         error_msg();
//     }
 
// }


void add_table_tiles(bool is_user)
{
    // try
    // {
    //     cout<<"테이블에 추가할 새 타일을 입력하세요.\n"<<endl;
    //     cout<<"# o새 빨강 타일 개수 (예: 5): ";
    //     cin>>num_of_table_red_tiles;
    //     cout<<"새 빨강 타일 번호 입력 (예: 1 3 5): ";
    //     for(int i = 0; i< num_of_table_red_tiles; i++)
    //     {
    //         int tile;
    //         cin>>tile;
    //         if(is_user)
    //         {
    //             if(red_tiles[tile] > 0)
    //             {
    //                 red_tiles[tile]--;
    //                 num_of_total_tiles--;
    //             }
    //             else
    //             {
    //                 error_msg();
    //                 return ;
    //             }
    //         }
    //         table_red_tiles[tile]++;
    //     }

    //     cout<<"# 새 파랑 타일 개수 (예: 5): ";
    //     cin>>num_of_table_blue_tiles;
    //     cout<<"새 파랑 타일 번호 입력 (예: 1 3 5): ";
    //     for(int i = 0; i< num_of_table_blue_tiles; i++)
    //     {
    //         int tile;
    //         cin>>tile;
    //         if(is_user)
    //         {
    //             if(blue_tiles[tile] > 0)
    //             {
    //                 blue_tiles[tile]--;
    //                 num_of_total_tiles--;
    //             }
    //             else
    //             {
    //                 error_msg();
    //                 return ;
    //             }
    //         }
    //         table_blue_tiles[tile]++;
    //     }

    //     cout<<"# 새 노랑 타일 개수 (예: 5): ";
    //     cin>>num_of_table_yellow_tiles;
    //     cout<<"새 노랑 타일 번호 입력 (예: 1 3 5): ";
    //     for(int i = 0; i< num_of_table_yellow_tiles; i++)
    //     {
    //         int tile;
    //         cin>>tile;
    //         if(is_user)
    //         {
    //             if(yellow_tiles[tile] > 0)
    //             {
    //                 yellow_tiles[tile]--;
    //                 num_of_total_tiles--;
    //             }
    //             else
    //             {
    //                 error_msg();
    //                 return ;
    //             }
    //         }
    //         table_yellow_tiles[tile]++;
    //     }
        
    //     cout<<"# 새 검정 타일 개수 (예: 5): ";
    //     cin>>num_of_table_black_tiles;
    //     cout<<"새 검정 타일 번호 입력 (예: 1 3 5): ";
    //     for(int i = 0; i< num_of_table_black_tiles; i++)
    //     {
    //         int tile;
    //         cin>>tile;
    //         if(is_user)
    //         {
    //             if(black_tiles[tile] > 0)
    //             {
    //                 black_tiles[tile]--;
    //                 num_of_total_tiles--;
    //             }
    //             else
    //             {
    //                 error_msg();
    //                 return ;
    //             }
    //         }
    //         table_black_tiles[tile]++;
    //     }
    //     num_of_table_total_tiles += num_of_table_red_tiles;
    //     num_of_table_total_tiles += num_of_table_yellow_tiles;
    //     num_of_table_total_tiles += num_of_table_blue_tiles;
    //     num_of_table_total_tiles += num_of_table_black_tiles;
    // }
    // catch(int err)
    // {
    //     error_msg();
    // }

    try
    {
        cout << "\n▶ 내 패를 테이블에 놓기\n";

        int cnt, tile;

        // 1) 빨강
        cout << "# 놓을 빨강 타일 개수 (예: 3): ";
        cin  >> cnt;
        num_of_table_red_tiles = cnt;
        if(cnt > 0) {
            cout << "빨강 타일 번호 입력 (예: 1 3 5): ";
            for(int i = 0; i < cnt; ++i) {
                cin >> tile;
                if(red_tiles[tile] > 0) {
                    red_tiles[tile]--; 
                    table_red_tiles[tile]++;
                    num_of_total_tiles--;
                    num_of_table_total_tiles++;
                } else {
                    cout << "❌ 보유한 빨강 타일이 부족합니다: " << tile << "\n";
                }
            }
        }

        // 2) 파랑
        cout << "# 놓을 파랑 타일 개수 (예: 2): ";
        cin  >> cnt;
        num_of_table_blue_tiles = cnt;
        if(cnt > 0) {
            cout << "파랑 타일 번호 입력 (예: 4 7): ";
            for(int i = 0; i < cnt; ++i) {
                cin >> tile;
                if(blue_tiles[tile] > 0) {
                    blue_tiles[tile]--; 
                    table_blue_tiles[tile]++;
                    num_of_total_tiles--;
                    num_of_table_total_tiles++;
                } else {
                    cout << "❌ 보유한 파랑 타일이 부족합니다: " << tile << "\n";
                }
            }
        }

        // 3) 노랑
        cout << "# 놓을 노랑 타일 개수 (예: 1): ";
        cin  >> cnt;
        num_of_table_yellow_tiles = cnt;
        if(cnt > 0) {
            cout << "노랑 타일 번호 입력 (예: 10): ";
            for(int i = 0; i < cnt; ++i) {
                cin >> tile;
                if(yellow_tiles[tile] > 0) {
                    yellow_tiles[tile]--; 
                    table_yellow_tiles[tile]++;
                    num_of_total_tiles--;
                    num_of_table_total_tiles++;
                } else {
                    cout << "❌ 보유한 노랑 타일이 부족합니다: " << tile << "\n";
                }
            }
        }

        // 4) 검정 (JOKER 포함 number==0 처리)
        cout << "# 놓을 검정 타일 개수 (예: 2): ";
        cin  >> cnt;
        num_of_table_black_tiles = cnt;
        if(cnt > 0) {
            cout << "검정 타일 번호 입력 (예: 11 0): ";
            for(int i = 0; i < cnt; ++i) {
                cin >> tile;
                // tile==0 이면 조커
                if(tile == 0) {
                    // 조커는 black_tiles[0] 카운트에서 꺼내면 됩니다
                    if(black_tiles[0] > 0) {
                        black_tiles[0]--;
                        table_black_tiles[0]++;
                        num_of_total_tiles--;
                        num_of_table_total_tiles++;
                    } else {
                        cout << "❌ 보유한 조커가 부족합니다.\n";
                    }
                } else if(black_tiles[tile] > 0) {
                    black_tiles[tile]--;
                    table_black_tiles[tile]++;
                    num_of_total_tiles--;
                    num_of_table_total_tiles++;
                } else {
                    cout << "❌ 보유한 검정 타일이 부족합니다: " << tile << "\n";
                }
            }
        }

        cout << "✅ 테이블에 타일을 놓았습니다.\n";
    }
    catch(...)
    {
        cout << "❌ 잘못된 입력입니다. 다시 시도해주세요.\n";
    }
}

void print_table_tiles()
{
    try
    {
        for(int j = 1; j <= num_of_table_total_tiles; j++)
        {
            printf(".___  ");
        }
        cout<<endl;

        // 조커
        int jokers = table_red_tiles[0] + table_blue_tiles[0]
                   + table_yellow_tiles[0] + table_black_tiles[0];
        for(int j = 0; j < jokers; ++j)
            cout << "|J |  ";


        for(int i = 1; i<= 13; i++)
        {
            if(table_red_tiles[i] != 0)
            {
                for(int j = 0; j < table_red_tiles[i]; j++)
                {
                    printf("|");
                    printf("\033[%dm%2d\033[0m",RED, i);
                    printf("|  ");
                }

            }
            
        }
        for(int i = 1; i<= 13; i++)
        {
            if(table_blue_tiles[i] != 0)
            {

                for(int j = 0; j < table_blue_tiles[i]; j++)
                {
                    printf("|");
                    printf("\033[%dm%2d\033[0m",BLUE, i);
                    printf("|  ");
                }
            }    
        }
        for(int i = 1; i<= 13; i++)
        {
            if(table_yellow_tiles[i] != 0)
            {
                for(int j = 0; j < table_yellow_tiles[i]; j++)
                {
                    printf("|");
                    printf("\033[%dm%2d\033[0m",YELLOW, i);
                    printf("|  ");
                }
            }
        }
        for(int i = 1; i<= 13; i++)
        {
            if(table_black_tiles[i] != 0)
            {
                for(int j = 0; j < table_black_tiles[i]; j++)
                {
                    printf("|");
                    printf("\033[%dm%2d\033[0m",BLACK, i);
                    printf("|  ");
                }
            }
        } 
        cout<<endl;
        for(int j = 1; j <= num_of_table_total_tiles; j++)
        {
            printf("|__|  ");
        }
        cout<<endl;
    }
    catch(int err)
    {
        error_msg();
    }

}



void print_user_tiles()
{
    try
    {
        for(int j = 1; j <= num_of_total_tiles; j++)
        {
            printf(".___  ");
        }
        cout<<endl;

        // 조커
        int jokers = red_tiles[0] + blue_tiles[0]
                   + yellow_tiles[0] + black_tiles[0];
        for(int j = 0; j < jokers; ++j) {
            cout << "|J |  ";
        } 

        for(int i = 1; i<= 13; i++)
        {
            if(red_tiles[i] != 0)
            {
                for(int j = 0; j < red_tiles[i]; j++)
                {
                    printf("|");
                    printf("\033[%dm%2d\033[0m",RED, i);
                    printf("|  ");
                }

            }
            
        }
        for(int i = 1; i<= 13; i++)
        {
            if(blue_tiles[i] != 0)
            {

                for(int j = 0; j < blue_tiles[i]; j++)
                {
                    printf("|");
                    printf("\033[%dm%2d\033[0m",BLUE, i);
                    printf("|  ");
                }
            }    
        }
        for(int i = 1; i<= 13; i++)
        {
            if(yellow_tiles[i] != 0)
            {
                for(int j = 0; j < yellow_tiles[i]; j++)
                {
                    printf("|");
                    printf("\033[%dm%2d\033[0m",YELLOW, i);
                    printf("|  ");
                }
            }
        }
        for(int i = 1; i<= 13; i++)
        {
            if(black_tiles[i] != 0)
            {
                for(int j = 0; j < black_tiles[i]; j++)
                {
                    printf("|");
                    printf("\033[%dm%2d\033[0m",BLACK, i);
                    printf("|  ");
                }
            }
        } 
        cout<<endl;
        for(int j = 1; j <= num_of_total_tiles; j++)
        {
            printf("|__|  ");
        }
        cout<<endl;
    }
    catch(int err)
    {
        error_msg();
    }

}

void error_msg()
{
    cout<<
    "\n_   _ ____ _  _    ___ _   _ ___  ____ ___     _ ___    _ _ _ ____ ____ _  _ ____     ____ ____ ___ ____ _   _  \n"<<
    " \\_/  |  | |  |     |   \\_/  |__] |___ |  \\    |  |     | | | |__/ |  | |\\ | | __     |__/ |___  |  |__/  \\_/   \n"<<
    "  |   |__| |__|     |    |   |    |___ |__/    |  |     |_|_| |  \\ |__| | \\| |__] .   |  \\ |___  |  |  \\   |   .\n"<<endl;
}

// void print_specific_tile(vector< pair<int,int> > v)
// {
//     cout<<endl;
//     for(int i = 0; i< v.size(); i++)
//     {
//         printf(".___  ");
//     }
//     cout<<endl;
//     for(int i = 0; i < v.size(); i++)
//     {
//         printf("|");
//         printf("\033[%dm%2d\033[0m",v[i].first, v[i].second);
//         printf("|  ");
//     }
//     cout<<endl;
//     for(int i = 0; i< v.size(); i++)
//     {
//         printf("|__|  ");
//     }
//     cout<<endl;
// }

// client/client.cpp

// … include 후 …

void print_specific_tile(const std::vector<std::pair<int,int>>& v)
{
    // 실제 출력 로직 (벡터 참조 하나만 사용)
    std::cout << "\n";
    for (size_t i = 0; i < v.size(); ++i) {
        printf(".___  ");
    }
    std::cout << "\n";
    for (auto &pr : v) {
        printf("|");
        printf("\033[%dm%2d\033[0m", pr.first, pr.second);
        printf("|  ");
    }
    std::cout << "\n";
    for (size_t i = 0; i < v.size(); ++i) {
        printf("|__|  ");
    }
    std::cout << "\n";
}


// 내 패 조합
void check_same_val()
{
    for(int i = 1;i <= 13; i++)
    {
        int count = 0;
        if(red_tiles[i] != 0) count++;
        if(yellow_tiles[i] != 0) count++;
        if(blue_tiles[i] != 0) count++;
        if(black_tiles[i] != 0) count++;
        if(3 <= count)
        {
            vector < pair<int,int> > color_check ;
            if(red_tiles[i] != 0) color_check.push_back(make_pair(RED, i));
            if(yellow_tiles[i] != 0) color_check.push_back(make_pair(YELLOW,i));
            if(blue_tiles[i] != 0) color_check.push_back(make_pair(BLUE,i));
            if(black_tiles[i] != 0) color_check.push_back(make_pair(BLACK,i));
            print_specific_tile(color_check);
            
        }
    }
}
void check_continue_val()
{
    vector<pair<int,int> > continue_check;

    for(int i = 1; i<= 14; i++)
    {
        if(i == 14 || red_tiles[i] == 0)
        {
            if(3 <= continue_check.size())
            {
                print_specific_tile(continue_check);
                continue_check.clear();
            }
            else
            {
                continue_check.clear();
            }
            
        }
        else continue_check.push_back(make_pair(RED,i));

    }
    for(int i = 1; i<= 14; i++)
    {
        if(i == 14 || blue_tiles[i] == 0)
        {
            if(3 <= continue_check.size())
            {
                print_specific_tile(continue_check);
                continue_check.clear();
            }
            else
            {
                continue_check.clear();
            }
            
        }
        else continue_check.push_back(make_pair(BLUE,i));

    }
    for(int i = 1; i<= 14; i++)
    {
        if(i == 14 || yellow_tiles[i] == 0)
        {
            if(3 <= continue_check.size())
            {
                print_specific_tile(continue_check);
                continue_check.clear();
            }
            else
            {
                continue_check.clear();
            }
            
        }
        else continue_check.push_back(make_pair(YELLOW,i));

    }
    for(int i = 1; i<= 14; i++)
    {
        if(i == 14 || black_tiles[i] == 0)
        {
            if(3 <= continue_check.size())
            {
                print_specific_tile(continue_check);
                continue_check.clear();
            }
            else
            {
                continue_check.clear();
            }
            
        }
        else continue_check.push_back(make_pair(BLACK,i));

    }

}

// 테이블 맵 조합
void check_table_same_val() {
    cout << "\n▶ 테이블에서 같은 숫자 세트 찾기\n";
    for(int i = 1; i <= 13; ++i) {
        int cnt = 0;
        if(table_red_tiles[i])   cnt++;
        if(table_blue_tiles[i])  cnt++;
        if(table_yellow_tiles[i])cnt++;
        if(table_black_tiles[i]) cnt++;
        if(cnt >= 3) {
            vector<pair<int,int>> v;
            if(table_red_tiles[i])    v.emplace_back(RED, i);
            if(table_blue_tiles[i])   v.emplace_back(BLUE, i);
            if(table_yellow_tiles[i]) v.emplace_back(YELLOW, i);
            if(table_black_tiles[i])  v.emplace_back(BLACK, i);
            print_specific_tile(v);
        }
    }
}
void check_table_continue_val() {
    cout << "\n▶ 테이블에서 연속 숫자 Run 찾기\n";
    auto scanRun = [&](const map<int,int>& tbl, int color){
        vector<pair<int,int>> run;
        for(int i=1;i<=14;++i){
            bool present = (i<=13 && tbl.at(i)>0);
            if(present) {
                run.emplace_back(color, i);
            } else {
                if(run.size() >= 3) print_specific_tile(run);
                run.clear();
            }
        }
    };
    scanRun(table_red_tiles,   RED);
    scanRun(table_blue_tiles,  BLUE);
    scanRun(table_yellow_tiles,YELLOW);
    scanRun(table_black_tiles, BLACK);
}

void game_end()
{
    cout<<
    "____ ____ _  _ ____    ____ _  _ ____ ____ \n"<<
    "| __ |__| |\\/| |___    |  | |  | |___ |__/ \n"<<
    "|__] |  | |  | |___    |__|  \\/  |___ |  \\ \n"<<endl;


}

int main(void) {
    show_ascii_art();
    int button = -1;

    while(button != 0)
    {
        cout<<endl;
        cout<<"1. 타일 초기화 (내 패 입력)"<<endl;
        cout<<"2. 내 패 보기"<<endl;
        cout<<"3. 내 패로 가능한 조합 보기"<<endl;
        cout<<"4. 테이블 패로 가능한 조합 보기"<<endl;
        cout<<"5. 내 패에 새 타일 추가"<<endl;
        cout<<"6. 테이블 패 보기"<<endl;
        cout<<"7. 내 패를 테이블에 놓기"<<endl;
        cout<<"8. 상대 패를 테이블에 놓기"<<endl;
        cout<<"0. 종료"<<endl;
        cout<<"\n번호 선택 : ";
        cin>>button;
        // if(button == 1)
        // {
        //     // cout<<"\n1번을 선택하셨습니다."<<endl;
        //     // init();
        //     // add_tiles();
        //     // int num_of_total_tiles = num_of_red_tiles + num_of_yellow_tiles + num_of_blue_tiles + num_of_black_tiles;
        //     // if (num_of_total_tiles == 14)
        //     // {
        //     //     cout<<"완료되었습니다."<<endl;
        //     // }
        //     // else
        //     // {
        //     //     error_msg();
        //     //     init();
        //     // }
        //     cout << "\n1번: 내 패를 랜덤으로 초기화합니다.\n";
        //     dealRandomTiles();
        //     print_user_tiles();
            
        // }
        // else if(button == 2)
        // {
        //     cout<<"\n2번을 선택하셨습니다."<<endl;
        //     print_user_tiles();
        // }
        // else if(button == 3)
        // {
        //     cout<<"\n3번을 선택하셨습니다."<<endl;
        //     check_same_val();
        //     check_continue_val();
        // }
        // else if(button == 4)
        // {
        //     cout<<"\n4번을 선택하셨습니다."<<endl;
        //     cout<<"준비 중입니다."<<endl;
        // }
        // // else if(button == 5)
        // // {
            
        // //     cout<<"\n5번을 선택하셨습니다."<<endl;
        // //     add_tiles();
        // // }
        // else if (button == 5)
        // {
        //     cout << "\n5번: 내 패에 랜덤 타일 1장 추가\n";
        //     // 1장만 뽑아서 map에 추가하는 코드 (dealRandomTiles 전체 14장 대신):
        //     vector<pair<int,int>> deck;
        //     for (int n = 1; n <= 13; ++n)
        //     for (int c = 0; c < 4; ++c)
        //         deck.emplace_back(c, n), deck.emplace_back(c, n);
        //     deck.emplace_back(0, 0); deck.emplace_back(0, 0); // 조커 두 장
        //     shuffle(deck.begin(), deck.end(), default_random_engine(random_device{}()));
        //     auto p = deck[0];
        //     int colorCode = p.first, tileNum = p.second;
        //     if (tileNum == 0) {
        //         // 조커 처리
        //     } else {
        //         switch (colorCode) {
        //             case 0: red_tiles[tileNum]++; break;
        //             case 1: blue_tiles[tileNum]++; break;
        //             case 2: yellow_tiles[tileNum]++; break;
        //             case 3: black_tiles[tileNum]++; break;
        //         }
        //     }
        //     num_of_total_tiles++;
        //     cout << "✅ 랜덤 타일 1장 추가 완료!\n";
        // }
        // else if(button == 6)
        // {
        //     cout<<"\n6번을 선택하셨습니다."<<endl;
        //     print_table_tiles();
        // }
        // else if(button == 7)
        // {
        //     cout<<"\n7번을 선택하셨습니다."<<endl;
        //     add_table_tiles(true);
        // }
        // else if(button == 8)
        // {
        //     cout<<"\n8번을 선택하셨습니다."<<endl;
        //     add_table_tiles(false);
        // }
        // else if(button == 0)
        // {
        //     cout<<"0번을 선택하셨습니다. 프로그램을 종료합니다."<<endl;
        //     game_end();
        //     break;
        // }
        // else
        // {
        //     error_msg();
        // }
        
        if(button==1){
            cout<<"\n▶ 1번 선택: 내 패를 랜덤으로 초기화합니다.\n";
            dealRandomTiles();
            print_user_tiles();
        }
        else if(button==2){
            cout<<"\n▶ 2번: 내 패 보기\n";
            print_user_tiles();
        }
        else if(button==3){
            cout<<"\n▶ 3번: 내 패로 가능한 조합 보기\n";
            check_same_val();
            check_continue_val();
        }
        else if(button==4){
            cout<<"\n▶ 4번: 테이블 패로 가능한 조합 보기\n";
            check_table_same_val();
            check_table_continue_val();
        }
        else if(button==5){
            cout<<"\n▶ 5번: 내 패에 랜덤 타일 1장 추가\n";
            addRandomTile();
            print_user_tiles();
        }
        else if(button==6){
            cout<<"\n▶ 6번: 테이블 패 보기\n";
            print_table_tiles();
        }
        else if(button==7){
            cout<<"\n▶ 7번: 내 패를 테이블에 놓기\n";
            add_table_tiles(true);
            print_table_tiles();
        }
        else if(button==8){
            cout<<"\n▶ 8번: 상대 패를 테이블에 놓기\n";
            add_table_tiles(false);
            print_table_tiles();
        }
        else if(button==0){
            cout<<"프로그램을 종료합니다.\n";
            game_end();
        }
        else {
            error_msg();
        }
    }

    return 0;
    
}
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
#include "../shared/common.hpp"
#include <set>
#include <thread>
#include <cstring> 
#include <cstdio>
#include <csignal>

static constexpr char SERVER_IP[]   = "127.0.0.1";
static constexpr int  SERVER_PORT   = PORT;      // common.hpp 에 정의된 PORT(12345) 사용
static std::vector<std::pair<int,int>> last_placed_tiles;

#define CONNECT_BUF_SIZE BUFFER_SIZE
#define BLACK   0
#define BLUE    36
#define RED     31
#define YELLOW  33

using namespace std;
void error_msg();

static vector<pair<int,int>> deckVec;
static bool initial_meld_done = false;

int sock = -1;

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
    "| |_) || | | || '_  _ \\ | '_  _ \\ | || |/ /| | | || '_ \\  \\___ \\  / _ \\ | |\\ \\ / / / _ \\| '__|\n"<<
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

// 같은 숫자 3장 이상 모임(set)인지 확인
static bool is_set(const vector<pair<int,int>>& v) {
    if (v.size() < 3) return false;
    int num = v[0].second;
    set<int> colors;
    for (auto &p : v) {
        if (p.second != num) return false;
        colors.insert(p.first);
    }
    return colors.size() == v.size();
}
// 같은 색상으로 연속 숫자(run) 3개 이상인지 확인
static bool is_run(vector<pair<int,int>> v) {
    if (v.size() < 3) return false;
    int color = v[0].first;
    vector<int> nums;
    for (auto &p : v) {
        if (p.first != color) return false;
        nums.push_back(p.second);
    }
    sort(nums.begin(), nums.end());
    for (size_t i = 1; i < nums.size(); ++i) {
        if (nums[i] != nums[i-1] + 1) return false;
    }
    return true;
}

void add_table_tiles(bool is_user)
{
    try
    {
        cout << "\n▶ 내 패를 테이블에 놓기\n";

        vector<pair<int,int>> placed;

        int cnt, tile;

        // 1) 빨강 타일
        cout << "# 놓을 빨강 타일 개수 (예: 3): ";
        cin  >> cnt;
        num_of_table_red_tiles = cnt;
        if (cnt > 0) {
            cout << "빨강 타일 번호 입력 (예: 1 3 5): ";
            for (int i = 0; i < cnt; ++i) {
                cin >> tile;
                if (tile < 1 || tile > 13) {
                    cout << "❌ 잘못된 빨강 타일 번호: " << tile << "\n";
                }
                else if (red_tiles[tile] > 0) {
                    placed.emplace_back(RED, tile);
                } else {
                    cout << "❌ 보유한 빨강 타일이 부족합니다: " << tile << "\n";
                }
            }
        }
        // 2) 파랑 타일
        cout << "# 놓을 파랑 타일 개수 (예: 2): ";
        cin  >> cnt;
        num_of_table_blue_tiles = cnt;
        if (cnt > 0) {
            cout << "파랑 타일 번호 입력 (예: 4 7): ";
            for (int i = 0; i < cnt; ++i) {
                cin >> tile;
                if (tile < 1 || tile > 13) {
                    cout << "❌ 잘못된 파랑 타일 번호: " << tile << "\n";
                }
                else if (blue_tiles[tile] > 0) {
                    placed.emplace_back(BLUE, tile);
                } else {
                    cout << "❌ 보유한 파랑 타일이 부족합니다: " << tile << "\n";
                }
            }
        }
        // 3) 노랑 타일
        cout << "# 놓을 노랑 타일 개수 (예: 1): ";
        cin  >> cnt;
        num_of_table_yellow_tiles = cnt;
        if (cnt > 0) {
            cout << "노랑 타일 번호 입력 (예: 10): ";
            for (int i = 0; i < cnt; ++i) {
                cin >> tile;
                if (tile < 1 || tile > 13) {
                    cout << "❌ 잘못된 노랑 타일 번호: " << tile << "\n";
                }
                else if (yellow_tiles[tile] > 0) {
                    placed.emplace_back(YELLOW, tile);
                } else {
                    cout << "❌ 보유한 노랑 타일이 부족합니다: " << tile << "\n";
                }
            }
        }
        // 4) 검정 타일 (조커 포함, 조커는 number==0)
        cout << "# 놓을 검정 타일 개수 (예: 2): ";
        cin  >> cnt;
        num_of_table_black_tiles = cnt;
        if (cnt > 0) {
            cout << "검정 타일 번호 입력 (예: 11 0): ";
            for (int i = 0; i < cnt; ++i) {
                cin >> tile;
                if (tile == 0) {
                    // 조커
                    if (black_tiles[0] > 0) {
                        placed.emplace_back(BLACK, 0);
                    } else {
                        cout << "❌ 보유한 조커가 부족합니다.\n";
                    }
                }
                else if (tile < 1 || tile > 13) {
                    cout << "❌ 잘못된 검정 타일 번호: " << tile << "\n";
                }
                else if (black_tiles[tile] > 0) {
                    placed.emplace_back(BLACK, tile);
                } else {
                    cout << "❌ 보유한 검정 타일이 부족합니다: " << tile << "\n";
                }
            }
        }
        // 첫 멜드(초기 Meld) 조건 검사 (3장 이상, Set 또는 Run, 합계 ≥ 30)
        // is_user==true 이면서 아직 initial_meld_done 가 false 일 때만 검사
        if (is_user && !initial_meld_done) {
            int sum = 0;
            for (auto &p : placed) sum += p.second;
            bool valid = is_set(placed) || is_run(placed);
            if (!valid || sum < 30) {
                cout << "❌ 첫 멜드는 3장 이상의 Set 또는 Run, 합계 30 이상이어야 합니다. (현재 합계: "
                     << sum << ")\n";
                return;
            }
            initial_meld_done = true;
        } else { // 첫 번째 이후
            // 놓은 것만으로 Set/Run
            bool placedSet = is_set(placed);
            bool placedRun = is_run(placed);
            // 기존 테이블 멜드에 확장되는지 검사
            bool extension = false;
            // 같은 숫자(Set) 확장?
            if(!placed.empty()) {
                int num0 = placed[0].second;
                bool allSameNum = true;
                for(auto &p:placed) if(p.second != num0) { allSameNum=false; break; }

                if(allSameNum) {
                    // 기존 테이블에 몇 가지 색이 있는지 세고 placed 의 색도 합하기
                    set<int> colors;
                    if(table_red_tiles[num0]   > 0) colors.insert(RED);
                    if(table_blue_tiles[num0]  > 0) colors.insert(BLUE);
                    if(table_yellow_tiles[num0]> 0) colors.insert(YELLOW);
                    if(table_black_tiles[num0] > 0) colors.insert(BLACK);
                    // placed 에서 색 채우기
                    for(auto &p:placed) colors.insert(p.first);

                    if(colors.size() >= 3) extension = true;
                }
            }
            // -- 같은 색(Run) 확장?
            // placed 를 색별로 묶어서, 기존 테이블+placed 합친 숫자들이 연속되는지 검사
            map<int, vector<int>> byColor;
            for(auto &p:placed) {
                byColor[p.first].push_back(p.second);
            }
            for(auto &kv: byColor) {
                int color = kv.first;
                vector<int> merged;
                // 1~13 모두 검사
                for(int n=1;n<=13;++n) {
                    if(table_red_tiles[n]   >0 && color==RED)    merged.push_back(n);
                    if(table_blue_tiles[n]  >0 && color==BLUE)   merged.push_back(n);
                    if(table_yellow_tiles[n]>0 && color==YELLOW) merged.push_back(n);
                    if(table_black_tiles[n] >0 && color==BLACK)  merged.push_back(n);
                }
                // placed 에서도
                for(int n: kv.second) merged.push_back(n);

                // 중복 제거 & 정렬
                sort(merged.begin(), merged.end());
                merged.erase(unique(merged.begin(), merged.end()), merged.end());

                // 연속 검사
                if(merged.size() >= 3) {
                    bool consec = true;
                    for(size_t i=1;i<merged.size();++i){
                        if(merged[i] != merged[i-1] + 1) {
                            consec = false;
                            break;
                        }
                    }
                    if(consec) { extension = true; break; }
                }
            }
            if(!(placedSet || placedRun || extension)) {
                cout<<"❌ 놓은 타일이 유효한 Set/Run 이 아니고, 기존 멜드 확장도 아닙니다.\n";
                return;
            }
        }
        // 실제로 맵에 반영
        for (auto &p : placed) {
            int c = p.first, n = p.second;
            switch (c) {
                case RED:    red_tiles[n]--;    table_red_tiles[n]++;    break;
                case BLUE:   blue_tiles[n]--;   table_blue_tiles[n]++;   break;
                case YELLOW: yellow_tiles[n]--; table_yellow_tiles[n]++;  break;
                case BLACK:  /* n==0? 조커: 일반 검정 */ 
                             if (n == 0) black_tiles[0]--, table_black_tiles[0]++;
                             else       black_tiles[n]--, table_black_tiles[n]++;
                             break;
            }
            num_of_total_tiles--;
            num_of_table_total_tiles++;
        }
        //last_placed_tiles = placed;
        cout << "✅ 테이블에 타일을 놓았습니다.\n";
        // 
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

// 네트워크 연결 + 핸드셰이크
int connect_to_server(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){ perror("소켓 생성 실패"); return -1; }
    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port   = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &srv.sin_addr);
    if(connect(sock, (sockaddr*)&srv, sizeof(srv)) < 0){
        perror("서버 연결 실패"); close(sock);
        return -1;
    }
    return sock;
}

// // 1바이트씩이 아니라 n바이트를 딱 읽어주는 헬퍼
// static std::string read_n(int sock, size_t n) {
//     std::string s;
//     s.resize(n);
//     size_t received = 0;
//     while (received < n) {
//         int r = recv(sock, &s[received], n - received, 0);
//         if (r <= 0) throw std::runtime_error("recv error");
//         received += r;
//     }
//     return s;
// }

// SET_COUNT ↔ OK 핸드셰이크만 처리
// static std::string read_handshake_token(int sock) {
//     std::string buf;
//     char c;
//     while (true) {
//         if (recv(sock, &c, 1, 0) <= 0) throw std::runtime_error("recv error");
//         buf.push_back(c);
//         if (buf == "SET_COUNT" || buf == "OK") return buf;
//         // 둘 중 어느 것도 접두사가 아니면 에러
//         if (!(std::string("SET_COUNT").compare(0, buf.size(), buf) == 0 ||
//               std::string("OK").compare(0, buf.size(), buf) == 0)) {
//             throw std::runtime_error("unexpected handshake token: " + buf);
//         }
//     }
// }

// common.hpp 에서 BUFFER_SIZE >= 9 로 정의되어 있다고 가정
static bool do_handshake(int sock) {
    char buf[BUFFER_SIZE];

    // 1) 첫 클라이언트는 "SET_COUNT"(9) 아니면 OK(2) 둘 중 하나
    int n = recv(sock, buf, 9, 0);
    if (n <= 0) { perror("recv"); return false; }

    if (n == 9 && std::strncmp(buf, "SET_COUNT", 9) == 0) {
        // --- 첫 클라이언트 ---
        int cnt;
        do {
          std::cout << "몇 명이 플레이합니까? (2~4): ";
          std::cin  >> cnt;
        } while (cnt < 2 || cnt > 4);

        auto s = std::to_string(cnt);
        send(sock, s.c_str(), s.size(), 0);

        // OK(2) 받기
        n = recv(sock, buf, 2, 0);
        if (n != 2 || std::strncmp(buf, "OK", 2) != 0) {
          std::cerr << "서버가 OK를 보냈어야 하는데: " 
                    << std::string(buf, n) << "\n";
          return false;
        }
    }
    else if (n == 2 && std::strncmp(buf, "OK", 2) == 0) {
        // --- 두 번째 이후 클라이언트 ---
        // 그냥 넘어갑니다.
    }
    else {
        std::cerr << "핸드셰이크 프로토콜 오류: 예상 SET_COUNT(9) or OK(2), got "
                  << std::string(buf, n) << "\n";
        return false;
    }

    return true;
}

static bool wait_for_start(int sock) {
    char buf[BUFFER_SIZE];
    int n = recv(sock, buf, 5, 0);
    if (n != 5 || std::strncmp(buf, "START", 5) != 0) {
        std::cerr << "START 신호를 받아야 하는데: "
                  << std::string(buf, n) << "\n";
        return false;
    }
    show_ascii_art();
    std::cout << "========== GAME START ==========\n";
    return true;
}


void receiver_loop(int sock) {
    char buf[BUFFER_SIZE];
    while (true) {
        int n = recv(sock, buf, sizeof(buf)-1, 0);
        if (n <= 0) break;
        buf[n] = '\0';

        if (std::strncmp(buf, "UPDATE", 6) == 0) {
            // "UPDATE <color> <num>"
            int color, num;
            std::sscanf(buf+7, "%d %d", &color, &num);
            switch(color) {
                case RED:    table_red_tiles[num]++;    break;
                case BLUE:   table_blue_tiles[num]++;   break;
                case YELLOW: table_yellow_tiles[num]++; break;
                case BLACK:  table_black_tiles[num]++;  break;
            }
            num_of_table_total_tiles++;
            print_table_tiles();
        }
        else if (std::strcmp(buf, "YOUR_TURN") == 0) {
            std::cout<<"\n[알림] 당신 차례입니다.\n";
            // main 루프에서 입력 받도록 플래그 세워도 좋음
        }
    }
}
// 서버에 방금 놓은 타일들 전송
void send_placed_to_server(int sock) {
    for (auto &p : last_placed_tiles) {
        char msg[32];
        int L = std::snprintf(msg, sizeof(msg), "PLACE %d %d", p.first, p.second);
        send(sock, msg, L, 0);
    }
}
void sigint_handler(int) {
    if (sock >= 0) ::close(sock);
    std::exit(1);
}
int main() {
    show_ascii_art();
    // 시그널 핸들러 등록
    std::signal(SIGINT, sigint_handler);
    std::signal(SIGTERM, sigint_handler);
    // 서버 연결
    int sock = connect_to_server();
    if(sock < 0) return 1;
    // 첫 클라이언트만 핸드셰이크
    // * 두 번째 이후 클라이언트는 do_handshake() 호출 안 함!
    if (!do_handshake(sock)) return 1;
    if (!wait_for_start(sock)) return 1;

    // START 신호 대기
    // 백그라운드 수신 스레드 실행
    std::thread receiver(receiver_loop, sock);
    receiver.detach();

    // 메시지 수신 전담 스레드
    // std::thread receiver([&](){
        // char buf[BUFFER_SIZE];
        // while (true) {
        //     int n = recv(sock, buf, sizeof(buf)-1, 0);
        //     if (n <= 0) break;
        //     buf[n] = '\0';

        //     if (strncmp(buf, "UPDATE", 6) == 0) {
        //         // "UPDATE <color> <num>"
        //         int color, num;
        //         sscanf(buf+7, "%d %d", &color, &num);
        //         // 여기에 table_* 맵을 직접 수정하거나
        //         // print_table_tiles() 호출 전에 반영할 수 있도록 전역변수에 저장
        //         // 예를 들면:
        //         switch(color) {
        //         case RED:   table_red_tiles[num]++;   break;
        //         case BLUE:  table_blue_tiles[num]++;  break;
        //         case YELLOW:table_yellow_tiles[num]++;break;
        //         case BLACK: table_black_tiles[num]++; break;
        //         }
        //         num_of_table_total_tiles++;
        //         // 그리고 바로 화면에 갱신
        //         print_table_tiles();
        //     }
        //     else if (strcmp(buf, "YOUR_TURN") == 0) {
        //         // 자신의 차례 알림
        //         std::cout << "\n[네트워크] 당신의 차례입니다. 테이블에 낼 타일을 선택하세요.\n";
        //         // 여기는 main 루프 쪽에서 처리하도록 플래그를 세워도 됩니다.
        //     }
        // }
    // });
    // receiver.detach();

     // 2) 첫 클라이언트는 서버로부터 SET_COUNT 신호를 받는다
    // char buf[BUFFER_SIZE];
    // int n = recv(sock, buf, sizeof(buf)-1, 0);
    // if (n <= 0) { perror("recv"); close(sock); return 1; }
    // buf[n] = '\0';

    // if (string(buf) == "SET_COUNT") {
    //     // 플레이어 수 입력
    //     int target;
    //     do {
    //         cout << "몇 명이 플레이합니까? (2~4): ";
    //         cin  >> target;
    //     } while (target < 2 || target > 4);

    //     // 서버에 전송
    //     auto s = to_string(target);
    //     send(sock, s.c_str(), s.size(), 0);

    //     // 서버의 OK 응답 대기
    //     n = recv(sock, buf, sizeof(buf)-1, 0);
    //     if (n <= 0 || string(buf,n) != "OK") {
    //         cerr << "서버가 설정을 수락하지 않았습니다.\n";
    //         close(sock);
    //         return 1;
    //     }
    // }
    // 두 번째 이후 클라이언트는 아무것도 보내지 않고 바로 START 대기

    // 3) 서버가 보낸 START 메시지 대기
    // n = recv(sock, buf, sizeof(buf)-1, 0);
    // if (n <= 0) { perror("recv"); close(sock); return 1; }
    // buf[n] = '\0';
    // if (string(buf) != "START") {
    //     cerr << "게임 시작 신호를 받지 못했습니다: " << buf << "\n";
    //     close(sock);
    //     return 1;
    // }

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
        cout<<"0. 종료"<<endl;
        cout<<"\n번호 선택 : ";
        cin>>button;
       
        
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
            send_placed_to_server(sock);
        }
        else if(button==0){
            cout<<"프로그램을 종료합니다.\n";
            game_end();
        }
        else {
            error_msg();
        }
    }
    close(sock);
    return 0;
}
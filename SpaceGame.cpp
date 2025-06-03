#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <conio.h>
#include <stdlib.h>


using namespace std;

// Rocket - Y
// Bullet - ^
// Enemy - E
// Enemy Bullet - *
// Enemy_Bullet with Bullet - %


int HP = 3;
int gun_reload = 10;

int height = 20;
int width = 40;

int x_pos = 25;
int y_pos = 15;

vector <vector<int>> bullet_pos = {};
vector <vector<int>> enemy_pos = {};
vector <vector<int>> enemy_bullet_pos = {};

random_device rd;
uniform_int_distribution<> enemy_distribution(1, width-1);

int score = 0;

void Draw();
void Keyboard();
void Timers();
void Logic();
void EndingScreen();
int CalculateTime(clock_t diff);

bool is_End = false;

clock_t start_keyboard;
clock_t start_bullet;
clock_t start_drawing;
clock_t start_bullet_speed_set;
clock_t start_enemy_spawn;
clock_t start_enemy_speed_set;
vector<clock_t> start_bullet_enemy;
clock_t start_bullet_enemy_speed_set;

clock_t bullet_pause;
clock_t keyboard_pause;
clock_t drawing_pause;
clock_t bullet_speed_set;
clock_t enemy_spawn;
clock_t enemy_speed_set;
clock_t bullet_enemy_speed_set;

int bullet_pause_msec;
int keyboard_pause_msec;
int drawing_pause_msec;
int bullet_speed_set_msec;
int enemy_spawn_msc;
int enemy_speed_set_msec;
vector<int> bullet_enemy_msec;
int bullet_enemy_speed_set_msec;

int main()
{

    start_keyboard = clock();
    start_bullet = clock();
    start_drawing = clock();
    start_bullet_speed_set = clock();
    start_enemy_spawn = clock();
    start_enemy_speed_set = clock();
    start_bullet_enemy_speed_set = clock();
    
    while (!is_End) {
        if (keyboard_pause_msec >= 50) {
            Keyboard();
            start_keyboard = clock();
        }
        Logic();
        if (drawing_pause_msec >= 2) {
            Draw();
            start_drawing = clock();
        }
        Timers();
    }
    EndingScreen();
}

void Draw() {
    system("cls");
    int bul_cnt = 0;
    int en_cnt = enemy_pos.size()-1;
    int en_bul_cnt = enemy_bullet_pos.size() - 1;
    for (int i = 0; i <= height; i++) {
        for (int j = 0; j <= width; j++) {
            if (i == y_pos && j == x_pos) {
                cout << 'Y';
            }
            else if (bullet_pos.size() != 0 && bullet_pos.size()-1 >= bul_cnt && i == bullet_pos[bul_cnt][1] && j == bullet_pos[bul_cnt][0]) {
                cout << '^';
                bul_cnt++;
            }
            else if (en_cnt >= 0 && i == enemy_pos[en_cnt][1] && j == enemy_pos[en_cnt][0]) {
                cout << 'E';
                en_cnt--;
            }
            else if (i == 0 || i == height) {
                cout << '#';
            }
            else if (j == 0 || j == width) {
                cout << '#';
            }
            else if (en_bul_cnt >= 0) {
                bool is_drawed = false;
                for (int k = 0; k < enemy_bullet_pos.size(); k++) {
                    if (enemy_bullet_pos[k][1] == i && enemy_bullet_pos[k][0] == j) {
                        cout << '*';
                        is_drawed = true;
                        en_bul_cnt--;
                    }
                }
                if (!is_drawed) {
                    cout << ' ';
                }
            }
            else { cout << ' '; }
        }
        if (i == height) {
            cout << "  " << score << endl;
        }
        else if (i == height - 3) {
            cout << "  HP: ";
            for (int i = 0; i < HP; i++) {
                cout << "+ ";
            }
            cout << endl;
        }
        else {
            cout << endl;
        }
    }
    cout << endl;
    cout << "|";
    for (int i = 0; i < 10; i++) {
        if (i >= gun_reload) {
            cout << '.';
        }
        else {
            cout << '!';
        }
    }
    cout << "|";
}

void Keyboard() {
    if (_kbhit()) {
        char button = _getch();
        if (button == 'w' || button == 'W') { // Forward
            y_pos--;
        }
        
        else if (button == 'd' || button == 'D') { // Right
            x_pos++;
        } 
        
        else if (button == 's' || button == 'S') { // Back
            y_pos++;
        }
        
        else if (button == 'a' || button == 'A') { // Left
            x_pos--;
        }
        
        if (button == ' ' && bullet_pause_msec >= 1000) { // Gun
            bullet_pos.push_back({x_pos, y_pos-1});
            start_bullet = clock();
        }

        if (button == 'o' || button == 'O') { is_End = true; }
    }
}

void Timers() {
    bullet_pause = clock() - start_bullet;
    keyboard_pause = clock() - start_keyboard;
    drawing_pause = clock() - start_drawing;
    bullet_speed_set = clock() - start_bullet_speed_set;
    enemy_spawn = clock() - start_enemy_spawn;
    enemy_speed_set = clock() - start_enemy_speed_set;
    bullet_enemy_speed_set = clock() - start_bullet_enemy_speed_set;

    for (int i = 0; i < start_bullet_enemy.size(); i++) {
        bullet_enemy_msec[i] = CalculateTime((clock() - start_bullet_enemy[i]));
    }

    keyboard_pause_msec = CalculateTime(keyboard_pause);
    bullet_pause_msec = CalculateTime(bullet_pause);
    drawing_pause_msec = CalculateTime(drawing_pause);
    bullet_speed_set_msec = CalculateTime(bullet_speed_set);
    enemy_spawn_msc = CalculateTime(enemy_spawn);
    enemy_speed_set_msec = CalculateTime(enemy_speed_set);
    bullet_enemy_speed_set_msec = CalculateTime(bullet_enemy_speed_set);
}

void Logic() {
    if (y_pos >= height) { y_pos = height - 1; }
    else if (y_pos <= 0) { y_pos = 1; }
    else if (x_pos >= width) { x_pos = width - 1; }
    else if (x_pos <= 0) { x_pos = 1; }

    
    for (int i = 0; i < bullet_pos.size(); i++) {
       if (bullet_speed_set_msec >= 30) {
           bullet_pos[i][1] -= 1;
           start_bullet_speed_set = clock();
       }
       if (bullet_pos[i][1] <= 0) {
            bullet_pos.erase(bullet_pos.begin()+i);
       }
       else {
           for (int j = 0; j < enemy_pos.size(); j++) {
                if (bullet_pos[i][0] == enemy_pos[j][0] && bullet_pos[i][1] == enemy_pos[j][1]) {
                    enemy_pos.erase(enemy_pos.begin() + j);
                    start_bullet_enemy.erase(start_bullet_enemy.begin() + j);
                    bullet_enemy_msec.erase(bullet_enemy_msec.begin() + j);
                    bullet_pos.erase(bullet_pos.begin() + i);
                    score += 10;
                    break;
                }
           }
       }
    }
        

    if (enemy_spawn_msc >= 4000-score*2) {
        mt19937 gen(rd());
        int random_x_pos = enemy_distribution(gen);
        enemy_pos.push_back({random_x_pos, 1});
        start_bullet_enemy.push_back(clock());
        bullet_enemy_msec.push_back(0);
        start_enemy_spawn = clock();
    }

    
    for (int i = 0; i < enemy_pos.size(); i++) {
        if (enemy_speed_set_msec >= 700) { 
            enemy_pos[i][1] += 1; 
            start_enemy_speed_set = clock();
        }
        if (enemy_pos[i][1] >= height) {
            enemy_pos.erase(enemy_pos.begin() + i);
            start_bullet_enemy.erase(start_bullet_enemy.begin() + i);
            bullet_enemy_msec.erase(bullet_enemy_msec.begin() + i);
        }
        else if (enemy_pos[i][0] == x_pos && enemy_pos[i][1] == y_pos) {
            enemy_pos.erase(enemy_pos.begin() + i);
            start_bullet_enemy.erase(start_bullet_enemy.begin() + i);
            bullet_enemy_msec.erase(bullet_enemy_msec.begin() + i);
            HP -= 2;
        }

        
    }
    for (int i = 0; i < bullet_enemy_msec.size(); i++) {
        if (bullet_enemy_msec[i] >= 2000) {
            enemy_bullet_pos.push_back({enemy_pos[i][0], enemy_pos[i][1]+1});
            start_bullet_enemy[i] = clock();
        }
    }

    
    for (int i = 0; i < enemy_bullet_pos.size(); i++) {
        if (bullet_enemy_speed_set_msec >= 100) {
            enemy_bullet_pos[i][1]++;
            start_bullet_enemy_speed_set = clock();
        }
        if (enemy_bullet_pos[i][1] >= height) {
            enemy_bullet_pos.erase(enemy_bullet_pos.begin() + i);
        }
        else if (enemy_bullet_pos[i][0] == x_pos && enemy_bullet_pos[i][1] == y_pos) { 
            HP--; 
            enemy_bullet_pos.erase(enemy_bullet_pos.begin() + i);
        }
    }
        

    if (bullet_pause >= 1000) {
        gun_reload = 10;
    }
    else {
        gun_reload = bullet_pause/100;
    }

    if (HP <= 0) { is_End = true; }
}

void EndingScreen() {
    system("cls");
    cout << "You have died  :(" << endl;
    cout << "Your score - " << score << endl;
    cout << "Try beating " << score << " next time!" << endl;

    while (_getch() != 'o') {}
}

int CalculateTime(clock_t diff) {
    return diff * 1000 / CLOCKS_PER_SEC;
}
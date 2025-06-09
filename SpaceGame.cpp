#include <iostream>
#include <vector>
#include <random>
#include <time.h> // Work with clocks
#include <string>
#include <conio.h> // Keyboard handling
#include <fstream> // Work with txt data files

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

int x_pos = 20;
int y_pos = 15;

vector <vector<int>> bullet_pos = {};
vector <vector<int>> enemy_pos = {};
vector <vector<int>> enemy_bullet_pos = {};

random_device rd;
uniform_int_distribution<> enemy_distribution(1, width-1);

int score = 0;
int total_score = 0;

void Draw();
void Keyboard();
void Timers();
void Logic();
void EndingScreen();
void Shop();
void GameInit();
int CalculateTime(clock_t diff);
bool FindSkin(char skin);
bool DataFileExists();
void UpdateData(char append);
int ParseScore(string line);
void CreateFile();

bool is_End = false;
bool is_Dead = false;
bool prog_run = true;
bool is_shop = false;

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



std::vector<char> shop_skins = {'Y', 'V', 'A', 'U', 'N', 'H', 'Z', 'X', 'W'};
std::vector<int> shop_prices = {0, 50, 100, 250, 250, 400, 800, 1500, 3000};

std::vector<char> bought_skins = { 'Y' };
char chosen_skin = shop_skins[0];



int main()
{
    while (prog_run) {
        GameInit();

        while (!is_End && !is_Dead) {
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
        if (!DataFileExists) {
            CreateFile();
        }
        UpdateData(' ');
        if (is_Dead) { 
            EndingScreen(); 
            score = 0;
        }
        if (is_shop) { Shop(); }
    }
    
}

void Draw() {
    system("cls");
    int bul_cnt = 0;
    int en_cnt = enemy_pos.size()-1;
    int en_bul_cnt = enemy_bullet_pos.size() - 1;
    for (int i = 0; i <= height; i++) {
        for (int j = 0; j <= width; j++) {
            if (i == y_pos && j == x_pos) {
                cout << chosen_skin;
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

        if (button == 'o' || button == 'O') { 
            is_End = true;
            prog_run = false;
        }
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

    if (HP <= 0) { is_Dead = true; }
}

void EndingScreen() {
    system("cls");
    cout << "You have died  :(" << endl;
    cout << "Your score - " << score << endl;
    cout << "Try beating " << score << " next time!" << endl;

    bool exit = false;
    while (!exit) {
        char button = _getch();
        if (button == 'r') {
            is_Dead = false;
            is_End = false;
            exit = true;
        }
        else if (button == 'o') {
            prog_run = false;
            exit = true;
        }
        else if (button == 's') {
            is_shop = true;
            exit = true;
        }
    }
}

void Shop() {
    system("cls");
    cout << "--------THE STORE--------" << endl << endl;
    cout << "Score: " << total_score << endl << endl;

    for (int i = 0; i < 9; i++) {
        cout << shop_skins[i] << ": " << shop_prices[i] << "  ";
        if (FindSkin(shop_skins[i])) {cout << "OWNED" << endl;}
        else if (i == 0) { cout << "STARTER SKIN" << endl; }
        else { cout << "UNAVAILABLE" << endl; }
    }
    cout << endl;
    cout << "If you want to buy a new skin, you should press the key button with that skin" << endl;

    bool exit = false;

    char add = ' ';

    while (!exit) {
        char button = _getch();
        if (button == 'r') {
            is_Dead = false;
            is_End = false;
            is_shop = false;
            exit = true;
        }
        else if (button == 'o') {
            prog_run = false;
            exit = true;
        }

        switch (button) {
        case 'y':
            cout << "You already have Y skin" << endl;
            cout << "Do you want to equip it? (Y/N)" << endl;
            char agr;
            cin >> agr;
            if (agr == 'Y' || agr == 'y') {
                chosen_skin = shop_skins[0];
                is_Dead = false;
                is_End = false;
                is_shop = false;
                exit = true;
            }
            break;

        case 'v':
            if (FindSkin(shop_skins[1])) {
                cout << "You already have V skin" << endl;
                cout << "Do you want to equip it? (Y/N)" << endl;
                char agr;
                cin >> agr;
                if (agr == 'Y' || agr == 'y') {
                    chosen_skin = shop_skins[1];
                    is_Dead = false;
                    is_End = false;
                    is_shop = false;
                    exit = true;
                }
            }
            else if (shop_prices[1] <= total_score) {
                score = -shop_prices[1];
                chosen_skin = shop_skins[1];
                add = shop_skins[1];
                is_Dead = false;
                is_End = false;
                is_shop = false;
                exit = true;
            }
            else { cout << "You Cannot afford V" << endl; }
            break;

        case 'a':
            if (FindSkin(shop_skins[2])) {
                cout << "You already have A skin" << endl;
                cout << "Do you want to equip it? (Y/N)" << endl;
                char agr;
                cin >> agr;
                if (agr == 'Y' || agr == 'y') {
                    chosen_skin = shop_skins[2];
                    is_Dead = false;
                    is_End = false;
                    is_shop = false;
                    exit = true;
                }
                else { break; }
            }
            else if (shop_prices[2] <= total_score) {
                score = -shop_prices[2];
                chosen_skin = shop_skins[2];
                add = shop_skins[2];
                is_Dead = false;
                is_End = false;
                is_shop = false;
                exit = true;
            }
            else { cout << "You Cannot afford A" << endl; }
            break;
        case 'u':
            if (FindSkin(shop_skins[3])) {
                cout << "You already have U skin" << endl;
                cout << "Do you want to equip it? (Y/N)" << endl;
                char agr;
                cin >> agr;
                if (agr == 'Y' || agr == 'y') {
                    chosen_skin = shop_skins[3];
                    is_Dead = false;
                    is_End = false;
                    is_shop = false;
                    exit = true;
                }
                else { break; }
            }
            else if (shop_prices[3] <= total_score) {
                score = -shop_prices[3];
                chosen_skin = shop_skins[3];
                add = shop_skins[3];
                is_Dead = false;
                is_End = false;
                is_shop = false;
                exit = true;
            }
            else { cout << "You Cannot afford U" << endl; }
            break;
        case 'n':
            if (FindSkin(shop_skins[4])) {
                cout << "You already have N skin" << endl;
                cout << "Do you want to equip it? (Y/N)" << endl;
                char agr;
                cin >> agr;
                if (agr == 'Y' || agr == 'y') {
                    chosen_skin = shop_skins[4];
                    is_Dead = false;
                    is_End = false;
                    is_shop = false;
                    exit = true;
                }
                else { break; }
            }
            else if (shop_prices[4] <= total_score) {
                score = -shop_prices[4];
                chosen_skin = shop_skins[4];
                add = shop_skins[4];
                is_Dead = false;
                is_End = false;
                is_shop = false;
                exit = true;
            }
            else { cout << "You Cannot afford N" << endl; }
            break;
        case 'h':
            if (FindSkin(shop_skins[5])) {
                cout << "You already have H skin" << endl;
                cout << "Do you want to equip it? (Y/N)" << endl;
                char agr;
                cin >> agr;
                if (agr == 'Y' || agr == 'y') {
                    chosen_skin = shop_skins[5];
                    is_Dead = false;
                    is_End = false;
                    is_shop = false;
                    exit = true;
                }
                else { break; }
            }
            else if (shop_prices[5] <= total_score) {
                score = -shop_prices[5];
                chosen_skin = shop_skins[5];
                add = shop_skins[5];
                is_Dead = false;
                is_End = false;
                is_shop = false;
                exit = true;
            }
            else { cout << "You Cannot afford H"<< endl; }
            break;
        case 'z':
            if (FindSkin(shop_skins[6])) {
                cout << "You already have Z skin" << endl;
                cout << "Do you want to equip it? (Y/N)" << endl;
                char agr;
                cin >> agr;
                if (agr == 'Y' || agr == 'y') {
                    chosen_skin = shop_skins[6];
                    is_Dead = false;
                    is_End = false;
                    is_shop = false;
                    exit = true;
                }
                else { break; }
            }
            else if (shop_prices[6] <= total_score) {
                score = -shop_prices[6];
                chosen_skin = shop_skins[6];
                add = shop_skins[6];
                is_Dead = false;
                is_End = false;
                is_shop = false;
                exit = true;
            }
            else { cout << "You Cannot afford Z" << endl; }
            break;
        case 'x':
            if (FindSkin(shop_skins[7])) {
                cout << "You already have X skin" << endl;
                cout << "Do you want to equip it? (Y/N)" << endl;
                char agr;
                cin >> agr;
                if (agr == 'Y' || agr == 'y') {
                    chosen_skin = shop_skins[7];
                    is_Dead = false;
                    is_End = false;
                    is_shop = false;
                    exit = true;
                }
                else { break; }
            }
            else if (shop_prices[7] <= total_score) {
                score = -shop_prices[7];
                chosen_skin = shop_skins[7];
                add = shop_skins[7];
                is_Dead = false;
                is_End = false;
                is_shop = false;
                exit = true;
            }
            else { cout << "You Cannot afford X"<< endl; }

            break;
        case 'w':
            if (FindSkin(shop_skins[8])) {
                cout << "You already have W skin" << endl;
                cout << "Do you want to equip it? (Y/N)" << endl;
                char agr;
                cin >> agr;
                if (agr == 'Y' || agr == 'y') {
                    chosen_skin = shop_skins[8];
                    is_Dead = false;
                    is_End = false;
                    is_shop = false;
                    exit = true;
                }
                else { break; }
            }
            else if (shop_prices[8] <= total_score) {
                score = -shop_prices[8];
                chosen_skin = shop_skins[8];
                add = shop_skins[8];
                is_Dead = false;
                is_End = false;
                is_shop = false;
                exit = true;
            }
            else { cout << "You Cannot afford W"<< endl; }
            break;
        default:
            break;
        }
    }
    UpdateData(add);
}

bool FindSkin(char skin) {
    int len = bought_skins.size();

    for (int i = 0; i < len; i++) {
        if (skin == bought_skins[i]) {
            return true;
        }
    }
    return false;

}

int CalculateTime(clock_t diff) {
    return diff * 1000 / CLOCKS_PER_SEC;
}

bool DataFileExists() {
    ifstream file("data.txt");

    return file.good();
}

void UpdateData(char append) {
    ifstream file("data.txt");
    string line1 = "";
    string line2 = "";
    if (file.is_open()) {
        getline(file, line1);
        getline(file, line2);
    }
    file.close();

    if (append != ' ') {
        line2 += append;
    }

    int len = line2.length();
    bought_skins = {};
    for (int i = len - 1; i >= 0; i--) {
        if (line2[i] == ':') { break; }
        else {
            bought_skins.push_back(line2[i]);
        }
    }

    ofstream _file;
    _file.open("data.txt");
    if (_file.is_open()) {
        total_score = score + ParseScore(line1);
        _file << "s:" << total_score << endl;
        _file << line2;
    }
    _file.close();
}

int ParseScore(string line){
    int len = line.length();
    int summ = 0;
    for (int i = len-1; i >= 0; i--) {
        if (line[i] == ':') {break;}
        else { summ += ((int)line[i] - 48) * pow(10, len-1-i); }
    }

    return summ;
}

void CreateFile() {
    ofstream file("data.txt", ios::out | ios::trunc);

    if (file.is_open()) {
        file << "s:0" << endl << "r:0";
        file.close();
    }
}

void GameInit(){
    bullet_pos = {};
    enemy_bullet_pos = {};
    enemy_pos = {};
    bullet_enemy_msec = {};
    bullet_enemy_speed_set = {};
    start_bullet_enemy = {};

    x_pos = 20;
    y_pos = 15;

    score = 0;
    HP = 3;
    gun_reload = 10;



    start_keyboard = clock();
    start_bullet = clock();
    start_drawing = clock();
    start_bullet_speed_set = clock();
    start_enemy_spawn = clock();
    start_enemy_speed_set = clock();
    start_bullet_enemy_speed_set = clock();
}

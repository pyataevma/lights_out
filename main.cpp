#include <iostream>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <string>

using namespace std;
//===============Constants describing colors and possible values of control variables==========
//int active_back_col = 2, idle_back_col = 7, active_text_col = 2, idle_text_col = 7;
const int light = 1, dark = 0, light_active = 2, dark_active = 4; // indicator states on the field
const int LeftSide = 0, MiddleSide = 1, RightSide = 2;  // type of vertical border
const int actionQuit = 1, actionPlay = 2, actionRequestParameters=3, actionAutoPlay=4, actionNewGame=5, actionRunMenu=6; // event handlers
const int keyDoNothing = 1, keyUpdateMenu = 2, keyWin = 3; // control commands that are not keyboard codes
const string bottom = { char(192),char(196),char(193),char(217) }; // border elements
const string top = { char(218),char(196),char(194),char(191) };
const string middle = { char(195),char(196),char(197),char(180) };

//===================Structures===================================================================
struct Menu
{
    int items_number;
    int active;
    string* items_text;
    int* actions; // array with action numbers corresponding to the text array items_text
    //int active_col, idle_col;
};

struct Position
{
    int x, y;
};

struct Board
{    // Game board parameters
    int board_size; // linear size of the board (number of cells)
    int screen_width; // number of characters in a screen row
    int margin_left; // left margin
    int margin_top; // top margin
    int margin_bottom; // bottom margin
    int total_lines; // total number of lines occupied by the game board
    int switched_on; // number of "switched on" indicators
    int steps_to_solve; // number of switches required to solve
    int steps_done; // number of moves made
    bool display_steps; // display the number of moves left for an optimal solution?
    Position active; // coordinates of the field where the cursor is located
    int* map; // array - game board, active fields are encoded by bits by x and array index by y
    int* sol; // coordinates of the fields to be switched to solve (x - bit number, y - number in the array)
};

struct Parameters {
    // variable to store settings
    int board_size;
    int number_of_menu_strings;
    int number_of_info_strings;
    bool display_steps;
};

/*struct ColoredLine {
    // template for colored screen
    int number_of_segments;
    int* positions;
    int* colors;
};
*/

struct ColoredText {
    // the name implies inclusion of colors
    int number_of_strings;
    string* st;
    int current_string = 0;
};

struct ExchangeVariable {
    // variable for data exchange between functions
    int action; // event handler active at the current moment
    char key; // code to perform an action, including the code of the pressed key, but not limited to it
    bool text_changed; // has the text (image on the screen) changed?
};

//==========Additional constants============

const Position LeftTop = {};

//========== Screen functions =====================================
//
// To speed up animation, all pseudo-graphic images are first written to an array of strings (ColoredText), and then
// displayed on the screen via cout immediately after clearing the screen.

char to866(char ch)
// After many attempts to beat setlocale, I decided to write a manual text literal transcoding function.
{
    if ((ch >= 'À') && (ch <= 'ï'))
    {
        return ch - 64;
    }
    if ((ch >= 'ð') && (ch <= 'ÿ'))
    {
        return ch - 16;
    }
    if (ch == '¸')
    {
        return char(-15);
    }
    if (ch == '¨')
    {
        return char(-16);
    }
    return ch;
}

string correctEncoding(string st)
{
    for (int i = 0; i < st.size(); i++)
    {
        st[i] = to866(st[i]);
    }
    return st;
}

void init_text(ColoredText* T, Board B, Parameters P)
{
    T->number_of_strings = (2 * B.board_size + 1) +B.margin_top+P.number_of_menu_strings+P.number_of_info_strings;
    //(2n+1 lines for the board, + top margin + 1 for menu + information at the bottom);
    T->st = new string[T->number_of_strings];
    T->current_string = 0;
}

void delete_text(ColoredText* T)
{
    delete[] T->st;
}

/*void display_line(HANDLE h, string s, ColoredLine CL)
{
    // Display colored line (template)
    for (int i = 0; i < CL.number_of_segments; i++)
    {
        SetConsoleTextAttribute(h, CL.colors[i]);
        if (i < CL.number_of_segments - 1)
        {
            cout << s.substr(CL.positions[i], CL.positions[i + 1] - 1);
        }
        else
        {
            cout << s.substr(CL.positions[i]);
        }
    }
}
*/

void display_text(ColoredText T, HANDLE h)
{
    for (int i = 0; i < T.number_of_strings; i++)
    {
        cout << T.st[i] << "\n";
        //display_line(h, T.st[i], CL); // for the colored version
    }
}

//============ "Drawing" the game board =================

string s_left_margin(Board B)
{
    string s = "";
    for (int i = 0; i < B.margin_left; i++)
    {
        s += " ";
    }
    return s;
}

string s_horizontal_border(Board B, string bord_el)
{
    // bord_el - border elements - string with border elements (4 characters)
    string s = s_left_margin(B) + string(1, bord_el[0]);
    for (int j = 0; j < B.board_size; j++)
    {
        s += string(1, bord_el[1]) + string(1, bord_el[1]);
        if (j < B.board_size - 1)
        {
            s += string(1, bord_el[2]);
        }
    }
    s += string(1, bord_el[3]);
    return s;
}

string s_vertical_border(Board B, int side)
{
    string s;
    switch (side)
    {
    case LeftSide:
        s += s_left_margin(B) + string(1, char(179));
        break;
    case MiddleSide:
        s = string(1, char(179));
        break;
    case RightSide:
        s = string(1, char(179));
        break;
    }
    return s;
}

string s_element(int state)
{
    // element of the game board (two characters depending on the state of the element)
    string s = "";
    switch (state)
    {
    case light:
        s += string(1, char(219)) + string(1, char(219));
        break;
    case dark:
        //s += string(1, char(176)) + string(1, char(176));
        s += string(1, char(32)) + string(1, char(32));
        break;
    case dark_active:
        s += string(1, char(60)) + string(1, char(62));
        break;
    case light_active:
        s += string(1, char(17)) + string(1, char(16));
    }
    return s;
}

void add_board_to_text(Board B, ColoredText* T)
{
    // the function transfers the board drawing to the string array T
    int px = 1;
    for (int i = 0; i < B.margin_top; i++)
    {
        T->st[T->current_string++] = "";
    }
    T->st[T->current_string] = s_horizontal_border(B, top);
    for (int iy = 0; iy < B.board_size; iy++)
    {
        px = 1;
        //cs=T->current_string + 2 * iy + 1;
        T->current_string++;
        T->st[T->current_string] = s_vertical_border(B, LeftSide);
        for (int ix = 0; ix < B.board_size; ix++)
        {

            if (px & B.map[iy])
            {
                if ((B.active.x == ix) && (B.active.y == iy))
                {
                    T->st[T->current_string] += s_element(light_active);
                }
                else
                {
                    T->st[T->current_string] += s_element(light);
                }
            }
            else
            {
                if ((B.active.x == ix) && (B.active.y == iy))
                {
                    T->st[T->current_string] += s_element(dark_active);
                }
                else
                {
                    T->st[T->current_string] += s_element(dark);
                }

            }
            px *= 2;
            if (ix < B.board_size - 1)
            {
                T->st[T->current_string] += s_vertical_border(B, MiddleSide);
            }

        }
        T->st[T->current_string] += s_vertical_border(B, RightSide);
        T->current_string++;
        if (iy < B.board_size - 1)
        {
            //cs=T->current_string + 2 * iy + 2;
            T->st[T->current_string] = s_horizontal_border(B, middle);
        }
    }
    T->st[T->current_string++] = s_horizontal_border(B, bottom);
    for (int i = 0; i < B.margin_bottom; i++)
    {
        T->st[T->current_string++] = "";
    }
}


// ================== Output additional information =========================

string text_separator(int len)
{
    string s;
    for (int i = 0; i < len; i++)
    {
        s += "_";
    }
    return s;
}

void add_info_to_text(Board* B, ColoredText* T, ExchangeVariable* EV)
{
    // adds auxiliary information to the string array T
    T->st[T->current_string++] = text_separator(20)+ correctEncoding(" Key assignments: ")+ text_separator(20);
    T->st[T->current_string++] = "";
        switch (EV->action)
    {
    case actionPlay:
        T->st[T->current_string++] = correctEncoding("Esc - return to the main menu");
        T->st[T->current_string++] = correctEncoding("Arrows - move around the board");
        T->st[T->current_string++] = correctEncoding("Space - toggle");
        T->st[T->current_string++] = correctEncoding("Q - instant exit");
        T->st[T->current_string++] = correctEncoding("S - show hint (the cursor will automatically move to the desired cell)");
        T->st[T->current_string++] = "";
        T->st[T->current_string++] = text_separator(20) + correctEncoding(" Game parameters: ") + text_separator(20);
        T->st[T->current_string++] = "";
        T->st[T->current_string++] = correctEncoding("Enabled ")+to_string(B->switched_on)+ correctEncoding(" indicators.");
        if (B->display_steps)
        {
            T->st[T->current_string++] = correctEncoding("To turn off all, you need to make ") + to_string(B->steps_to_solve) + correctEncoding(" switches.");
        }
        T->st[T->current_string++] = correctEncoding("Made ") + to_string(B->steps_done) + correctEncoding(" switches.");
    break;
    case actionRunMenu:
        T->st[T->current_string++] = correctEncoding("Arrows - select menu item");
        T->st[T->current_string++] = correctEncoding("Enter - execute selected item");
        T->st[T->current_string++] = correctEncoding("Q - instant exit");
        break;
    }
    T->st[T->current_string++] = text_separator(60);
    // the remaining lines of the array are filled with empty values
    while (T->current_string < T->number_of_strings)
    {
        T->st[T->current_string++] = "";
    }
}

void add_congratulation(Board* B, ExchangeVariable* EV, ColoredText* T)
{
    T->st[T->current_string++] = "";
    T->st[T->current_string++] = correctEncoding(" Congratulations! You won! ");
    T->st[T->current_string++] = "";
    T->st[T->current_string++] = correctEncoding(" It took you ") + to_string(B->steps_done) + correctEncoding(" moves.");
    T->st[T->current_string++] = "";
    T->st[T->current_string++] = correctEncoding(" Press any key. ");
    EV->action = actionRunMenu;
    while (T->current_string < T->number_of_strings)
    {
        T->st[T->current_string++] = "";
    }
}

void request_parameters(Parameters* P, ExchangeVariable* EV)
{
    system("cls");
    cout << correctEncoding("Game settings \"Lights out (special edition)\".\n");
    cout << correctEncoding("A new game will start after the changes.\n");
    cout << correctEncoding("Specify the board size (even number from 4 to 10) -> ");
    cin >> P->board_size; // input validation will be added later
    cout << correctEncoding("Do you want to display the number of optimal solution moves? (\'Y\' - yes, any other symbol - no) -> ");
    char ch;
    cin >> ch;
    P->display_steps = (ch == 'y' || ch == 'Y');
}

void display_introduction()
{
    system("cls");
    cout << correctEncoding("\nGame \"Lights out (special edition)\".\n\n");
    cout << correctEncoding("The goal of the game is to turn off all the indicators on the control panel.\n");
    cout << correctEncoding("But when you switch any indicator, all the indicators in the same row and the same column are switched too.\n\n");
    cout << correctEncoding("Press any key.\n");
    _getch();
}

void display_goodbye()
{
    system("cls");    cout << correctEncoding("\nGoodbye!\n");
}


// =============== Working with the menu =======================

void init_main_menu(Menu* M)
{
    int items_number = 4;
    M->items_text = new string[items_number];
    M->actions = new int[items_number];
    M->items_number = 0;
    M->items_text[M->items_number] = correctEncoding("Continue game");
    M->actions[M->items_number++] = actionPlay;
    M->items_text[M->items_number] = correctEncoding("New game");
    M->actions[M->items_number++] = actionNewGame;
    M->items_text[M->items_number] = correctEncoding("Settings");
    M->actions[M->items_number++] = actionRequestParameters;
    M->items_text[M->items_number] = correctEncoding("Exit");
    M->actions[M->items_number++] = actionQuit;
    M->active = 0;
}

void delete_main_menu(Menu* M)
{
    delete[]  M->items_text;
    delete[]  M->actions;
}

void add_menu_to_text(Menu M, ExchangeVariable* EV, ColoredText* T)
{
    // adds the menu line to the string array T
    T->st[T->current_string] = "";
    for (int i = 0; i < M.items_number; i++)
    {
        if (i == M.active && EV->action == actionRunMenu)
        {
            T->st[T->current_string] += "<" + M.items_text[i] + ">";
        }
        else
        {
            T->st[T->current_string] += " " + M.items_text[i] + " ";
        }
    }
    T->current_string++;
}

void run_menu(Menu* M, ExchangeVariable* EV)
{
    // key press handler when the menu is active
    EV->text_changed = true; // if something is done, the screen needs to be updated
    switch (EV->key)
    {
    case 77:
        if (M->active < M->items_number - 1)
        {
            M->active++;
        }
        break;
    case 75:
        if (M->active > 0)
        {
            M->active--;
        }
        break;
    case 13:
        EV->action = M->actions[M->active];
        break;
        // the action stored in the EV exchange variable is selected according to the menu item
    case keyUpdateMenu:
        EV->text_changed = true;
        break;
    default:
        EV->text_changed = false;
        // if no action is taken, the screen does not need to be updated.
    }
    EV->key = keyDoNothing;
    // the key is processed and its value is reset to avoid reprocessing
}

// ============== Game algorithms ===================

long power2(int n)
// works only with non-negative integer powers
{
    long p = 1;
    for (p; n > 0; n--)
    {
        p *= 2;
    }
    return p;
}

long mask(int size)
{
    return power2(size) - 1;
    // size ones, the rest are zeros
}

void toggle_cross(int* map, int size, Position p)
{
    // toggles a cross in the map matrix
    long px = power2(p.x);
    for (int i = 0; i < size; i++)
    {
        if (i == p.y)
        {
            map[i] = map[i] ^ mask(size);
        }
        else
            map[i] = map[i] ^ px;
    }
}

int find_switched_on(int* map, int size)
{
    // counts the number of switched-on fields
    long pm = power2(size), s = 0;
    for (int i = 0; i < size; i++)
    {
        for (int px = 1; px < pm; px *= 2)
        {
            if (map[i] & px)
            {
                s++;
            }
        }
    }
    return s;
}

void find_solution(Board* B)
{
    // The function determines which elements need to be toggled to turn everything off.
    // If you're interested in playing, it's better not to delve into how it works
    for (int i = 0; i < B->board_size; i++)
    {
        B->sol[i] = 0;
    }
    Position p;
    for (p.y = 0; p.y < B->board_size; p.y++)
    {
        for (p.x = 0; p.x < B->board_size; p.x++)
        {
            if (B->map[p.y] & power2(p.x))
            {
                toggle_cross(B->sol, B->board_size, p);
            }
        }
    }
}

Position element_to_change(Board* B)
{
    Position P = B->active;
    find_solution(B);
    B->steps_to_solve = find_switched_on(B->sol, B->board_size);
    if (B->steps_to_solve)
    {
        // the first position for change found during top-down, left-to-right scanning
        int px = 1;
         for (int i = 0; i < B->board_size; i++)
        {
            px = 1;
            for (int j = 0; j < B->board_size; j++)
            {
                if (B->sol[i] & px)
                {
                    P.y = i;
                    P.x = j;
                    return P;
                }
                px *= 2;
            }
        }
    }
    return P;
}

void fill_random(int* map, int size)
{
    for (int i = 0; i < size; i++)
    {
        map[i] = (rand() * power2(15) + rand()) & mask(size);
    }
}

void init_new_game(Board* B, Parameters P)
{
    fill_random(B->map, B->board_size);
    B->switched_on = find_switched_on(B->map, B->board_size);
    find_solution(B);
    B->steps_to_solve = find_switched_on(B->sol, B->board_size);
    B->active = LeftTop;
    B->display_steps = P.display_steps;
    B->steps_done = 0;
}

void init_board(Board* B, Parameters P)
{
    B->board_size = P.board_size;
    B->map = new int[B->board_size]{};
    B->sol = new int[B->board_size]{};
    B->screen_width = 80;
    B->margin_left = (B->screen_width - (3 * B->board_size + 1)) / 2;
    B->margin_top = 1;
    B->margin_bottom = 1;
    init_new_game(B, P);
}

void delete_board(Board* B)
{
    delete[] B->map;
    delete[] B->sol;
}

void run_board(Board* B, ExchangeVariable* EV)
{
    // event handler for actions related to the game board (EV.action = actionGame)
    EV->text_changed = true; // if anything triggers, the screen needs to be refreshed
    switch (EV->key)
    {
    case 72:
        if (B->active.y > 0)
        {
            B->active.y--;
        }
        break;
    case 80:
        if (B->active.y < B->board_size - 1)
        {
            B->active.y++;
        }
        break;
    case 77:
        if (B->active.x < B->board_size - 1)
        {
            B->active.x++;
        }
        break;
    case 75:
        if (B->active.x > 0)
        {
            B->active.x--;
        }
        break;
    case 32:
        //toggle_cross_on_board(B);
        toggle_cross(B->map, B->board_size, B->active);
        B->steps_done++;
        break;
    case 's':
    case 'S':
        B->active = element_to_change(B);
        break;
    case 27:
        EV->action = actionRunMenu;
        // transfer control to the menu block
    default:
        EV->text_changed = false; // if nothing triggers, the screen does not need to be refreshed
    }
    if (EV->action == actionRunMenu)
    {
        EV->key = keyUpdateMenu;
    }
    else
    {
        EV->key = keyDoNothing; // the key has been processed, and to prevent reprocessing, its value is reset
    }
    B->switched_on = find_switched_on(B->map, B->board_size);
    find_solution(B);
    B->steps_to_solve = find_switched_on(B->sol, B->board_size);
    if (!B->steps_to_solve)
    {
        EV->key = keyWin;
    }
}

// ========================= main event handler ======================

void handle_hotkeys(ExchangeVariable* EV)
{
    if (EV->key == 'q' || EV->key == 'Q')
    {
        EV->action = actionQuit;
    }
}

void refresh_text(ColoredText* T, HANDLE h, Menu M, ExchangeVariable* EV, Board* B)
{
    T->current_string = 0;
    add_menu_to_text(M, EV, T);
    if (EV->key == keyWin)
    {
        add_congratulation(B, EV, T);
    }
    else
    {
        add_board_to_text(*B, T);
        add_info_to_text(B, T, EV);
    }
    system("cls");
    display_text(*T, h);
    EV->text_changed = false;
}

void handle_event(Board* B, ExchangeVariable* EV, HANDLE h, Parameters* P, Menu M, ColoredText* T)
{
    refresh_text(T, h, M, EV, B);
    do
    {
        if (_kbhit())
        {
            EV->key = _getch();
            // this variable stores not only key codes but also some action codes
            if (EV->key == -32)
            {
                // the additional code generated by arrow keys is discarded immediately
                EV->key = _getch();
            }
        }
        handle_hotkeys(EV); // handle hotkeys
        switch (EV->action)
        {
        case actionRunMenu:
            run_menu(&M, EV);
            //EV->key = 0;
            break;
        case actionPlay:
            run_board(B, EV);
            break;
        case actionNewGame:
            init_new_game(B, *P);
            EV->text_changed = true;
            EV->action = actionPlay;
            EV->key = keyDoNothing;
            break;
        case actionRequestParameters:
            request_parameters(P, EV);
            delete_board(B);
            init_board(B, *P);
            delete_text(T);
            init_text(T, *B, *P);
            EV->text_changed = true;
            EV->action = actionRunMenu;
            EV->key = keyUpdateMenu;
        }
        if (EV->text_changed)
        {
            // this check is necessary to rebuild the screen only if something changed
            // to prevent the screen from flickering when holding a key
            refresh_text(T, h, M, EV, B);
        }
    } while (!(EV->action == actionQuit));
}

// =================== main function ======================

int main()
{
    SetConsoleCP(866);
    SetConsoleOutputCP(866);
    srand(time(NULL));
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // for the colored version;
    Parameters P = {4, 16, 1, true}; // (board_size, number_of_info_strings, number_of_menu_strings, display_steps)
    Menu main_menu;
    init_main_menu(&main_menu);
    Board B;
    init_board(&B, P);
    ExchangeVariable EV = {actionRunMenu, keyDoNothing, true};
    ColoredText CT; // the entire screen as text representation
    init_text(&CT, B, P);
    //========================= start the main event handler
    display_introduction();
    handle_event(&B, &EV, hConsole, &P, main_menu, &CT);
    display_goodbye();
    //========================= delete dynamic variables
    delete_text(&CT);
    delete_board(&B);
    delete_main_menu(&main_menu);
}

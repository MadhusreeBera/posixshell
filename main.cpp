#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <iomanip>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <vector>
#include <stack>
#include <list>
#include <algorithm>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <sstream>
#include <sys/wait.h>
#include <fstream>

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

#define MOD 1000000007
#define MOD1 998244353
#define INF 1e18
#define nline "\n"
#define pb push_back
#define ppb pop_back
#define mp make_pair
#define ff first
#define ss second
#define PI 3.141592653589793238462
#define set_bits __builtin_popcountll
#define sz(x) ((int)(x).size())
#define all(x) (x).begin(), (x).end()

using namespace std;
class Config
{
public:
    struct termios orig_termios;
    int shell_id;
    string HOME = string(getenv("HOME"));
} config;

class cursor
{
public:
    int win_x = 0;
    int win_y = 0;
    int x = 0;
    int y = 0;
} cursor;
class Command
{
public:
    vector<string> instructions;
} cmd;
enum KEYS
{
    ENTER = 10,
    UP = 65,
    DOWN = 66,
    LEFT = 68,
    RIGHT = 67,
    BACK_SPACE = 127,
    HOME = 104,
    HOME1 = 72,
    COLON = 58,
    ESC = 27,
    q = 113,
    Q = 81
};
bool set_cursor_position(int row, int col)
{
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "\x1b[%d;%dH", row, col);
    write(STDOUT_FILENO, buf, len);
    return true;
}
void clearLineMacro()
{
    cout << "\033[2K" << flush;
}
void disable_shell()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.orig_termios) != 0)
    {
    };
}
void ctrl_c(int signum)
{
    disable_shell();
    pid_t p = getpid();
    if (p < 0)
    {
    }

    if (p != config.shell_id)
    {
        return;
    }

    /* if (CHILD_ID != -1)
    {
        kill(CHILD_ID, SIGINT);
    } */

    while (true)
    {

        signal(SIGINT, ctrl_c);
        break;
    }

    return;
}

void ctrl_z(int signum)
{
    disable_shell();
}
int getCursorPosition(int *rows, int *cols)
{
    string s;
    s.resize(32);
    unsigned int i = 0;
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return -1;
    while (i < s.length() - 1)
    {
        if (read(STDIN_FILENO, &s[i], 1) != 1)
            break;
        if (s[i] == 'R')
            break;
        i++;
    }
    s[i] = '\0';
    if (s[0] != '\x1b' || s[1] != '[')
        return -1;
    if (sscanf(&s[2], "%d;%d", rows, cols) != 2)
        return -1;
    return 0;
}
int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
            return -1;
        // editorReadKey();
        return getCursorPosition(rows, cols);
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void enable_shell()
{
    struct termios raw = config.orig_termios;
    atexit(disable_shell);
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
vector<string> split_string(string s, char ch)
{
    vector<string> tokens;

    stringstream ss(s);
    string token;

    while (getline(ss, token, ch))
    {
        tokens.push_back(token);
    }
    return tokens;
}
string getParent_dir(string s)
{
    vector<string> tokens = split_string(s, '/');
    string ret;
    for (int i = 0; i < tokens.size() - 1; i++)
    {
        if (i > 0)
            ret += "/" + tokens[i];
    }
    return ret;
}
string read_command()
{
    
    string s;
    s = "\x1b[1m\x1b[34mPOSIX-PARTY : \x1b[0m\x1b[1m\x1b[32m" + config.HOME + "$ \x1b[0m";
    cout << s << flush;
    cursor.x = s.length();
    int label = s.length();
    while (1)
    {
        char key[3] = {'\0'};
        read(STDIN_FILENO, &key, 3);
        if (iscntrl(key[0]))
        {
            if (int(key[0]) == BACK_SPACE)
            {
                if (!s.empty() && cursor.x > label)
                {
                    clearLineMacro();
                    set_cursor_position(cursor.y, 0);
                    s.pop_back();
                    cursor.x--;
                    cout << s << flush;
                }
                else
                {
                    clearLineMacro();
                    set_cursor_position(cursor.y, 0);
                    cout << s << flush;
                }
            }
            else if (int(key[0]) == 27)
            {

                if (int(key[1]) == 91)
                {

                    if (int(key[2]) == RIGHT)
                    {
                    }
                    else if (int(key[2]) == LEFT)
                    {
                    }
                }
            }
            else if (int(key[0]) == ENTER)
            {
                cursor.y++;
                break;
            }
        }
        else
        {
            s.push_back(key[0]);
            cursor.x++;
        }
    }


    return s.substr(label,cursor.x-label);
}
vector<Command> process_commands(string shell_i)
{
    vector<Command> cmds;
    vector<string> tokens = split_string(shell_i, '|');
    for (int i = 0; i < tokens.size(); i++)
    {
        Command cmd;
        cmd.instructions = split_string(tokens[i], ' ');
        cmds.push_back(cmd);
    }
    return cmds;
}
void handler(int sig)
{
    int pstatus;
    pid_t pid = waitpid(-1, &pstatus, WNOHANG);

    if (pid > 0)
    {
        char j_name[256];
        int index = 0;

        int i = 0;
        /* while (i < jobs.size())
        {

            if (jobs[i].PID == pid)
            {

                while (true)
                {
                    strcpy(j_name, jobs[i].job_name);

                    while (true)
                    {
                        jobs.erase(jobs.begin() + i);
                        break;
                    }

                    index = 1;
                    break;
                }
                break;
            }

            i = i + 1;
        } */

        if (WEXITSTATUS(pstatus) == 0 && WIFEXITED(pstatus) && index)
        {
        }

        else if (index)
        {
        }

        fflush(stdout);
        fflush(stdout);
        fflush(stdout);
    }

    return;
}
void clear_screen()
{

    write(STDOUT_FILENO, "\x1b[H", 3);
    write(STDOUT_FILENO, "\x1b[2J", 4);
}
void init()
{
    enable_shell();
    if (getWindowSize(&cursor.win_y, &cursor.win_x) == -1)
    {
        cout << "No Window size available" << endl;
        return;
    }
    clear_screen();
}
int main(int argc, char const *argv[])
{
    init();

    signal(SIGINT, ctrl_c);
    /* signal(SIGCHLD, handler); */
    while (1)
    {
        string command = read_command();
        if (command == "exit")
        {
            exit(EXIT_SUCCESS);
        }

        vector<Command> cmds = process_commands(command);
    }
    return 0;
}

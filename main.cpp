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
using namespace std;
class Config
{
public:
    struct termios orig_termios;
} config;
enum KEYS
{
    ENTER = 0x0A,
    BACK_SPACE = 0x7f,
    TAB = 0x09
};
void disable_shell()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.orig_termios);
}
void enable_shell()
{
    struct termios raw = config.orig_termios;
    atexit(disable_shell);
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

string read_command()
{

    char c[100000];
    int rowlen = snprintf(c, sizeof(c), "\x1b[1m\x1b[34mPOSIX-PARTY : \x1b[0m\x1b[1m\x1b[32m%s $ \x1b[0m", getenv("HOME"));
    int i = rowlen;
    cout << c;
    while ((c[i] = getchar()))
    {

        if (c[i] == BACK_SPACE)
        {
            if (i > rowlen)
            {
                cout << "\b";
                cout << " ";
                cout << "\b";
                i--;
            }
        }
        else if (c[i] == ENTER)
        {
        }
        else if (c[i] == TAB)
        {
        }
        else
        {
            cout << c[i];
            i++;
        }
    }
    cout << endl;
    c[i] = '\0';
    string input(c);

    return input;
}

int main(int argc, char const *argv[])
{

    enable_shell();
    while (1)
    {

        cout << read_command();
    }
    return 0;
}

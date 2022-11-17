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
class Command{
    public :
    vector<string> instructions;
} cmd;
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
    atexit (disable_shell);

    struct termios raw = config.orig_termios;
    atexit(disable_shell);
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
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
            break;
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
    cout<<endl;
    c[i] = '\0';
    string input(c);

    return input.substr(rowlen,i-rowlen);
}
vector<Command> process_commands(string shell_i)
{   vector<Command> cmds;
    vector<string> tokens=split_string(shell_i,'|');
    for(int i=0;i<tokens.size();i++){
        Command cmd;
        cmd.instructions=split_string(tokens[i],' ');
        cmds.push_back(cmd);
    }
    return cmds;
}

int start_command (vector <Command> commands){
    pid_t pid, wpid;
    int status;

    char* args [commands [0].instructions.size ()+1];

    for (int i=0; i<commands [0].instructions.size (); i++){
        args [i] = new char [commands [0].instructions [i].length ()];
        strcpy (args [i], commands[0].instructions [i].c_str ()); 
    }
    pid = fork ();
    args [commands [0].instructions.size ()] = NULL;
    if (pid == 0){
        if (execvp (args [0], args) == -1){
            cerr<<"command failed"<<endl;
        }
    } else if (pid < 0){
        cerr<<"forking error"<<endl;
    } else if (pid > 0){
        do {
            wpid = waitpid (pid, &status, WUNTRACED);
        } while (!WIFEXITED (status) && !WIFSIGNALED (status));
    }

    return 1;
}

int main(int argc, char const *argv[])
{

    enable_shell();
    while (1)
    {   string command=read_command();
        if(command=="exit"){
            break;
        }
        vector<Command> cmds=process_commands(command);
        start_command (cmds);
    }
    return 0;
}

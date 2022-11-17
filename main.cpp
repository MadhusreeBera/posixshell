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
#include <sys/stat.h>
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

char path[256];

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
    int rowlen = snprintf(c, sizeof(c), "\x1b[1m\x1b[34mPOSIX-PARTY : \x1b[0m\x1b[1m\x1b[32m%s $ \x1b[0m", path);
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

bool pathexists(string pathname)
{
  struct stat buff;
  return (stat(pathname.c_str(), &buff) == 0);
}

string handle_path(string s)
{
    vector<string> tokens = split_string(s, '/');
    string dir;
    vector<string> format;
    if (tokens[0] == "~")
    {
        dir = getenv("HOME");
        for (int i = 1; i < tokens.size(); i++)
        {
            dir += "/" + tokens[i];
        }
    }
    else if (tokens[0] == ".")
    {
        dir = getcwd(path, 256);
        for (int i = 1; i < tokens.size(); i++)
        {
            dir += "/" + tokens[i];
        }
    }
    else if (tokens[0] == "..")
    {
        dir = getParent_dir(getcwd(path, 256));
        for (int i = 1; i < tokens.size(); i++)
        {
            dir += "/" + tokens[i];
        }
    }
    else
    {
        if (s[0] == '/')
            dir = s;
        else
            dir = string(getcwd(path, 256)) + "/" + s;
    }
    if (!dir.empty())
    {
        vector<string> new_tokens = split_string(dir, '/');
        for (int i = 1; i < new_tokens.size(); i++)
        {
            format.push_back("/" + new_tokens[i]);
        }
    }
    vector<string> v;
    for (int i = 0; i < format.size(); i++)
    {

        if (format[i] == "/..")
        {
            v.pop_back();
        }
        else if (format[i] == "/.")
        {
            continue;
        }
        else
        {
            v.push_back(format[i]);
        }
    }
    string x;
    for (int i = 0; i < v.size(); i++)
    {
        x += v[i];
    }
    return x;
}

void command_handle(vector<Command> cmds){
    int ind = cmds.size()-1;
    if(strcmp(cmds[ind].instructions[0].c_str(), "cd") == 0){
        if(cmds[ind].instructions.size() == 1){
            return;
        }
        if(cmds[ind].instructions.size() > 2){
            cout<<"Too many arguments"<<endl;
            return;
        }
        if(!pathexists(cmds[ind].instructions[1])){
            cout<<"No such file or directory"<<endl;
            return;
        }
        chdir((handle_path(cmds[ind].instructions[1])).c_str());
        getcwd(path, 256);
    }

    else if(strcmp(cmds[ind].instructions[0].c_str(), "pwd") == 0){
        cout<<path<<endl;
    }
}

int main(int argc, char const *argv[])
{

    enable_shell();
    getcwd(path, 256);
    while (1)
    {   string command=read_command();
        if(command=="quit")
            break;
        vector<Command> cmds=process_commands(command);
        command_handle(cmds);
    }
    return 0;
}
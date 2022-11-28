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
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <map>

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

class Node
{
public:
    char ch;
    Node *parent;
    Node *next[257];
    Node()
    {
        parent = NULL;
        for (int i = 0; i < 27; i++)
        {
            next[i] = NULL;
        }
    }
    Node(char ch)
    {
        this->ch = ch;
        for (int i = 0; i < 27; i++)
        {
            next[i] = NULL;
        }
    }
};
list<int> bg;
class Trie
{
public:
    Node *root;
    Node *end;
    ofstream trie_logger;
    Trie()
    {
        root = new Node('/');
        end = new Node('$');
    }
    void initialize_trie()
    {
        trie_logger.open("trie_logger.txt");
        trie_logger << "Initializing trie" << endl;

        DIR *dir = opendir("/bin/");
        trie_logger << "accessing files" << endl;
        if (dir == NULL)
        {
            trie_logger << "not open" << endl;
            exit(1);
        }
        struct dirent *entity;
        entity = readdir(dir);
        trie_logger << "printing files" << endl;

        while (entity != NULL)
        {

            string command_name = string(entity->d_name);
            // command_name = entity->d_name;
            if (command_name == "." || command_name == "..")
            {
                entity = readdir(dir);

                continue;
            }

            insert(command_name);
            // trie_logger << command_name << endl;

            entity = readdir(dir);
        }
        closedir(dir);
    }
    void insert(string st)
    {
        Node *cur = root;
        int n = st.length();
        for (int i = 0; i < n; i++)
        {
            int index = int(st[i]);
            if (cur->next[index] == NULL)
            {
                Node *temp = new Node(st[i]);
                temp->parent = cur;
                cur->next[index] = temp;
                cur = temp;
            }
            else
            {
                cur = cur->next[index];
            }
        }
        cur->next[256] = end;
    }
    int find(string st)
    {
        Node *cur = root;
        for (int i = 0; i < st.length(); i++)
        {
            int index = int(st[i]);

            if (cur->next[index] == NULL)
                return 0;
            cur = cur->next[index];
        }
        if (cur->next[256] == NULL) // string did not end
            return 0;
        return 1;
    }
    void getWords(Node *cur, string prefix, vector<string> &wordList)
    {
        if (cur == end)
        {
            wordList.push_back(prefix);
            return;
        }
        for (int i = 0; i < 257; i++)
        {
            if (cur->next[i] != NULL)
            {
                getWords(cur->next[i], prefix + cur->ch, wordList);
            }
        }
    }
    vector<string> auto_complete(string prefix)
    {
        Node *cur = root;
        vector<string> wordList;
        for (int i = 0; i < prefix.length(); i++)
        {
            int index = int(prefix[i]);
            if (cur->next[index] == NULL)
            {
                // cout << -1 << endl;
                return wordList;
            }
            else
            {
                cur = cur->next[index];
            }
        }
        prefix.pop_back();
        getWords(cur, prefix, wordList);
        // cout << wordList.size() << endl;
        return wordList;
    }
    void insert_suffix(string st)
    {
        for (int i = 0; i < st.length(); i++)
        {
            insert(st.substr(i));
        }
    }
} * trie;
class Config
{
public:
    struct termios orig_termios;
    int shell_id;
    char path[256];
    string s_path;
    string username, hostname, symbol;
    string HOME = string(getenv("HOME"));
    string myrc_fullpath;
    int ex_status = 0;
    map<string, string> alias;
    string myrc_info = "";
    char myrc_path[256];
    int shell_term;
    pid_t shell_pg;
    bool isSuspended;
} config;

class cursor
{
public:
    int win_x = 0;
    int win_y = 0;
    int x = 1;
    int y = 1;
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
    TAB = 9,
    COLON = 58,
    ESC = 27,
    q = 113,
    Q = 81
};
class History
{
public:
    ofstream history_logger;
    int HISTSIZE, no_of_commands;
    History()
    {
        history_logger.open("history.txt", ios_base::app);
        // read HISTSIZE from myrc

        HISTSIZE = 100;
        no_of_commands = 0;

        ifstream history_if;
        history_if.open("history.txt");
        string line;
        while (std::getline(history_if, line))
            ++no_of_commands;
    }
    void insert(string command)
    {
        if (command == "")
            return;
        if (no_of_commands < HISTSIZE)
        {
            string line = to_string(++no_of_commands) + "\t" + command;
            history_logger << line << endl;
        }
    }
    void print_history()
    {
        ifstream history_input_stream;
        history_input_stream.open("history.txt");
        if (history_input_stream.fail())
        {
            perror("Failed to load history: ");
            return;
        }
        string line, line_number;
        for (std::string line; getline(history_input_stream, line);)
        {
            cout << line << endl;
        }
        history_input_stream.close();
    }

    void search_history_file(string key)
    {
        ifstream history_input_stream;
        history_input_stream.open("history.txt");
        if (history_input_stream.fail())
        {
            perror("Failed to open history: ");
            return;
        }
        string line, line_number;
        for (std::string line; getline(history_input_stream, line);)
        {
            // cout << line << endl;
            Trie *line_trie = new Trie();
            line_trie->insert_suffix(line);
            vector<string> searchResult = line_trie->auto_complete(key);
            if (searchResult.size() > 0)
            {
                cout << line << endl;
            }
        }
        history_input_stream.close();
    }
} * history;
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
        exit(EXIT_FAILURE);
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
void create_myrc()
{
    fstream file;
    file.open("myrc.txt", ios::out);
    getcwd(config.myrc_path, 256);
    config.myrc_fullpath = string(config.myrc_path) + "/myrc.txt";
    file << "PATH=" + string(getenv("PATH")) << "\n";
    config.myrc_info += "PATH=" + string(getenv("PATH")) + "\n";
    file << "HOME=" + string(getenv("HOME")) << "\n";
    config.myrc_info += "HOME=" + string(getenv("HOME")) + "\n";
    config.username = string(getenv("USER"));
    file << "USER=" + config.username << "\n";
    config.myrc_info += "USER=" + config.username + "\n";
    char host[1024];
    memset(host, 0, 1024);
    gethostname(host, 1024);
    config.hostname = string(host);
    file << "HOSTNAME=" + config.hostname << "\n";
    config.myrc_info += "HOSTNAME=" + config.hostname + "\n";
    config.symbol = "$";
    file << "PS1=$"
         << "\n";
    config.myrc_info += "PS1=$\n";
}
void enable_shell()
{
    tcgetattr(STDIN_FILENO, &config.orig_termios);
    struct termios raw = config.orig_termios;
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
string read_command(bool *exec)
{
    set_cursor_position(cursor.win_y - 1, 1);
    string s;
    s = BOLDRED + config.username + "@" + config.hostname + ": \x1b[0m" + BOLDYELLOW + config.s_path + " " + config.symbol + " \x1b[0m";
    cout << '\n'
         << s << flush;
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
                    set_cursor_position(cursor.win_y - 1, 1);
                    s.pop_back();
                    cursor.x--;
                    cout << '\n'
                         << s << flush;
                }
                else
                {
                    clearLineMacro();
                    set_cursor_position(cursor.win_y - 1, 1);
                    cout << '\n'
                         << s << flush;
                }
            }
            else if (int(key[0]) == 27)
            {

                if (int(key[1]) == 91)
                {

                    if (int(key[2]) == RIGHT)
                    {
                        clearLineMacro();
                        set_cursor_position(cursor.win_y - 1, 1);
                        cout << '\n'
                             << s << flush;
                    }
                    else if (int(key[2]) == LEFT)
                    {
                        clearLineMacro();
                        set_cursor_position(cursor.win_y - 1, 1);
                        cout << '\n'
                             << s << flush;
                    }
                    else if (int(key[2]) == UP)
                    {
                        clearLineMacro();
                        set_cursor_position(cursor.win_y - 1, 1);
                        cout << '\n'
                             << s << flush;
                    }
                    else if (int(key[2]) == DOWN)
                    {
                        clearLineMacro();
                        set_cursor_position(cursor.win_y - 1, 1);
                        cout << '\n'
                             << s << flush;
                    }
                }
            }
            else if (int(key[0]) == ENTER)
            {
                cursor.y++;
                break;
            }
            else if (int(key[0]) == TAB)
            {
                string prefix = "";
                int k;
                for (k = label; k < cursor.x; k++)
                {
                    prefix += s[k];
                }
                int prefix_len = prefix.length();
                vector<string> commandList = trie->auto_complete(prefix);
                clearLineMacro();
                set_cursor_position(cursor.win_y - 1, 1);
                cout << '\n'
                     << s << flush;
                if (commandList.size() == 1)
                {

                    for (int j = prefix_len; j < commandList[0].length(); j++)
                    {
                        s += commandList[0][j];
                        cursor.x++;
                    }
                    clearLineMacro();
                    set_cursor_position(cursor.win_y - 1, 1);
                    cout << '\n'
                         << s << flush;
                }
                else if (commandList.size() > 1)
                {
                    cout << endl;
                    int count = 0;
                    cursor.y++;
                    for (auto ele : commandList)
                    {

                        cout << left << setw(30) << ele;
                        count++;
                        if (count % 4 == 0)
                        {
                            cursor.y++;
                            cout << endl;
                        }
                    }
                    cursor.y++;
                    cout << endl;
                    *exec = false;
                    break;
                }
            }
        }
        else
        {
            s.push_back(key[0]);
            cursor.x++;
        }
    }

    return s.substr(label, cursor.x - label);
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
void print_alarm(string message, int seconds)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        sleep(seconds);
        cout << message << endl;

        exit(EXIT_SUCCESS);
    }
}

void clear_screen()
{

    write(STDOUT_FILENO, "\x1b[H", 3);
    write(STDOUT_FILENO, "\x1b[2J", 4);
}
void export_print()
{
    fstream file;
    file.open(config.myrc_fullpath.c_str(), ios::in);
    string x;
    while (file >> x)
    {
        if (x == "" || x == "\n")
        {
            continue;
        }
        vector<string> var = split_string(x, '=');
        cout << "declare -x " << var[0] << "="
             << "\"" << var[1] << "\"" << endl;
    }
    file.close();
}
void export_delete(string del_comm)
{
    string line;
    ifstream fin;
    fin.open(config.myrc_fullpath.c_str());
    ofstream temp;
    string tempfile = string(config.myrc_path) + "/temp.txt";
    temp.open(tempfile.c_str());
    while (getline(fin, line))
    {
        if (line == "" || line == "\n")
        {
            continue;
        }
        vector<string> var = split_string(line, '=');
        if (strcmp(var[0].c_str(), del_comm.c_str()) == 0)
        {
            continue;
        }
        temp << line << endl;
    }
    temp.close();
    fin.close();
    remove(config.myrc_fullpath.c_str());
    rename(tempfile.c_str(), config.myrc_fullpath.c_str());
}
bool pathexists(string pathname)
{
    struct stat buff;
    return (stat(pathname.c_str(), &buff) == 0);
}
string display_path(string s)
{
    vector<string> tokens = split_string(s, '/');
    if (tokens.size() <= 2)
    {
        return s;
    }
    string res = "~";
    for (int i = 3; i < tokens.size(); i++)
    {
        res += "/" + tokens[i];
    }
    return res;
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
        dir = getcwd(config.path, 256);
        for (int i = 1; i < tokens.size(); i++)
        {
            dir += "/" + tokens[i];
        }
    }
    else if (tokens[0] == "..")
    {
        dir = getParent_dir(getcwd(config.path, 256));
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
            dir = string(getcwd(config.path, 256)) + "/" + s;
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
            if (v.empty())
            {
                continue;
            }
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
    if (x == "")
    {
        x = "/";
    }
    return x;
}
void path_commands(vector<Command> cmds, int ind)
{
    if (strcmp(cmds[ind].instructions[0].c_str(), "cd") == 0)
    {
        if (cmds[ind].instructions.size() == 1)
        {
            config.ex_status = 0;
            return;
        }
        if (cmds[ind].instructions.size() > 2)
        {
            cout << "Too many arguments" << endl;
            config.ex_status = 1;
            return;
        }
        string s = (handle_path(cmds[ind].instructions[1]));
        if (!pathexists(s))
        {
            cout << "No such file or directory" << endl;
            config.ex_status = 1;
            return;
        }
        chdir(s.c_str());
        getcwd(config.path, 256);
        config.ex_status = 0;
        config.s_path = display_path(string(config.path));
    }

    else if (strcmp(cmds[ind].instructions[0].c_str(), "pwd") == 0)
    {
        cout << config.path << endl;
        config.ex_status = 0;
    }
    else
    {
        if (cmds[ind].instructions.size() == 1 || (cmds[ind].instructions.size() == 2 && strcmp(cmds[ind].instructions[1].c_str(), "-p")))
        {
            export_print();
            config.ex_status = 0;
        }
        // else if (cmds[ind].instructions.size() == 3)
        // {
        //     if (strcmp(cmds[ind].instructions[1].c_str(), "-n") == 0)
        //     {
        //         export_delete(cmds[ind].instructions[2]);
        //     }
        // }
    }
}

void printmyrc()
{
    fstream file;
    file.open(config.myrc_fullpath.c_str(), ios::in);
    string x;
    while (file >> x)
    {
        if (x == "" || x == "\n")
        {
            continue;
        }
        cout << x << endl;
    }
    file.close();
}

int printenv_value(string envar)
{
    int stat = 0;
    if (envar == "USER")
    {
        cout << config.username << endl;
    }
    else if (envar == "HOSTNAME")
    {
        cout << config.hostname << endl;
    }
    else if (envar == "PS1")
    {
        cout << config.symbol << endl;
    }
    else
    {
        fstream file;
        file.open(config.myrc_fullpath.c_str(), ios::in);
        string x;
        if (envar == "PATH")
        {
            file >> x;
            cout << x << endl;
        }
        else if (envar == "HOME")
        {
            file >> x;
            file >> x;
            cout << x << endl;
        }
        else
        {
            stat = 1;
        }
        file.close();
    }
    return stat;
}

void handle_echo(vector<Command> commands, int ind)
{
    for (int i = 1; i < commands[ind].instructions.size(); i++)
    {
        for (int j = 0; j < commands[ind].instructions[i].length(); j++)
        {
            if (commands[ind].instructions[i][j] == '$')
            {
                if (commands[ind].instructions[i][j + 1])
                {
                    if (commands[ind].instructions[i][j + 1] == '$')
                    {
                        cout << getpid();
                        j++;
                        continue;
                    }
                    else if (commands[ind].instructions[i][j + 1] == '?')
                    {
                        cout << config.ex_status;
                        j++;
                        continue;
                    }
                }
            }
            cout << commands[ind].instructions[i][j];
        }
        cout << " ";
    }
    cout << endl;
    config.ex_status = 0;
}

void alias_modify()
{
    string line;
    ofstream fout;
    fout.open(config.myrc_fullpath.c_str());
    fout << config.myrc_info;
    for (auto i : config.alias)
    {
        fout << "alias " + i.first + "=" + "\'" + i.second + "\'\n";
    }
}

void handle_alias(vector<Command> commands, int ind)
{
    int len = commands[ind].instructions.size();
    if (len == 1)
    {
        for (auto i : config.alias)
        {
            cout << "alias " + i.first + "=" + "\'" + i.second + "\'" << endl;
        }
        config.ex_status = 0;
        return;
    }
    else
    {
        int j;
        string al_name = "", al_comm = "";
        for (j = 0; j < commands[ind].instructions[1].length(); j++)
        {
            if (commands[ind].instructions[1][j] != '=')
            {
                al_name += commands[ind].instructions[1][j];
            }
            else
            {
                break;
            }
        }
        j += 2;
        while (commands[ind].instructions[1][j])
        {
            al_comm += commands[ind].instructions[1][j];
            j++;
        }
        if (al_comm != "")
        {
            al_comm += " ";
        }
        for (int i = 2; i < len - 1; i++)
        {
            al_comm += commands[ind].instructions[i] + " ";
        }

        j = 0;
        if (commands[ind].instructions[len - 1].length() != 1)
        {
            while (j < commands[ind].instructions[len - 1].length() - 1)
            {
                al_comm += commands[ind].instructions[len - 1][j];
                j++;
            }
        }

        config.alias[al_name] = al_comm;
        alias_modify();
        config.ex_status = 0;
    }
}
void init()
{
    enable_shell();
    atexit(disable_shell);
    if (getWindowSize(&cursor.win_y, &cursor.win_x) == -1)
    {
        cout << "No Window size available" << endl;
        return;
    }
    trie = new Trie();
    trie->initialize_trie();
    getcwd(config.path, 256);
    config.s_path = display_path(string(config.path));
    create_myrc();
    clear_screen();
    history = new History();
    config.shell_pg = getpid();

    config.isSuspended = false;
    setpgid(config.shell_pg, config.shell_pg);
}

void bghandler(int sig)
{
    int bgstatus = 0;

    int corpse = waitpid((pid_t)-1, &bgstatus, WNOHANG);

    if (corpse > 0)
    {
        cout << corpse << " done with status " << bgstatus << endl;
        bg.remove(corpse);
        tcsetpgrp(config.shell_term, config.shell_pg);
        config.isSuspended = false;
    }
}

int check_builinCommands(vector<Command> commands, vector<Command> &alias)
{
    int ind = 0;
    auto search = config.alias.find(commands[ind].instructions[0]);
    if (search != config.alias.end())
    {
        if (search->second == "exit")
        {
            exit(EXIT_SUCCESS);
        }
        alias = process_commands(search->second);
        return 1;
    }

    if (strcmp(commands[ind].instructions[0].c_str(), "cd") == 0 || strcmp(commands[ind].instructions[0].c_str(), "pwd") == 0 || strcmp(commands[ind].instructions[0].c_str(), "export") == 0)
    {
        path_commands(commands, ind);
        return 0;
    }

    if (strcmp(commands[ind].instructions[0].c_str(), "env") == 0 && commands[ind].instructions.size() == 1)
    {
        printmyrc();
        config.ex_status = 0;
        return 0;
    }

    if (strcmp(commands[ind].instructions[0].c_str(), "printenv") == 0 && commands[ind].instructions.size() == 2)
    {
        if (printenv_value(commands[ind].instructions[1]))
        {
            config.ex_status = 1;
        }
        else
        {
            config.ex_status = 0;
        }
        return 0;
    }

    if (strcmp(commands[ind].instructions[0].c_str(), "echo") == 0)
    {
        handle_echo(commands, ind);
        return 0;
    }

    if (strcmp(commands[ind].instructions[0].c_str(), "alias") == 0)
    {
        handle_alias(commands, ind);
        return 0;
    }
    return -1;
}

void setFg(int pid)
{
    if (pid > 0)
    {
        int status;
        cout << " + " << pid << "\t" << endl;
        tcsetpgrp(config.shell_term, pid);
        config.isSuspended = true;
    }
}

int start_command(vector<Command> commands)
{

    for (int i = 0; i < commands.size() - 1; i++)
    {
        pid_t pid, wpid;
        int status;

        int pd[2];

        if (pipe(pd) < 0)
        {
            cout << "pipe failed" << endl;
        }

        char *args[commands[i].instructions.size() + 1];

        for (int j = 0; j < commands[i].instructions.size(); j++)
        {
            args[j] = new char[commands[i].instructions[j].length()];
            strcpy(args[j], commands[i].instructions[j].c_str());
        }
        args[commands[i].instructions.size()] = NULL;

        pid = fork();
        bool redirectFlag = 0;

        switch (pid)
        {
        case -1:
            cout << "fork failed" << endl;
            break;

        case 0:
            dup2(pd[1], 1);
            close(pd[0]);
            close(pd[1]);

            if (args[1] != NULL)
            {
                int index;
                for (int j = 0; j < commands[i].instructions.size(); j++)
                {
                    if (strcmp(args[j], ">") == 0 || strcmp(args[j], ">>") == 0)
                    {
                        index = j;
                        redirectFlag = 1;
                        break;
                    }
                }

                if (redirectFlag)
                {
                    int fd;
                    if (strcmp(args[index], ">") == 0)
                    {
                        fd = open(args[index + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
                    }
                    else
                    {
                        fd = open(args[index + 1], O_CREAT | O_WRONLY | O_APPEND, 0644);
                    }
                    dup2(fd, 1);
                    args[index] = 0;
                    if (execvp(args[0], args) == -1)
                    {
                        cout << "command failed" << flush;
                        _exit(EXIT_FAILURE);
                    }
                    close(fd);
                }
            }

            if (!redirectFlag)
            {
                if (execvp(args[0], args) == -1)
                {
                    cout << "command failed" << endl;
                    _exit(EXIT_FAILURE);
                }
            }

            for (int i = 0; i < commands[i].instructions.size() + 1; i++)
                delete (args[i]);
            break;

        default:
            do
            {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            break;
        }

        dup2(pd[0], 0);
        close(pd[1]);
        close(pd[0]);
    }

    int n = commands.size() - 1;

    char *args[commands[n].instructions.size() + 1];

    for (int i = 0; i < commands[n].instructions.size(); i++)
    {
        args[i] = new char[commands[n].instructions[i].length()];
        strcpy(args[i], commands[n].instructions[i].c_str());
    }
    args[commands[n].instructions.size()] = NULL;

    bool redirectFlag = 0;

    if (args[1] != NULL)
    {
        int index;
        for (int i = 0; i < commands[n].instructions.size(); i++)
        {
            if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0)
            {
                index = i;
                redirectFlag = 1;
                break;
            }
        }

        if (redirectFlag)
        {
            int fd;
            if (strcmp(args[index], ">") == 0)
            {
                fd = open(args[index + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            }
            else
            {
                fd = open(args[index + 1], O_CREAT | O_WRONLY | O_APPEND, 0644);
            }
            dup2(fd, 1);
            args[index] = 0;
            if (execvp(args[0], args) == -1)
            {
                cout << "command failed" << flush;
                _exit(EXIT_FAILURE);
            }
            close(fd);
        }
    }

    if (!redirectFlag)
    {
        if (execvp(args[0], args) == -1)
        {
            cout << "command failed" << endl;
            _exit(EXIT_FAILURE);
        }
        for (int i = 0; i < commands[n].instructions.size() + 1; i++)
            delete (args[i]);
    }

    return 1;
}
int main(int argc, char const *argv[])
{
    init();

    signal(SIGINT, ctrl_c);
    signal(SIGTTOU, SIG_IGN);
    /* signal(SIGCHLD, handler); */
    while (1)
    {
        bool exec = true;

        if (config.isSuspended)
            continue;

        string command = read_command(&exec);
        bool isBg = 0;
        for (int i = command.length() - 1; i >= 0; i--)
        {
            if (command[i] == '&')
            {
                isBg = 1;
                command.pop_back();
                break;
            }

            if (command[i] != ' ')
                break;
        }

        if (command[command.length() - 1] == ' ')
        {
            int i = command.length() - 1;
            while (command[i] == ' ')
            {
                command.pop_back();
            }
        }
        history->insert(command);
        if (command == "exit")
        {
            exit(EXIT_SUCCESS);
            return 0;
        }
        if (command == "history")
        {
            history->print_history();
            continue;
        }
        if (command.substr(0, 8) == "history " && command.length() > 8)
        {
            string keyword = command.substr(8, command.length() - 8);
            history->search_history_file(keyword);
            continue;
        }

        if (command == "fg")
        {
            setFg(bg.front());
            continue;
        }

        vector<Command> cmds = process_commands(command);
        vector<Command> builin_commands;
        int val = check_builinCommands(cmds, builin_commands);
        if (val == 1)
        {
            cmds = builin_commands;
        }
        else if (val == 0)
        {
            continue;
        }

        if (!cmds.empty() && exec)
        {
            if (isBg)
                signal(SIGCHLD, bghandler);

            signal(SIGTTIN, SIG_DFL);
            pid_t pid = fork();
            pid_t wpid;
            int status = 0;

            switch (pid)
            {
            case -1:
                cout << "fork error" << endl;
                break;

            case 0:
                if (isBg)
                {
                    setpgrp();
                    isBg = 0;
                }
                start_command(cmds);
                break;

            default:
                if (!isBg)
                {
                    do
                    {
                        wpid = waitpid(pid, &status, WUNTRACED);
                    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
                }

                if (isBg)
                {
                    bg.push_front(pid);
                    cout << bg.size() << " " << pid << endl;
                    isBg = 0;
                }

                setpgid(pid, pid);
                tcsetpgrp(config.shell_term, config.shell_pg);
            }
        }
    }
    return 0;
}
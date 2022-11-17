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
using namespace std;

/***************************** TRIE *************************************/
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
};
class Config
{
public:
    struct termios orig_termios;
} config;
class Command
{
public:
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
string read_command(Trie *trie)
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
            string prefix = "";
            int k;
            for (k = rowlen; k < i; k++)
            {
                prefix += c[k];
            }
            int prefix_len = prefix.length();
            vector<string> commandList = trie->auto_complete(prefix);
            if (commandList.size() == 1)
            {
                for (int j = prefix_len; j < commandList[0].length(); j++)
                {
                    c[i] = commandList[0][j];
                    cout << c[i++];
                }
            }
            else if (commandList.size() > 1)
            {
                cout << endl;
                int count = 0;
                for (auto ele : commandList)
                {

                    cout << left << setw(30) << ele;
                    count++;
                    if (count % 4 == 0)
                    {
                        cout << endl;
                    }
                }
            }
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

    return input.substr(rowlen, i - rowlen);
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

int main(int argc, char const *argv[])
{

    // creating Trie object
    Trie *trie = new Trie();
    trie->initialize_trie();

    enable_shell();
    while (1)
    {
        string command = read_command(trie);
        if (command == "quit")
        {
            disable_shell();
            break;
        }
        vector<Command> cmds = process_commands(command);
    }
    return 0;
}

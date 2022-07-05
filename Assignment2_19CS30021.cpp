/*
    OS Lab Assignment - 2

    Group 29
    Haasita Pinnepu 19CS30021
    Majji Deepika 19CS30027

    : Use of syscall :
*/

#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#include <fstream>
#include <dirent.h>
using namespace std;

#define MAX_CHAR 100
#define MAX_STR_LEN 1000 // maximum length for any string

vector<string>history;
char getch(void);
void saveCommand(string);
void searchDir(string);
void searchingHistory();

string filename("hist.txt");

void sigint_handler(int signo) {
    printf("Caught SIGINT\n");
}

void execute(string sentence, int in_fd, int out_fd, int pre_out, int next_in) {
    istringstream iss(sentence);

    pid_t child_pid;
    int wait_flag = 1;
    int l =0, r=0, bg=0,end=0, h=0;
    string l_file, r_file;
    vector<string> tokens;
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(tokens));
    for(unsigned int i=0;i<tokens.size();i++)
        {
            if(tokens[i]=="<")
            {
                l =1;
                l_file = tokens[i+1];
            }
            if(tokens[i]==">")
            {
                r=1;
                r_file = tokens[i+1];
            }
            if(tokens[i]=="&")
            {
                bg=1;
            }
            if(!l&&!r&&!bg)
                end++;
            if(tokens[i] == "history")
            {
                h=1;
            }
        }
        const char **argv = new const char *[end+1];
        for (int i = 0; i < end; i++)
        {
            argv[i] = tokens[i].c_str();
        }
        argv[end] = NULL;
        wait_flag = !bg;
        if ((child_pid = fork()) == 0)
        {   
            signal(SIGINT, SIG_DFL);

            if(r)
                out_fd = open(r_file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
           
            if(out_fd!=1) {
                dup2(out_fd, 1);
            }
            if(l) 
                in_fd = open(l_file.c_str(), O_RDONLY);
            
            if(in_fd!=0) 
            {
                dup2(in_fd, 0);
            }
            if(h)
            {
                //displayHistory();
                ifstream fp;
                fp.open(filename);

                string line;

                unsigned int curLine = 0;
                while(getline(fp, line)) 
                {
                    cout<<line<<endl;
                }
            }
            

            execvp(tokens[0].c_str(), (char **)argv);
            exit(0);
        }
        
        else
        {
            // wait untill child process is not over
            if(wait_flag)
            {
                wait(NULL);
                if(in_fd!=0)
                    close(in_fd);
                if(out_fd!=1)
                    close(out_fd);

            }
            else
            {
                cout<< "[ Process ] Background\n";
            }
        }
    return;
}

void sighandler(int sig_num)
{
    signal(SIGTSTP, sighandler);
    cout << "\nThe interrupt signal is (" << sig_num<< "). \n";
    pid_t pid = getpid();
    kill(pid, SIGCONT);
}

int main()
{
    string sentence;

    cout << "\033[1;32mWelcome to Grp 29 Shell (Enter 'exit' to stop):\033[0m\n";

    while (true)
    {   
        signal(SIGINT, SIG_IGN); // ctrl c
        signal(SIGTSTP, sighandler); //ctrl z

        char curr_dir[MAX_STR_LEN] = "";
        char *str_pt;

        str_pt = getcwd(curr_dir, sizeof(curr_dir));
        if (str_pt == NULL)
        {
            perror("Couldn't fetch current working directory!"); // to print error in getcwd.
            continue;
        }
        string cwd = string(curr_dir).replace(0, 14, "~/");
        cout << "(virtual) \033[1;33mnow-running-shell:\033[0m"
            << "\033[1;34m" << cwd << "\033[0m"
            << "$ ";
        
        while (true)
        {
            char c = getch();

            if (c == 18)
            {
                cout << "Searching History of commands: \n";
                searchingHistory();
                sentence.clear();
                break;
            }

            if(c == '\t')
            {
                searchDir(sentence);
                sentence.clear();
                break;
            }

            else if(c == 127) 
            {
                if(sentence.length()>0)
                {
                    sentence.pop_back();
                    fputs("\b \b", stdout);
                }
            }

            else if (c == '\n') 
            { 
                putchar(c);
                break;
            }
            else if (c >= 32 && c <= 127) 
            { 

                putchar(c);
                sentence.push_back(c);
     
            }
        }

    
            
            //Break on getting "exit"
            if (sentence == "exit")
            {   
                cout << "\033[1;31mExiting Shell...\033[0m\n";
                break;
            }
            
            vector<string> cmds;
            int b_cmd = 0, e_cmd = 0;
            for(int i=0;sentence[i]!='\0';i++){
                if(sentence[i]=='|') {
                    cmds.push_back(sentence.substr(b_cmd, e_cmd-b_cmd));
                    b_cmd = e_cmd = i+1;
                }
                else
                    e_cmd++;
            }
            cmds.push_back(sentence.substr(b_cmd, e_cmd-b_cmd));

            int no_pipes = cmds.size()-1;
            int **pipes = new int *[no_pipes];
            for(int j=0;j<no_pipes;j++){
                    pipes[j]=new int[2];
                    pipe(pipes[j]);
            }

            for(unsigned int i=0;i<cmds.size();i++) 
            {
                int in_fd = 0, out_fd = 1;
                int pre_out = -1, next_in = -1;
                if(i>0) {
                    in_fd = pipes[i-1][0];
                    pre_out = pipes[i-1][1];
                }
                if(i<cmds.size()-1) {
                    out_fd = pipes[i][1];
                    next_in = pipes[i][0];
                }

                saveCommand(cmds[i]);
                execute(cmds[i], in_fd, out_fd, pre_out, next_in);
            }
            
       fflush(stdin);
       fflush(stdout);
       sentence.clear();
    }
    return 0;
}

void saveCommand(string sentence)
{
    string save(sentence);
    history.push_back(save);
    fstream fp;
    fp.open(filename,std::ios_base::app | std::ios_base::in);

    if (history.empty()) {
        cout << "command history is empty" << endl;
    } 
         
    else 
    {
        if(fp.is_open())
        {
            int i = history.size() - 1;
            fp << " " << history.size() << " " << history[i] << endl;
        }
    }
    fp.close(); 
}

void searchingHistory()
{

    string word;

    cout<<"Enter Search Term: \n";

    getline(cin,word);

    ifstream  fp;

    fp.open(filename);

    int flag=0;
    string line;
    //getline(cin,word);
    unsigned int curLine = 0;
    while(getline(fp, line)) 
    { 
        curLine++;
        if (line.find(word) != string::npos) 
        {
            cout << "found: " << word  <<" line: " << curLine << endl;
        }
    }
}

void searchDir(string filestring)
{
    vector <string> files;

    cout<<"\nOpening Directory"<<endl;
    struct dirent *d;
    DIR *dr;
    dr = opendir("./");
    

    while ((d = readdir(dr)) != NULL) 
    {
        std::string fname = d->d_name;
        files.push_back(fname);
    }

    int j=files.size();

    int flag=0;

    vector <string> substring;
    vector <string> result;
    istringstream iss(filestring);
    for (string s; iss >> s; )
		result.push_back(s);

    if (result.size() > 1)
    {   
        string str = result.back();
        for(int i=0;i<j;i++)
        {
            if(str==files[i])
            {
                flag=1;
                for(int i = 0; i < result.size()-1; i++)
                    cout << result[i]<<" ";
                cout<<files[i]<<endl;
            }
        }

        int m=0;

        if(flag==0)
        {
            for(int i=0;i<j;i++)
            {
                if(files[i].find(str)!=string::npos)
                {
                    m++;
                    cout<<m<<" "<<files[i]<<endl;
                    substring.push_back(files[i]);

                }

            }
        }
    }

    else
    {
        for(int i=0;i<j;i++)
        {
            if(filestring==files[i])
            {
                flag=1;
                cout<<files[i]<<" "<<endl;
                return;
            }

        }

        int m=0;

        if(flag==0)
        {
            for(int i=0;i<j;i++)
            {
                if(files[i].find(filestring)!=string::npos)
                {
                    m++;
                    cout<<m<<" "<<files[i]<<endl;
                    substring.push_back(files[i]);

                }

            }
        }
    }

    
    if(substring.size() > 1)
    {
        cout<<"Select a number:"<<endl;

        int number;
        cin>>number;
        for(int i = 0; i < result.size()-1; i++)
                cout << result[i]<<" ";
        cout<<substring[number-1]<<endl;
    }
    else 
    {
        for(int i = 0; i < result.size()-1; i++)
                cout << result[i]<<" ";
        cout<<substring[0]<<endl;
    }
    

    return;
}

char getch(void)
{
 int ch;
 struct termios oldt;
 struct termios newt;
 tcgetattr(STDIN_FILENO, &oldt); 
 newt = oldt; 
 newt.c_lflag &= ~(ICANON | ECHO); 
 tcsetattr(STDIN_FILENO, TCSANOW, &newt); 
 ch = getchar(); 
 tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
 return ch; 
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char prev_dir[1000];

char def_tilda[100] = "/home"; 
char homedir[100];

void signal_handler(int signal_number) 
{
	//Do nothing
}

char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i =0; i < strlen(line); i++){

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') 
        {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0; 
            }
        } 
        else {
            token[tokenIndex++] = readChar;
        }
    }
 
    free(token);
    tokens[tokenNo] = NULL ;
    return tokens;
}

char **rectify_tilda(char **tokens)
{
    char **upd_token;
    int MAX_CHAR_FILE = 10000;
    char temp[MAX_CHAR_FILE];
    strcpy(temp,tokens[0]);    
    strcat(temp," ");
    for(int i=1;tokens[i]!=NULL;i++)
    {
        if(tokens[i][0] == '~')
        {
            strcat(temp,homedir);
            strcat(temp,tokens[i]+1);
            strcat(temp," ");
        }
        else
        {
            strcat(temp,tokens[i]);
            strcat(temp," ");
        }
    }
    upd_token = tokenize(temp);
}

void run_cd(char **tokens)
{
    char temp[1000];
    char cwd[1000];
    int check;

    if (tokens[1]==NULL || tokens[2]!=NULL)
    {
        perror("Error!! Incorrect number of inputs");
    }
    else
    {
        if(!strcmp(tokens[1],"-"))
        {
            if(strlen(prev_dir)==0)
            {
                
                printf("Error!! cd : OLDPWD not set\n");
                return;
            }
            else
            {
                strcpy(tokens[1],prev_dir);
            }
        }
        getcwd(cwd,sizeof(cwd));
        strcpy(temp,cwd);
        check = chdir(tokens[1]);
        if(check!=0)
        {
            perror("Error!! Directory does not exist");
        }
        else
        {
            getcwd(cwd,sizeof(cwd));
            strcpy(prev_dir,temp);
            printf("%s\n",cwd);
        }
    }
}

void implement_pipe(char **tokens, int pos)
{   
    if(pos==0 || tokens[pos-1]==NULL)
    {
        perror(" error near unexpected token `|'");
        exit(1);
    }
    else
    {
        int fd[2];
        int check_pipe = pipe(fd);
        
        char temp_left[1000];
        char temp_right[1000];

        char **left_cmd, **right_cmd;

        if(check_pipe==-1)
        {
            perror("Pipe failed");
            exit(1);
        }

        //Making the left command for pipe
        int l = 0;

        strcpy(temp_left,tokens[l++]);
        strcat(temp_left," ");
        
        for(;l<pos; l++)
        {
            strcat(temp_left,tokens[l]);
            strcat(temp_left," ");
        }
        left_cmd = tokenize(temp_left);

        //Making the right command for pipe
        int r = pos+1;
        strcpy(temp_right,tokens[r++]);
        strcat(temp_right," ");
        
        for(;tokens[r]!=NULL; r++)
        {
            strcat(temp_right,tokens[r]);
            strcat(temp_right," ");
        }
        right_cmd = tokenize(temp_right);

        //Two forks for the pipe
        pid_t pipe_left, pipe_right;

        //Fork for left command execution
        if((pipe_left = fork())<0)
        {
            perror("Error!! Fork failed");
            exit(1);
        }
        else if(pipe_left==0)
        {
            close(fd[0]);  // close reading end of first pipe
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            if(execvp(*left_cmd, left_cmd)<0)    
            {
                perror("Not a valid command");
                exit(1);
            }
            exit(1);
        }

        //Fork for right command execution
        if((pipe_right = fork())<0)
        {
            perror("Error!! Fork failed");
            exit(1);        
        }
        else if(pipe_right==0)
        {
            close(fd[1]);  // close writing end of first pipe
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            if(execvp(*right_cmd, right_cmd) < 0)
            {
                perror("Not a valid command");
                exit(1);
            }
            exit(1);
        }
        close(fd[0]);
        close(fd[1]);

        // Wait for two childs to exit
        wait(NULL);
        wait(NULL);
    }
}
void run_general(char **tokens)
{
    int status;
    int check_basic;
    pid_t gen_id = fork();
    int flag_pipe = 0;
    if(gen_id==0)
    {
        // Implementing IO redirections
        for(int i = 0; tokens[i]!=NULL; i++)
        {
            //Output(>) redirection Implementation
            if(!strcmp(tokens[i],">"))
            {
                if(tokens[i+1]!=NULL)
                {
                    int fd1 = creat(tokens[i+1],0644);
                    if(fd1<0)
                    {
                        perror("Error!! Could not open file");
                        exit(0);
                    }
                    else
                    {
                        dup2(fd1,STDOUT_FILENO); 
                        close(fd1);
                    }
                    int j;
                    for(j = i+2;tokens[j]!=NULL;j++)
                    {
                        strcpy(tokens[j-2],tokens[j]);
                    }
                    tokens[j-2] = '\0';
                    i = i-1;
                }
                else
                {
                    perror("Error!! No filename after token '>'");   
                }

            }

            //Input(<) redirection Implementation
            else if(!strcmp(tokens[i],"<"))
            {
                if(tokens[i+1]!=NULL)
                {
                    int fd0 = open(tokens[i+1],O_RDONLY);
                    if(fd0<0)
                    {
                        perror("Error!! Could not open file");
                        exit(0);
                    }
                    else
                    {
                        dup2(fd0,STDIN_FILENO); 
                        close(fd0);
                    }
                    int j;
                    for(j = i+2;tokens[j]!=NULL;j++)
                    {
                        strcpy(tokens[j-2],tokens[j]);
                    }
                    tokens[j-2] = '\0';
                    i = i-1;
                }
                else
                {
                    perror("Error!! No filename after token '<'");   
                }

            }
        }

        for(int i = 0; tokens[i]!=NULL; i++)
        {
            //Piping(|) implementation
            if(!strcmp(tokens[i],"|"))
            {
                implement_pipe(tokens,i);
                flag_pipe = 1;
            }
        }


        if(flag_pipe == 0)
        {
            check_basic = execvp(*tokens,tokens);
            if(check_basic < 0)
            {
                perror("Not a valid command");
                exit(1);
            }
        }
        exit(1);
    }
    else
    {
        while (wait(&status) != gen_id);
    }
}

void run_exit()
{
    exit(1);
}

void exec_token(char **tokens)
{
    char **upd_token, **cmd;
    upd_token = rectify_tilda(tokens);
    for(int i = 0; upd_token[i]!=NULL; i++)
    {
        char temp[1000];
        strcpy(temp,upd_token[i]);
        strcat(temp," ");
        int j;
        for(j = i+1; upd_token[j]!=NULL; j++)
        {
            if(!strcmp(upd_token[j],";;"))
            {
                break;
            }
            strcat(temp,upd_token[j]);
            strcat(temp," ");
        }
        i = j;
        cmd = tokenize(temp);
        if(!strcmp(cmd[0],"cd"))
            run_cd(cmd);
        else if(!strcmp(cmd[0],"exit"))
            run_exit();
        else
            run_general(cmd);
    }
}

void main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;
    printf("Please enter your ~ workspace e.g: '/home/user' : ");
    gets(homedir);

    if(strlen(homedir)==0)
    {
        strcpy(homedir,def_tilda);
    }
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    
    while (1) {           
        printf("Hello>");
        bzero(line, MAX_INPUT_SIZE);
        gets(line);           
        printf("Got command %s\n", line);
        
        if(strlen(line)!=0)
        {
        
            line[strlen(line)] = '\n'; //terminate with new line
 
            tokens = tokenize(line);
            if(tokens[0]!=NULL)
            {
                //do whatever you want with the commands, here we just print them
                exec_token(tokens);

                // Freeing the allocated memory	
                for(i=0;tokens[i]!=NULL;i++)
                {
                    free(tokens[i]);
                }
                free(tokens);
            }
        }
    }
}

                

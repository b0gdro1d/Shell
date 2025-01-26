#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#define SMAX 100
#define m 10

int pids[100]={0};
int pid,stat;

void process(char **a,long long unsigned l) {
    char **cmd=malloc(l*sizeof(char*));
    long long unsigned i=0,t=0;
    int status, fd[2]={0},cp[2]={dup(0),dup(1)};
    while (a[i]) 
    {
        if (strchr("><",a[i][0])) 
        {
		    if (a[i][0]=='<') fd[0]=open(a[i+1],O_RDONLY);
		    if (a[i][0]=='>') fd[1]=open(a[i+1], O_WRONLY | O_CREAT | (a[i][1]? O_APPEND : O_TRUNC), 0777);
		    i = a[i+1] ? i+2 : i+1;
		    continue;
	    }
	    else if (!strchr("&|",a[i][0])) cmd[t++]=a[i];
	    else 
        {
	        cmd[t]=NULL;
	        if (!strcmp(a[i],"|")) 
            {
		        while (1) 
                {
		            t=0;
		            pipe(fd);
		            if (fork()==0) 
                    {
			            if (a[i]!=NULL) dup2(fd[1],1);
			            close(fd[0]);
			            close(fd[1]);
			            execvp(cmd[0],cmd);
		            }
		            dup2(fd[0],0);
		            close(fd[0]);
		            close(fd[1]);
		            if (!a[i] || strcmp(a[i],"|")) break;
		            while (a[i+1] && strchr("&|><",a[i+1][0])==NULL) cmd[t++]=a[++i];
		            cmd[t]=NULL;
		            i++;
		        }
		        close(fd[1]);
		        while (wait(&status)!=-1);
		        if (a[i]) i++;
		        dup2(cp[1],1);
		        dup2(cp[0],0);
		        fd[0]=0;
		        fd[1]=0;
		        cmd[0]=NULL; 
                t=0;
		        continue;
	        }

	        if (fd[0]) 
            {
                dup2(fd[0], 0);
                close(fd[0]);
            }	
	        if (fd[1]) 
            {
                dup2(fd[1], 1);
                close(fd[1]);
            }
	        if (!strcmp("&",a[i])) 
            {
		        int ff=open("/dev/null",O_RDONLY);
		        dup2(ff,0);
		        close(ff);
	        }
	    
	        if (pid=fork()) 
            {
		        if (strcmp("&",a[i])!=0) waitpid(pid,&status,0);
		        else for (int j=0; j<SMAX; j++) if (pids[j]==0) 
                {
                    pids[j]=pid; 
                    break;
                }
	        }
	        else execvp(cmd[0],cmd);
	        t=0;
	        dup2(cp[0],0);
	        dup2(cp[1],1);
	        fd[0]=0;
	        fd[1]=0;
	    }
	    i += (int)(a[i]!=NULL);
    } 
    if (cmd[0]==NULL) 
    {
	    free(cmd);
	    return;
    }
    
    if (fd[0]) 
    {
        dup2(fd[0], 0);
        close(fd[0]);
    }	
    if (fd[1]) 
    {
        dup2(fd[1],1);
        close(fd[1]);
    }
	
    if (pid=fork()) waitpid(pid, &status, 0);
    else execvp(cmd[0],cmd);
    dup2(cp[0], 0);
    dup2(cp[1], 1);
    fd[0]=0;
    fd[1]=0;
    free(cmd);
    return;
}

int main()
{
    char mas_id[SMAX][SMAX];
    char first_word[SMAX];
    printf(">");
    scanf("%s", first_word);
    while ((strncmp(first_word, "exit", 4) != 0))
    {
        int k = 0;
        char inp_str[SMAX];
        char* mas_input[SMAX]={};
        fgets(inp_str, sizeof(inp_str), stdin);
        strcat(first_word, inp_str);
        strncpy(inp_str, first_word, SMAX);
        char* tok = strtok(inp_str, " ");
        while (tok != NULL)
        {
            mas_input[k] = tok;
            ++k;
            tok = strtok(NULL, " ");
        }
        mas_input[k-1][strlen(mas_input[k-1])-1]= '\0';
        process(mas_input, k);
        printf(">");
        scanf("%s", first_word);
    }
    return 0;
}
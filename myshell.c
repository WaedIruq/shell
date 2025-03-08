#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>





int prepare(){
    if(signal(SIGINT, SIG_IGN) == SIG_ERR) {
        perror("Failed to change signal SIGINT handling");
        return -1;
    }
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
        perror("Failed to change signal SIGCHLD handling");
        return -1;
    }

return 0;
}

 

 
int finalize(){
    return 0;
}

int process_arglist(int count, char** arglist){
    int i,j,status,child, ndChild, pfd[2],pipeIdx, fd;
    pid_t pid, pid2;
    char* symbol;
    child = 1;
    ndChild = 1;
    pipeIdx = 0;
    for(i=0,j=0;i<count;i++){
        if(strcmp(arglist[i],"&") == 0 || strcmp(arglist[i],"<") == 0 || strcmp(arglist[i],">") == 0 || strcmp(arglist[i],"|") == 0 ){
            j++;
            if(*arglist[i] == '|') pipeIdx = i;
            }
    }
    if(pipe(pfd) == -1){ 
                perror("Failed to pipe.");
                exit(1); }
    if(pipeIdx==0)pid = fork();            
    
    if(pid == -1){
        perror("Failed to fork.");
        exit(1);
    }
    
    if(j>=2){
        perror("THERE IS MORE THAN TWO <>&| !!!!!!!");
        exit(1);
    }
    symbol = arglist[count-1];
   
    if(strcmp(symbol, "&") == 0){ 
            if(pid == 0){
                
                arglist[count-1] = NULL;
                
                
                if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGCHLD handling");
                    exit(1);
                    }
                child = execvp(arglist[0], arglist);
                }   
            if(child == -1) {
                perror("Failed to exec.");
                exit(1);
                }
            else{ 
                return 1;
                }  
           
        }
    
    else{
        if (j == 0){
        
            if(pid==0){
                if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGINT handling");
                    exit(1);
                    }
                if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGCHLD handling");
                    exit(1);
                    }
                child = execvp(arglist[0],arglist);
                }
        }
        
        
        else if(strcmp(arglist[count-2], ">") == 0){
            

            if(pid == 0){
                arglist[count-2] = NULL;
                fd = open(arglist[count - 1], O_WRONLY | O_CREAT | O_TRUNC , 0600);
                if(fd == -1) return 0;
                if (dup2(fd, 1) == -1){
                    perror("Failed to dup");
                    exit(1);
                    }
                close(fd);
                if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGINT handling");
                    exit(1);
                    }
                if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGCHLD handling");
                    exit(1);
                    }
                child = execvp(arglist[0], arglist);
                }
            }
        else if(strcmp(arglist[count-2], "<") == 0){
            

            if(pid == 0){
                arglist[count-2] = NULL;
                fd = open(arglist[count - 1], O_RDONLY );
                if(fd == -1) return 0;
                if (dup2(fd, 0) == -1){
                    perror("Failed to dup");
                    close(fd);
                    return 0;
                    }
                close(fd);
                if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGINT handling");
                    exit(1);
                    }
                if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGCHLD handling");
                    exit(1);
                    }
                child = execvp(arglist[0], arglist);
                }
            }        

        else if(pipeIdx > 0){
            pid = fork();            
    
            if(pid == -1){
                perror("Failed to fork.");
                exit(1);
                }
            
            arglist[pipeIdx] = NULL;
            
            if(pid > 0){
                
                pid2 = fork();
                if(pid2 == -1){
                    perror("Failed to fork.");
                    exit(1);
                    }
                }
            
            if(pid > 0 && pid2 == 0){
                close(pfd[0]);
                if(dup2(pfd[1],1)==-1){
                    close(pfd[1]);
                    perror("Failed to dup.");
                    exit(1);
                }
                close(pfd[1]);
                if (signal(SIGINT, SIG_DFL) == SIG_ERR) { 
                    perror("Failed to change signal SIGINT handling");
                    exit(1);
                    }
                if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGCHLD handling");
                    exit(1);
                    }
                child = execvp(arglist[0], arglist);
                
                }
            else if(pid == 0){

                close(pfd[1]);
                if (dup2(pfd[0], 0) == -1) {
                    close(pfd[0]);
                    perror("Failed to dup.");
                    exit(1);
                    }
                close(pfd[0]);
                if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGINT handling.");
                    exit(1);
                    }
                if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
                    perror("Failed to change signal SIGCHLD handling.");
                    exit(1);
                    }
                ndChild = execvp(arglist[pipeIdx+1], arglist+pipeIdx+1);
            }
          
        }
        
    }
    if(pipeIdx>0){
        
        close(pfd[0]);
        close(pfd[1]);
    }
    waitpid(pid, NULL, 0);
    if(ndChild)waitpid(pid2, NULL, 0);
    
    if(child == -1 || ndChild == -1) {
        
        perror("Failed to exec.");
        exit(1);
        }
    
    
    return 1;
    
}


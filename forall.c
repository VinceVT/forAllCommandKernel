#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
// your code professor
//bool b;
int len;
char *argk[1024];
int count = 2;
static void sigHandler(int sig, siginfo_t *si, void *ignore)
{
    
    if (sig == SIGINT) {
        // si->si_pid sends the signal;
        //b = false;
        printf("Signaling %d\n", getpid());
        dprintf(0, "Stopped executing %s %s signal = %d", argk[1], argk[count], SIGINT);
        count++;
    }   else if (sig == SIGQUIT) {
        printf("Exiting due to quit signal\n");
        kill(0, SIGTERM);
    } else {
        dprintf(0, "Finished Executing %s %s exit code = %d \n", argk[1], argk[count], 0);
        count++;
        //b = true;
    }
}

int main(int argc, char *argv[]) 
{
    pid_t pid;
    len = argc;
    for (int m = 0; m < len; m++) {
        argk[m] = argv[m];
    }
    struct sigaction sigA;
    sigA.sa_flags = SA_SIGINFO;
    sigemptyset(&sigA.sa_mask);
    sigA.sa_sigaction = sigHandler;
    int pi;
    char *f;
    int number = 1;
    char file[FILENAME_MAX];
    if (sigaction(SIGINT, &sigA, NULL) == -1)   {
        perror("sigaction signal for SIGINT failed");
        exit(errno);
    }   
    if (sigaction(SIGQUIT, &sigA, NULL) == -1)  {
        perror("sigaction signal for SIGQUIT failed");
        exit(errno);
    }
    for (int i = 2; i < len; i++) {
        //char conversion, saw this on discussion to help
        sprintf(file, "%d.out", number);
        f = file;
        int outPut = open(f, O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR);

        pid = fork();
        close(0);
        close(2);
        dup2(outPut, 0);
        dup2(outPut, 2);
        //close(outPut);
        //child fork
        if (pid == 0) {
            pi = waitpid(pid, NULL, 0);
            sigaction(SIGINT, &sigA, NULL);
            sigaction(SIGQUIT, &sigA, NULL);
            dprintf(0, "Executing %s %s \n", argv[1], argv[i]);
            execlp(argv[1], argv[1], argv[i], NULL);
        } else if (pid < 0) {
            //No processes to fork left
            perror("Error: could not fork!");
        } else {
            //parent process
            //pi = waitpid(pid1, NULL, 0); - for parent pi
            wait(NULL);
            
            // if (b == false)    {
            //     wait(NULL);
            //     dprintf(0, "Stopped executing %s %s signal = %d", argv[1], argv[i], SIGINT);
            // }   else {
            //     wait(NULL);
            //     dprintf(0, "Finished Executing %s %s exit code = %d \n", argv[1], argv[i], pi);
            // }
        }
        number++;
    }
    _exit(0);
    return 0;
}
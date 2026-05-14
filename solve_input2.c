#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main() {

    char *argv[101];
    for (int i = 0; i < 100; i++) {
        argv[i] = "a";
    }
//s1
    argv['A'] = "\x00";
    argv['B'] = "\x20\x0a\x0d";
    argv['C'] = "5555"; // Chọn một port bất kỳ cho Stage 5
    argv[100] = NULL;

//s2
    int pipe_stdin[2];
    int pipe_stderr[2];
    if(pipe(pipe_stdin) < 0 || pipe(pipe_stderr) < 0) exit(1);

//s3
    char *envp[] = {"\xde\xad\xbe\xef=\xca\xfe\xba\xbe", NULL};

//s4
    FILE* fp = fopen("\x0a", "w");
    fwrite("\x00\x00\x00\x00", 4, 1, fp);
    fclose(fp);

    pid_t pid = fork();
    if(pid == 0) {
        dup2(pipe_stdin[0], 0);
        dup2(pipe_stderr[0], 2);

        close(pipe_stdin[1]);
        close(pipe_stderr[1]);

        execve("/home/input2/input2", argv, envp);

    }
    else {
        write(pipe_stdin[1], "\x00\x0a\x00\xff", 4);
        write(pipe_stderr[1], "\x00\x0a\x02\xff", 4);

        sleep(2);
        int sd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in saddr;
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        saddr.sin_port = htons(atoi(argv['C']));

        if(connect(sd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
            printf("Connect failed\n");
            exit(1);
        }

        send(sd, "\xde\xad\xbe\xef", 4, 0);
        close(sd);

        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}
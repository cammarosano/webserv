#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    char *env[9] = {"SERVER_PORT=3000",
                    "SERVER_NAME=localhost",
                    "SERVER_SOFTWARE=WEBSERV/1.0",
                    "SERVER_PROTOCOL=HTTP/1.0",
                    "GATEWAY_INTERFACE=CGI/1.1",
                    "REQUEST_METHOD=GET",
                    "SCRIPT_NAME=CGI/ubuntu_cgi_tester",
                    "PATH_INFO=/index.html/",
                    NULL};

    char *ex = "./ubuntu_cgi_tester";
    char **arg = NULL;
    pid_t id = fork();
    if (id == 0) {
        execve("./cgi_tester", arg, env);
        printf("Error: cant find %s\n", ex);
        exit(EXIT_FAILURE);
    }
    waitpid(id, NULL, 0);
    return 0;
}
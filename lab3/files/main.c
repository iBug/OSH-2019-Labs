// File: main.c
// Author: iBug

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "config.h"
#include "globals.h"
#include "util.h"
#include "signals.h"
#include "status_code.h"
#include "reqhandler.h"

int main(){
    register_signal_handlers();
    char *s = realpath(DOCUMENT_ROOT, NULL);
    strcpy(document_root, s);
    free(s);
    document_root_len = strlen(document_root);

    server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sock == -1)
        errorexit("socket");
    const unsigned opt = 1U;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        errorexit("setsockopt");

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(BIND_IP_ADDR),
        .sin_port = htons(BIND_PORT)
    };
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        errorexit("bind");
    if (listen(server_sock, MAX_CONN) == -1)
        errorexit("listen");
    eprintf("Listening on %s port %d...\n", BIND_IP_ADDR, BIND_PORT);

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    while (1)
    {
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
        pid_t pid = fork(); // Fork a child
        if (pid == 0) {
            close(server_sock);
            handle_client(client_sock);
            _exit(0);
        }
        close(client_sock);
    }
}

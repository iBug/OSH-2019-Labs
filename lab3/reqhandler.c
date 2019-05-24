// File: reqhandler.c
// Author: iBug

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include "config.h"
#include "globals.h"
#include "util.h"
#include "signals.h"
#include "status_code.h"

void handle_client(int client_sock)
{
    char* req = malloc(MAX_RECV_LEN * sizeof(char));
    ssize_t req_len = 0;

    char* path = malloc(MAX_PATH_LEN * sizeof(char));

    char* response = malloc(MAX_SEND_LEN * sizeof(char)) ;

    while (req_len < MAX_RECV_LEN) {
        ssize_t read_len = read(client_sock, req + req_len, MAX_RECV_LEN - req_len);
        req_len += read_len;
        // Examine the content for the 2nd space
        req[MAX_RECV_LEN - 1] = 0;
        char* target = strchr(req + 4, ' ');
        if (target != NULL) {
            // Target found
            *target = 0;
            break;
        }
    }

    if (strncmp(req, "GET ", (size_t)4U)) {
        sprintf(response, "HTTP/1.0 " STATUS_501 "\r\nContent-Length: 0\r\n\r\n");
        write(client_sock, response, strlen(response));
        close(client_sock);
        free(req);
        free(path);
        free(response);
        return;
    }

    // Construct path
    strcpy(path, DOCUMENT_ROOT);
    strcat(path, req + 4);

    // open(2) and stat(2) the file
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            sprintf(response, "HTTP/1.0 " STATUS_404 "\r\nContent-Length: 0\r\n\r\n");
        } else {
            sprintf(response, "HTTP/1.0 " STATUS_500 "\r\nContent-Length: 0\r\n\r\n");
        }
        write(client_sock, response, strlen(response));
        free(req);
        free(path);
        free(response);
        return;
    }
    struct stat st;
    if (fstat(fd, &st) == -1) {
        sprintf(response, "HTTP/1.0 " STATUS_500 "\r\nContent-Length: 0\r\n\r\n");
        write(client_sock, response, strlen(response));
    } else if ((st.st_mode & S_IFMT) != S_IFREG && (st.st_mode & S_IFMT) != S_IFLNK) {
        sprintf(response, "HTTP/1.0 " STATUS_403 "\r\nContent-Length: 0\r\n\r\n");
        write(client_sock, response, strlen(response));
    } else {
        sprintf(response,
                "HTTP/1.0 " STATUS_200 "\r\n"
                "Content-Length: %zd\r\n"
                "\r\n",
                // "Content-Type: application/octet-stream\r\n\r\n",
                st.st_size);
        write(client_sock, response, strlen(response));

        const ssize_t size = st.st_size;
        void *mm;
        off_t off = 0L;
        while (size - off >= MAX_SEND_LEN) {
            mm = mmap(NULL, MAX_SEND_LEN, PROT_READ, MAP_PRIVATE, fd, off);
            if (mm == (void*)-1)
                goto END;
            write(client_sock, mm, MAX_SEND_LEN);
            munmap(mm, MAX_SEND_LEN);
            off += MAX_SEND_LEN;
        }
        if (size - off > 0) {
            int s = size - off;
            mm = mmap(NULL, s, PROT_READ, MAP_PRIVATE, fd, off);
            if (mm == (void*)-1)
                goto END;
            write(client_sock, mm, s);
            munmap(mm, s);
            off += s;
        }
    }
END:
    close(fd);
    close(client_sock);

    free(req);
    free(path);
    free(response);
}

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main() {
    const char* host = "httpbin.org";
    const char* port = "80";
    const char* path = "/get";

    // 1. Resolve hostname
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    // 2. Create socket
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 3. Connect to server
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    // 4. Send HTTP GET request
    std::string request = 
        "GET " + std::string(path) + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: close\r\n\r\n";

    send(sock, request.c_str(), request.length(), 0);

    // 5. Receive and print response
    char buffer[4096];
    int bytes;
    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        std::cout << buffer;
    }

    // 6. Cleanup
    close(sock);
    //freeaddrinfo(res);

    return 0;
}

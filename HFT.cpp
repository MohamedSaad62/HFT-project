#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
class request {
    const char* host = "httpbin.org";
    const char* path = "/get";
    const char* port = "443";
    addrinfo *res;
    SSL_CTX* ctx;
    int sock;
    SSL* ssl;
    public:
    static void init_ssl() {
        static bool init = false; 
        if (!init) {
            SSL_library_init();
            SSL_load_error_strings();
            init = true;
        }
    }
    request(){
        const SSL_METHOD* method = SSLv23_client_method();
        ctx = SSL_CTX_new(method);
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        getaddrinfo(host, port, &hints, &res);

        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        connect(sock, res->ai_addr, res->ai_addrlen);
        SSL_connect(ssl);
    }
    void send(std::string request){
        SSL_write(ssl, request.c_str(), request.length());
        char buffer[4096];
        int bytes;
        while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes] = '\0';
            std::cout << buffer;
        }
    }
    ~request(){
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
        freeaddrinfo(res);
        SSL_CTX_free(ctx);
    }
};

int main() {
    request::init_ssl();
    request r;
    const char* host = "httpbin.org";
    const char* path = "";
    std::string request =
        "GET / HTTP/1.1\r\n"
        "Host: " + std::string(host) + "\r\n"
        "Connection: close\r\n\r\n";
    r.send(request);

    return 0;
}

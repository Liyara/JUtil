#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>

#include <JUtil/jutil.h>
JUTIL_IMPL;

jutil::Queue<jutil::Queue<int>* > queue;
jutil::Mutex mutex;

class CTask : public jutil::Task {
    public:
    void run() override {
        mutex.lock();
        jutil::Queue<int> printer;
        queue.insert(&printer);
        mutex.unlock();
        for (int i = 0; i < 5; i++) {
            printer.insert(i);
            jutil::out << jutil::Thread::getThreadID() << " -> " << printer.last() << jutil::endl;
        }
    }
};

class CThread : public jutil::Thread {
    public:
    void run() {
        jutil::out << "Hello World" << jutil::endl;
    }
};

int main() {
    jutil::ThreadPool<16, 100> pool;
    pool.start();

    while (true) {
        jutil::String in;
        jutil::in >> in;
        jutil::Queue<jutil::String> cmd = jutil::split(in, ' ');
        if (cmd[0] == "exit") {
            pool.stop();
            break;
        } else if (cmd[0] == "add") {
            size_t n = 1;
            if (cmd.size() > 1) {
                n = static_cast<size_t>(cmd[1]);
            }
            for (size_t i = 0; i < n; i++) pool.submit(new CTask());
        }
    }

    return 0;
}

/*#define BUFFER_SIZE 1024

int main() {
    // Server information
    const char *server_url = "www.google.com";
    const int server_port = 80;
    const char *request = "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n";

    // Resolve URL to IP address
    struct addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(server_url, NULL, &hints, &result) != 0) {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }

    // Create socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set the socket to non-blocking mode
    int flags = fcntl(client_socket, F_GETFL, 0);
    fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

    // Server address structure
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    memcpy(&server_addr.sin_addr, &((struct sockaddr_in*)result->ai_addr)->sin_addr, sizeof(struct in_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    freeaddrinfo(result);

    // Initiate connection to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        if (errno != EINPROGRESS) {
            perror("Connection failed");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
    }

    // Use select to wait for the connection to complete
    fd_set write_fds;
    FD_ZERO(&write_fds);
    FD_SET(client_socket, &write_fds);

    struct timeval timeout;
    timeout.tv_sec = 5;  // Adjust the timeout as needed
    timeout.tv_usec = 0;

    int select_result = select(client_socket + 1, NULL, &write_fds, NULL, &timeout);

    if (select_result <= 0) {
        perror("Connection timeout or error");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Connection is now established, proceed with sending the request
    send(client_socket, request, strlen(request), 0);

    // Receive and print the response
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received == -1) {
            // No data available, continue waiting
            continue;
        } else if (bytes_received == 0) {
            // Connection closed by the server
            break;
        }

        // Print the received data
        fwrite(buffer, 1, bytes_received, stdout);
    }

    // Close the socket
    close(client_socket);

    return 0;
}

/*void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    return sockfd;
}

void connect_to_server(int sockfd, const char *hostname, int port) {
    struct sockaddr_in server_addr;
    struct hostent *server;

    server = gethostbyname(hostname);
    if (server == NULL) {
        perror("Could not resolve hostname.");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        if (errno != EINPROGRESS) {
            perror("Connection failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }
}

void send_http_request(int sockfd, const char *hostname, const char *path) {
    char request[BUFFER_SIZE];
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, hostname);

    fd_set write_fds;
    FD_ZERO(&write_fds);
    FD_SET(sockfd, &write_fds);

    struct timeval timeout;
    timeout.tv_sec = 10;  // Adjust the timeout as needed
    timeout.tv_usec = 0;

    int select_result = select(sockfd + 1, NULL, &write_fds, NULL, &timeout);

    if (select_result <= 0) {
        perror("Connection timeout or error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("Error sending request");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

void receive_and_print_response(int sockfd) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);

        if (bytes_received == -1) {
            // No data available, continue waiting
            continue;
        } else if (bytes_received == 0) {
            // Connection closed by the server
            break;
        }

        // Print the received data
        fwrite(buffer, 1, bytes_received, stdout);
    }
}

int main() {
    const char *hostname = "www.example.com";
    const int port = 80;
    const char *path = "/";

    int sockfd = create_socket();
    connect_to_server(sockfd, hostname, port);
    send_http_request(sockfd, hostname, path);
    receive_and_print_response(sockfd);

    close(sockfd);
    return 0;
}*/

/*#include <JUtil/jutil.h>
JUTIL_IMPL

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

int main() {
    Client client;
    auto response = client.request("google.com", 80);
    out << response << endl;

    gethostbyname("google.com");

    /*struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char sp[4];
    snprintf(sp, sizeof(sp), "%d", 80);

    if (getaddrinfo("google.com", sp, &hints, &res) != 0) {
        err << "getaddrinfo failed" << endl;
        return 1;
    }

    struct addrinfo* i;

    for(i = res; i != NULL; i = i->ai_next) {
        char str[INET6_ADDRSTRLEN];
        if (i->ai_addr->sa_family == AF_INET) {
            struct sockaddr_in *p = (struct sockaddr_in *)i->ai_addr;
            printf("%s\n", inet_ntop(AF_INET, &p->sin_addr, str, sizeof(str)));
        } else if (i->ai_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *p = (struct sockaddr_in6 *)i->ai_addr;
            printf("%s\n", inet_ntop(AF_INET6, &p->sin6_addr, str, sizeof(str)));
        }
    }


    return 0;
}*/

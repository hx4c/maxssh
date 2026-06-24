#ifndef _WIN32

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout
            << "Usage: "
            << argv[0]
            << " <server-ip>\n";

        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(2222);

    inet_pton(AF_INET,
              argv[1],
              &server.sin_addr);

    if (connect(sock,
                reinterpret_cast<sockaddr*>(&server),
                sizeof(server)) < 0)
    {
        std::cerr << "connect failed\n";
        return 1;
    }

    std::cout << "Connected.\n";

    std::string line;
    char buffer[4096];

    while (true)
    {
        std::cout << "> ";

        if (!std::getline(std::cin, line))
            break;

        line += "\n";

        send(sock,
             line.c_str(),
             line.size(),
             0);

        int received =
            recv(sock,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

        if (received <= 0)
            break;

        buffer[received] = '\0';

        std::cout << buffer;

        if (line == "QUIT\n")
            break;
    }

    close(sock);
}

#endif

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

std::string handle_command(const std::string& cmd)
{
    if (cmd == "PING")
        return "PONG\n";

    if (cmd == "TIME")
    {
        std::time_t now = std::time(nullptr);
        return std::string(std::ctime(&now));
    }

    if (cmd.rfind("ECHO ", 0) == 0)
        return cmd.substr(5) + "\n";

    if (cmd == "QUIT")
        return "BYE\n";

    return "UNKNOWN COMMAND\n";
}

int main()
{
    WSADATA wsa{};

    if (WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(2222);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server,
             reinterpret_cast<sockaddr*>(&addr),
             sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "bind failed\n";
        return 1;
    }

    listen(server, SOMAXCONN);

    std::cout << "Listening on 0.0.0.0:2222\n";

    while (true)
    {
        SOCKET client = accept(server, nullptr, nullptr);

        if (client == INVALID_SOCKET)
            continue;

        std::cout << "Client connected\n";

        char buffer[4096];

        while (true)
        {
            int received =
                recv(client, buffer, sizeof(buffer) - 1, 0);

            if (received <= 0)
                break;

            buffer[received] = '\0';

            std::string cmd(buffer);

            while (!cmd.empty() &&
                   (cmd.back() == '\n' ||
                    cmd.back() == '\r'))
            {
                cmd.pop_back();
            }

            std::string response = handle_command(cmd);

            send(client,
                 response.c_str(),
                 static_cast<int>(response.size()),
                 0);

            if (cmd == "QUIT")
                break;
        }

        closesocket(client);
        std::cout << "Client disconnected\n";
    }

    closesocket(server);
    WSACleanup();
}

#endif



#include "tools/socket.h"
#include <stdio.h>

int main()
{
    tools::CSocket s(tools::CSocket::eSocketType_UDP);
    s.Create();
    s.Bind(10234);
    char    buffer[4096];
    int     ret;
    while (true) {
        ret = s.Recv(buffer, sizeof(buffer));
        buffer[ret] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }
    return 0;
}


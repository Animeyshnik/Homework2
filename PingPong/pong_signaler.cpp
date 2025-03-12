#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <unistd.h>

struct message {
    long type;
    char text[10];
};

int main() {
    key_t key = ftok("msgqueue", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    message msg;
    while (true) {
        msgrcv(msgid, &msg, sizeof(msg.text), 1, 0);
        std::cout << "Received: " << msg.text << std::endl;

        strcpy(msg.text, "PONG");
        msg.type = 2;
        msgsnd(msgid, &msg, sizeof(msg.text), 0);
        std::cout << "Sent: " << msg.text << std::endl;
        sleep(1);
    }

    return 0;
}


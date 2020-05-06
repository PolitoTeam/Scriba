//
// Created by enrico on 06/05/19.
//

#ifndef LAB_03_NETWORKSERVER_H
#define LAB_03_NETWORKSERVER_H

#include <map>
#include <queue>
#include "Message.h"
class SharedEditor;

class NetworkServer {
private:
    int id = 0;
    std::map<int, SharedEditor *> editors;
    std::queue<Message> queue;

public:
    int connect(SharedEditor *sharedEditor);
    void disconnect(SharedEditor *sharedEditor);
    void send(const Message& m);
    void dispatchMessages();
};

#endif //LAB_03_NETWORKSERVER_H

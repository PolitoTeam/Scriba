//
// Created by Giuseppe Pastore on 2019-05-06.
//

#ifndef ESERCITAZIONE_3_NETWORKSERVER_H
#define ESERCITAZIONE_3_NETWORKSERVER_H
#include <vector>
#include <queue>
#include <map>
#include "Message.h"

class SharedEditor;


class NetworkServer {
private:
    std::vector<SharedEditor*> sharedEditors;
    std::queue<Message> msg;
    int counter;
public:
    NetworkServer(): counter(0) {};
    int connect(SharedEditor* sharedEditor);
    void disconnect(SharedEditor* sharedEditor);
    void send(const Message& m);
    void dispatchMessages();
};


#endif //ESERCITAZIONE_3_NETWORKSERVER_H

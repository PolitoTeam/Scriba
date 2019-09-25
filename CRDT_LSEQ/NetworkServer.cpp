//
// Created by Giuseppe Pastore on 2019-05-06.
//
#include "SharedEditor.h"
#include "NetworkServer.h"


int NetworkServer::connect(SharedEditor* sharedEditor){
    sharedEditors.push_back(sharedEditor);
    return ++counter;
}

void NetworkServer::disconnect(SharedEditor* sharedEditor){
    auto i=find(sharedEditors.begin(),sharedEditors.end(),sharedEditor);
    sharedEditors.erase(i);
}

void NetworkServer::send(const Message& m){
    msg.push(m);
}

void NetworkServer::dispatchMessages() {
    while (!msg.empty()){
        Message m=msg.front();
        for_each(sharedEditors.begin(),sharedEditors.end(),
                [m](SharedEditor* e) {
                    if (e->getSiteId()!=m.getSiteId()) {
                        e->process(m);
                    }
        });
        msg.pop();
    }
}


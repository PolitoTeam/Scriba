//
// Created by enrico on 06/05/19.
//

#include "NetworkServer.h"
#include "SharedEditor.h"

int NetworkServer::connect(SharedEditor *sharedEditor) {
    this->editors[this->id] = sharedEditor;
    return this->id++;
}

void NetworkServer::disconnect(SharedEditor *sharedEditor) {
    editors.erase(sharedEditor->getId());
}

void NetworkServer::send(const Message& m) {
    this->queue.push(m);
}

void NetworkServer::dispatchMessages() {
    while (!queue.empty()) {
        Message m = queue.front();
        for (auto e : editors) {
            if (e.second->getId() != m.getEditorId()) {
                e.second->process(m);
            }
        }
        queue.pop();
    }
}
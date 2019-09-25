//
// Created by Giuseppe Pastore on 2019-05-06.
//

#include "Message.h"

std::vector<int> Message::getPos() const{
    return pos;
}

int Message::getType() const {
    return type;
}

std::pair<int,int> Message::getId() const {
    return id;
}

char Message::getValue() const {
    return c;
}

int Message::getSiteId() const {
    return siteId;
}
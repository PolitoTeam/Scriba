//
// Created by Giuseppe Pastore on 2019-05-06.
//

#include "Symbol.h"

std::pair<int,int> Symbol::getId() const{
    return id;
}

char Symbol::getValue() const{
    return c;
}

std::vector<int> Symbol::getPos() const{
    return pos;
}
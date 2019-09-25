//
// Created by Giuseppe Pastore on 2019-05-06.
//

#ifndef ESERCITAZIONE_3_MESSAGE_H
#define ESERCITAZIONE_3_MESSAGE_H

#include <string>
#include <vector>

class Message {
private:
    int type;
    int siteId;
    char c;
    std::pair<int,int> id;
    std::vector<int> pos;
public:
    // 1->inserimento, 0 -->cancellazione
    Message(int type, int siteId,char c,std::pair<int,int>id, std::vector<int> pos): type(type),siteId(siteId), c(c), id(id),pos(pos) {};
    std::vector<int> getPos() const;
    int getType() const;
    std::pair<int,int> getId() const;
    char getValue() const;
    int getSiteId() const;
    //Message(){};
};


#endif //ESERCITAZIONE_3_MESSAGE_H

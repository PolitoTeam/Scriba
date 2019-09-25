//
// Created by Giuseppe Pastore on 2019-05-06.
//

#ifndef ESERCITAZIONE_3_SHAREDEDITOR_H
#define ESERCITAZIONE_3_SHAREDEDITOR_H
#include "NetworkServer.h"
#include "Symbol.h"
#include "Message.h"

class SharedEditor {
private:
    NetworkServer& _server;
    int _siteId;
    int _counter;
    // multidimensional array
    // std::vector<std::vector<Symbol>> _lines;

    //unique array of character...includes newline
    std::vector<Symbol> _symbols;
    bool randomBool();
    int randomNum(int n1,int n2);
    std::vector<int> prefix(std::vector<int> id,int depth,int s);
    std::map<int,bool> strategy;
    std::vector<int> LSEQ_alloc(std::vector<int> pos1,std::vector<int> pos2);
public:
    explicit SharedEditor(NetworkServer& ns): _server(ns),_counter(0) {_siteId=ns.connect(this);
        strategy.insert(std::pair<int,bool>(0,true));};
    void localInsert(int index, char value);
    void localErase(int index);
    void process(const Message& m);
    std::string to_string();
    int getSiteId() const;
    ~SharedEditor();
};


#endif //ESERCITAZIONE_3_SHAREDEDITOR_H

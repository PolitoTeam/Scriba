//
// Created by enrico on 06/05/19.
//

#ifndef LAB_03_SHAREDEDITOR_H
#define LAB_03_SHAREDEDITOR_H

#include <iostream>
#include <iomanip>
#include <exception>
#include <random>
#include <functional>
#include "Symbol.h"
#include "NetworkServer.h"

#define BASE 32
#define BOUNDARY 10

class SharedEditor {
private:
    NetworkServer& _server;
    int _siteId;
    std::vector<Symbol> _symbols;
    int _counter = 0;

    std::map<int,bool> strategyCache;


public:
    SharedEditor(NetworkServer &server) : _server(server) {
        _siteId = _server.connect(this);
    };
    int getId() { return _siteId; }
    ~SharedEditor() { _server.disconnect(this); };

    void localInsert(int index, char value) {
        if (index < 0 || index > _symbols.size())
            throw std::runtime_error("Error: index out of bound.\n");

        // calculate position
        std::vector<Identifier> posBefore, posAfter, newPos;
        if (!_symbols.empty() && index != 0)
            posBefore = _symbols[index - 1].getPosition();
        if (!_symbols.empty() && index < _symbols.size())
            posAfter = _symbols[index].getPosition();
        std::vector<Identifier> position = generatePositionBetween(posBefore, posAfter, newPos);

        // generate symbol
        Symbol s(value, newPos, ++_counter);
        _symbols.insert(_symbols.begin() + index, s);

        // broadcast
        Message m(_siteId, s, INSERT);
        _server.send(m);
    }

    std::vector<Identifier> generatePositionBetween(std::vector<Identifier>& pos1, std::vector<Identifier> pos2, std::vector<Identifier>& newPos, int level=0) { // TODO: pass vectors by reference?
        Identifier id1 = level < pos1.size() ? pos1[0] : Identifier(0, this->_siteId);
        Identifier id2 = level < pos2.size() ? pos2[0] : Identifier(BASE, this->_siteId); // == BASE * std::pow(2, 0)

        if (id2.digit - id1.digit > 1) {
            // case 1: enough space to add in between
            int newDigit = generateIdBetween(id1.digit, id2.digit, level);
            newPos.push_back(Identifier(newDigit, this->_siteId));
            return newPos;
        } else if (id2.digit - id1.digit == 1) {
            // case 2: no space in between, use identifier of first position
            newPos.push_back(id1);
            return generatePositionBetween(pos1, std::vector<Identifier> {}, newPos, level + 1);
        } else if (id1.digit == id2.digit) {
            // case 3: same digit, use site id to discriminate
            if (id1.site < id2.site) {
                newPos.push_back(id1);
                return generatePositionBetween(pos1, std::vector<Identifier> {}, newPos, level + 1);
            } else if (id1.site == id2.site) {
                newPos.push_back(id1);
                return generatePositionBetween(pos1, pos2, newPos, level + 1);
            } else {
                throw std::runtime_error("Invalid ordering");
            }
        }
    }

    int generateIdBetween(int id1, int id2, int level) {
        int interval = id2 - id1;
        std::cout << "ids: " << id1 << " " << id2 << std::endl;

        if (strategyCache.find(level) == strategyCache.end()) {
            strategyCache[level] = generateRandomBool();
        }

        int step=std::min(BOUNDARY, interval);
        std::cout << "step: " << step << std::endl;
        if (strategyCache[level]) { //boundary+
            int delta = generateRandomNumBetween(1, step - 1);
            std::cout << "Random+ ="<< delta << std::endl;
            return id1 + delta;
        }
        else{ //boundary-
            int delta = generateRandomNumBetween(1, step - 1);
            std::cout << "Random- =" << delta << std::endl;
            return id2 - delta;
        }
    }

    bool generateRandomBool() {
        static auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
        return gen();
    }

    // returns n1 <= x <= n2
    int generateRandomNumBetween(int n1,int n2) { // TODO: check if better to use uniform_int_distribution
        return n1 + (std::rand() % (n2 - n1 + 1));
    }

    void localErase(int index) {
        Symbol s = _symbols[index];
        _symbols.erase(_symbols.begin() + index);

        // broadcast
        Message m(_siteId, s, DELETE);
        _server.send(m);
    }

    void process(const Message& m) {
        if (m.getOperation() == INSERT) {
            // find leftmost index (position) for insertion
            int index = findInsertIndex(m.getSymbol());
            if (index < 0)
                throw std::runtime_error("Invalid index for insertion");
            _symbols.insert(_symbols.begin() + index, m.getSymbol());
        } else { // DELETE
            // find index (position) of the symbol to delete
            int index = findIndexByPosition(m.getSymbol());
            if (index >= 0) // otherwise already deleted by another editor (i.e. another site)
                _symbols.erase(_symbols.begin() + index);
        }
    }

    // linear search
//    int findIndexByPosition(std::vector<Symbol> _symbols, Symbol s) {
//        for (int i = 0; i < _symbols.size(); i++) {
//            if (comparePositions(s, _symbols[i]) == 0)
//                return i;
//        }
//        return -1;
//    }

    // binary search
    int findIndexByPosition(Symbol s) {
        int l = 0, r = _symbols.size() - 1, m, comp;

        while (l <= r) {
            m = (l + r) / 2;
            comp = Symbol::compare(s.getPosition(), _symbols[m].getPosition());
            if (comp == 0) {
                return m;
            } else if (comp > 0) {
                l = m + 1;
            } else {
                r = m - 1;
            }
        }
        return -1;
    }

//    // find leftmost linear
//    int findInsertIndex(std::vector<Symbol> _symbols, Symbol s) {
//        Symbol dummyTail = Symbol('/', "-1", std::vector<int>{UPPER_BOUND + 1});
//        _symbols.push_back(dummyTail);
//
//        int i;
//        for (i = 0; i < _symbols.size(); i++) {
//            if (comparePositions(s, _symbols[i]) < 0) {
//                break;
//            }
//        }
//        _symbols.erase(_symbols.end() - 1); // remove dummyTail
//        return i;
//    }

    // find leftmost binary
    int findInsertIndex(Symbol s) {
        int l = 0, r = _symbols.size() - 1, comp;

        while (l <= r) {
            int m = (l + r) / 2;
            comp = Symbol::compare(s.getPosition(), _symbols[m].getPosition());
            if (comp < 0)
                r = m - 1;
            else if (comp > 0)
                l = m + 1;
            else
                throw std::runtime_error("Non valid index");
        }
        return l;
    }

//    std::string to_string() {
//        std::string text("");
//        for (auto s : _symbols)
//            text += s.getValue();
//        return text;
//    }
    std::string to_string(){ // TODO: don't use concatenation, stringsteam???
        std::string str = "";
        bool first = true;
        for(Symbol s: _symbols){
            if (first) {
                first = false;
            } else {
                str += ", ";
            }
            str += s.to_string();

        }
        return str;
    }
};

#endif //LAB_03_SHAREDEDITOR_H

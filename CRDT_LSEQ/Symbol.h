//
// Created by Giuseppe Pastore on 2019-05-06.
//

#ifndef ESERCITAZIONE_3_SYMBOL_H
#define ESERCITAZIONE_3_SYMBOL_H
#include <vector>


class Symbol {
private:
    char c;
    std::vector<int> pos;
    std::pair<int,int> id;
public:
    Symbol(){};
    Symbol(char c, int site_ID, int site_counter, std::vector<int> pos): c(c) {
        id=std::make_pair(site_ID,site_counter);
        this->pos=pos;
    }
    std::vector<int> getPos() const;
    std::pair<int,int> getId() const;
    char getValue() const;
};


#endif //ESERCITAZIONE_3_SYMBOL_H

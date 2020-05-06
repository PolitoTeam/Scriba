//
// Created by enrico on 06/05/19.
//

#ifndef LAB_03_SYMBOL_H
#define LAB_03_SYMBOL_H

#include <string>
#include <vector>

class Identifier {
public:
    int digit;
    int site;

public:
    Identifier(int digit, int site) : digit(digit), site(site) {}

    static int compare(const Identifier& i1, const Identifier& i2) {
        if (i1.digit < i2.digit) {
            return -1;
        } else if (i1.digit > i2.digit) {
            return 1;
        } else {
            if (i1.site < i2.site) {
                return -1;
            } else if (i1.site > i2.site) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    std::string to_string() {
        return std::to_string(digit) + "_" + std::to_string(site);
    }
};

class Symbol {
private:
    char value;
    std::vector<Identifier> position;
    bool italic, bold, underline; // TODO: add these attributes
    int counter; // TODO: when/where is it used???

public:
    Symbol(char value, std::vector<Identifier> position, int counter) : value(value), position(position), counter(counter) {};
    char getValue() { return value; }
    std::vector<Identifier> getPosition() const { return position; }
    int getCounter() const { return counter; }

    static int compare(const std::vector<Identifier>& p1, const std::vector<Identifier>& p2) {
        for (int i = 0; i < std::min(p1.size(), p2.size()); i++) {
            int comp = Identifier::compare(p1[i], p2[i]);
            if (comp != 0) {
                return comp;
            }
        }
        if (p1.size()< p2.size()) {
            return - 1;
        } else if (p1.size() > p2.size()) {
            return 1;
        } else {
            return 0;
        }
    }

    std::string to_string() {
        std::string result = std::string(1, value) + "[";
        bool first = true;

        for (Identifier i : position) {
            if (first) {
                first = false;
            } else {
                result += ", ";
            }
            result += i.to_string();
        }
        result += "]";
        return result;
    }
};

#endif //LAB_03_SYMBOL_H

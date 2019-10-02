#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>

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

    QString to_string() {
        return QString::number(digit) + "_" + QString::number(site);
    }

    QJsonObject toJson() {
        QJsonObject json;
        json["digit"] = digit;
        json["site"] = site;
        return json;
    }
};

class Symbol
{
private:
    char value;
    std::vector<Identifier> position;
    bool italic, bold, underline; // TODO: add these attributes
    int counter; // TODO: when/where is it used???

public:
    Symbol(char value, std::vector<Identifier> position, int counter) : value(value), position(position), counter(counter) {};
    char getValue() const { return value; }
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

    QString to_string() {
        QString result = QString(1, value) + "[";
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

    QJsonObject toJson() {
        QJsonObject json;

        json["value"] = QString(1, value);
        QJsonArray jsonArray;
        for (Identifier i : position) {
            jsonArray.append(i.toJson());
        }
        json["position"] = jsonArray;
        json["counter"] = counter;

        return json;
    }
};

#endif // SYMBOL_H

#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>

class Identifier {
public:
    int digit;
    int site;

public:
    Identifier() {} // empty constructor needed, otherwise compile error
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
    QVector<Identifier> position;
    bool italic, bold, underline; // TODO: add these attributes
    int counter; // TODO: when/where is it used???

public:
    Symbol() {} // empty constructor needed, otherwise compile error
    Symbol(char value, QVector<Identifier> position, int counter) : value(value), position(position), counter(counter) {}
    char getValue() const { return value; }
    QVector<Identifier> getPosition() const { return position; }
    int getCounter() const { return counter; }

    static int compare(const Symbol& s1, const Symbol& s2) {
        QVector<Identifier> p1 = s1.getPosition();
        QVector<Identifier> p2 = s2.getPosition();

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
        QString value_string = (value == '\n') ? "NL" : QString(1, value);
        QString result = value_string + "[";
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

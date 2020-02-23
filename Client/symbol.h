#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QFont>
#include <QJsonObject>
#include <QTextCharFormat>
#include <QDebug>

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

class SymbolFormat {
public:
    enum Alignment {ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER} left;
    bool italic, bold, underline;
    QString font;
    double size;

    QJsonObject toJson() {
        QJsonObject json;

        json["italic"] = italic;
        json["bold"] = bold;
        json["underline"] = underline;

        return json;
    }

    static SymbolFormat fromJson(QJsonObject json) {
        SymbolFormat format;
        format.italic = json["italic"].toBool();
        format.bold = json["bold"].toBool();
        format.underline = json["underline"].toBool();
        qDebug() << "from json" << format.italic << format.bold << format.underline;
        return format;
    }

    QTextCharFormat getQTextCharFormat() const {
        QTextCharFormat format;
        qDebug() << "Format class" << italic << bold << underline;
        QFont font;
        font.setItalic(italic);
        font.setBold(bold);
        font.setUnderline(underline);
        format.setFont(font);
        qDebug() << "Format class2" << format.font().italic() << format.font().bold() << format.font().underline();

        //            charFormat.setFont(QFont("Times", 15, QFont::Bold));
    //        charFormat.setFontWeight(QFont::Bold);
    //        charFormat.setForeground(QBrush(QColor(0xff0000)));
        return format;
    }
};

class Symbol
{
private:
    char value;
    QVector<Identifier> position;
    int counter; // TODO: when/where is it used???
    SymbolFormat format;

public:
    Symbol() {} // empty constructor needed, otherwise compile error
    Symbol(char value, QVector<Identifier> position, int counter) : value(value), position(position), counter(counter) {}
    Symbol(char value, QVector<Identifier> position, int counter, QFont font) : value(value),
            position(position), counter(counter) {
        format.italic = font.italic();
        format.bold = font.bold();
        format.underline = font.underline();

        qDebug() << "constructor" << format.italic << format.bold << format.underline;
    }
    Symbol(char value, QVector<Identifier> position, int counter, SymbolFormat format) : value(value),
            position(position), counter(counter), format(format) {}
    char getValue() const { return value; }
    QVector<Identifier> getPosition() const { return position; }
    int getCounter() const { return counter; }

    QTextCharFormat getQTextCharFormat() const { return format.getQTextCharFormat(); }

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
        json["format"] = format.toJson();

        return json;
    }

    static Symbol fromJson(QJsonObject json) {
        char value = json["value"].toString().at(0).toLatin1();
        int counter = json["counter"].toInt();

        QVector<Identifier> position;
        QJsonArray positionJson = json["position"].toArray();
        for (int i = 0; i < positionJson.size(); i++) {
            QJsonObject identifier = positionJson[i].toObject();
            int digit = identifier["digit"].toInt();
            int site = identifier["site"].toInt();
            position.push_back(Identifier(digit, site));
        }

        SymbolFormat format = SymbolFormat::fromJson(json["format"].toObject());
        Symbol s(value, position, counter, format);

        return s;
    }
};

#endif // SYMBOL_H

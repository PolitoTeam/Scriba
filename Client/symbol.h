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
    enum Alignment {ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER} align;
    bool italic, bold, underline;
    QString font;
    int size;
    QString color;

    QJsonObject toJson() {
        QJsonObject json;
        json["italic"] = italic;
        json["bold"] = bold;
        json["underline"] = underline;
        json["font"] = font;
        json["size"] = size;
        json["color"] = color;
        json["alignment"] = align;
        return json;
    }

    static SymbolFormat fromJson(QJsonObject json) {
        SymbolFormat format;
        format.italic = json["italic"].toBool();
        format.bold = json["bold"].toBool();
        format.underline = json["underline"].toBool();
        format.font = json["font"].toString();
        format.size = json["size"].toInt();
        format.color = json["color"].toString();
        format.align = static_cast<Alignment>(json["alignment"].toInt());
//        qDebug() << "from json" << format.italic << format.bold << format.underline;
        return format;
    }

    QTextCharFormat getQTextCharFormat() const {
        QTextCharFormat format;
//        qDebug() << "Format class" << italic << bold << underline;
        QFont font;
        font.setItalic(this->italic);
        font.setBold(this->bold);
        font.setUnderline(this->underline);
        font.setFamily(this->font);
        font.setPointSize(this->size);
        format.setFont(font);
        format.setForeground(QColor(color));
//        qDebug() << "Format class2" << format.font().italic() << format.font().bold() << format.font().underline();

        //            charFormat.setFont(QFont("Times", 15, QFont::Bold));
    //        charFormat.setFontWeight(QFont::Bold);
    //        charFormat.setForeground(QBrush(QColor(0xff0000)));
        return format;
    }
};

class Symbol
{
private:
    ushort value;
    QVector<Identifier> position;
    int counter; // TODO: when/where is it used???
    SymbolFormat format;
    int username;
public:
    Symbol() {} // empty constructor needed, otherwise compile error
    Symbol(ushort value, QVector<Identifier> position, int counter) : value(value), position(position), counter(counter) {}
    Symbol(ushort value, QVector<Identifier> position, int counter, QFont font, QColor color) : value(value),
            position(position), counter(counter) {
        format.italic = font.italic();
        format.bold = font.bold();
        format.underline = font.underline();
        format.size = font.pointSize();
        format.font = font.family();
        format.color = color.name();

//        qDebug() << "constructor" << format.italic << format.bold << format.underline;
    }
    Symbol(ushort value, QVector<Identifier> position, int counter, SymbolFormat format) : value(value),
            position(position), counter(counter), format(format) {}
    ushort getValue() const { return value; }
    QVector<Identifier> getPosition() const { return position; }
    int getCounter() const { return counter; }

    void setFormat(QFont font, QColor color) {
        format.italic = font.italic();
        format.bold = font.bold();
        format.underline = font.underline();
        format.size = font.pointSize();
        format.font = font.family();
        format.color = color.name();
    }

    void setAlignment(SymbolFormat::Alignment a) {
        format.align = a;
    }

    void setUsername(int editor_id){
        this->username=editor_id;
    }

    int getUsername(){
        return this->username;
    }

    SymbolFormat::Alignment getAlignment() const {
        return format.align;
    }

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
        QString value_string;
        if (value == '\n')
            value_string = "NL";
        else if (value == '\0')
            value_string = "NULL";
        else
            value_string = QString(1, value);

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
        json["username"] = username;

        return json;
    }

    static Symbol fromJson(QJsonObject json) {
        ushort value = json["value"].toString().at(0).unicode();
        int counter = json["counter"].toInt();
        int username = json["username"].toInt();

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
        s.setUsername(username);

        return s;
    }
};

#endif // SYMBOL_H

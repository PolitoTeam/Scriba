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

	friend QDataStream &operator<<(QDataStream& out,
								   const Identifier& id) {
		out << id.digit << id.site;
		return out;
	}

	friend QDataStream &operator>>(QDataStream& in,
								   Identifier& id) {
		id = Identifier();
		in >> id.digit >> id.site;
		return in;
	}
};

class SymbolFormat {
public:
	enum Alignment {ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER} align;
	bool italic, bold, underline;
	QString font;
	int size;
	QString color;

	SymbolFormat() {}

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
		return format;
	}

	QTextCharFormat getQTextCharFormat() const {
		QTextCharFormat format;
		QFont font;
		font.setItalic(this->italic);
		font.setBold(this->bold);
		font.setUnderline(this->underline);
		font.setFamily(this->font);
		font.setPointSize(this->size);
		format.setFont(font);
		format.setForeground(QColor(color));
		return format;
	}

	// Function to serialize any enum into QDataStream as a qint64
	template<typename Enum,
			 typename = typename std::enable_if<std::is_enum<Enum>::value>::type>
	friend QDataStream& operator<<(QDataStream& stream, const Enum& e) {
		stream << static_cast<qint64>(e);
		return stream;
	}

	// Function to deserialize any enum from QDataStream as a qint64
	template<typename Enum,
			 typename = typename std::enable_if<std::is_enum<Enum>::value>::type>
	friend QDataStream& operator>>(QDataStream& stream, Enum& e) {
		qint64 v;
		stream >> v;
		e = static_cast<Enum>(v);
		return stream;
	}

	friend QDataStream &operator<<(QDataStream& out,
								   const SymbolFormat& format) {
		out << format.align;
		out << format.italic << format.bold << format.underline;
		out << format.font << format.size << format.color;
		return out;
	}

	friend QDataStream &operator>>(QDataStream& in,
								   SymbolFormat& format) {
		format = SymbolFormat();
		in >> format.align;
		in >> format.italic >> format.bold >> format.underline;
		in >> format.font >> format.size >> format.color;
		return in;
	}
};

class Symbol
{
private:
	ushort value;
	QVector<Identifier> position;
	int counter; // TODO: when/where is it used???
	SymbolFormat format;

public:
	Symbol() {} // Empty constructor needed, otherwise compile error
	Symbol(ushort value, QVector<Identifier> position, int counter)
		: value(value), position(position), counter(counter) {}
	Symbol(ushort value, QVector<Identifier> position, int counter, QFont font,
		   QColor color) : value(value),
		position(position), counter(counter) {
		format.italic = font.italic();
		format.bold = font.bold();
		format.underline = font.underline();
		format.size = font.pointSize();
		format.font = font.family();
		format.color = color.name();
	}

	Symbol(ushort value, QVector<Identifier> position, int counter,
		   SymbolFormat format) : value(value), position(position),
								  counter(counter), format(format) {}

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

	int getUsername(){
		return this->position[this->position.size()-1].site;
	}

	SymbolFormat::Alignment getAlignment() const {
		return format.align;
	}

	QTextCharFormat getQTextCharFormat() const {
		return format.getQTextCharFormat();
	}

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

		return json;
	}

	static Symbol fromJson(QJsonObject json) {
		ushort value = json["value"].toString().at(0).unicode();
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

	friend QDataStream &operator<<(QDataStream& out,
								   const Symbol& symbol) {
		out << symbol.value << symbol.position
			<< symbol.counter << symbol.format;
		return out;
	}

	friend QDataStream &operator>>(QDataStream& in,
								   Symbol& symbol) {
		symbol = Symbol();
		in >> symbol.value >> symbol.position
			>> symbol.counter >> symbol.format;
		return in;
	}
};

Q_DECLARE_METATYPE(Symbol)

#endif // SYMBOL_H

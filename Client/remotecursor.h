#ifndef REMOTECURSOR_H
#define REMOTECURSOR_H

#include <QTextCursor>

class RemoteCursor
{
public:
	RemoteCursor(QTextCursor cursor, QTextBlock initial_block, int initial_index, QColor color);
	RemoteCursor(); // necessary to use QMap
	~RemoteCursor();
	void moveTo(QTextBlock block, int index);
	int getPosition();
	int getPosition(int& line, int& index);

	QTextCursor getCursor(){return remoteCursor;};
	QColor getColor(){return color;};

private:
	QString cursorHtml;
	QTextCursor remoteCursor;
	QColor color;
};

#endif // REMOTECURSOR_H


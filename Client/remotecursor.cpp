#include <QTextDocument>
#include <QDebug>
#include <QTextBlock>
#include "remotecursor.h"

RemoteCursor::RemoteCursor(QTextCursor cursor, QTextBlock initial_block,
						   int initial_index, const QColor color)
{
	this->remoteCursor = cursor;
	this->remoteCursor.setPosition(initial_block.position() + initial_index);
	this->color = color;
}

RemoteCursor::RemoteCursor() {} // Necessary to use QMap

RemoteCursor::~RemoteCursor() {}

void RemoteCursor::moveTo(QTextBlock block, int index) {
	this->remoteCursor.setPosition(block.position() + index);
}

// Get absolute position
int RemoteCursor::getPosition()
{
	return remoteCursor.position();
}

// Get position in terms of line and index
int RemoteCursor::getPosition(int& line, int& index) {
	line = this->remoteCursor.blockNumber();
	index = this->remoteCursor.positionInBlock();
}



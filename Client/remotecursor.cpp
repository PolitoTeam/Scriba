#include "remotecursor.h"
#include <QTextDocument>
#include <QDebug>

RemoteCursor::RemoteCursor(QTextDocument *document, int initial_position, const QString& color)
{
    this->cursorHtml = "<span style='background-color:" + color + ";display:block'>&nbsp;</span>";

    this->remoteCursor = new QTextCursor(document);
    this->remoteCursor->setPosition(initial_position);
    this->remoteCursor->insertHtml(cursorHtml);
}

RemoteCursor::~RemoteCursor() {
    delete remoteCursor;
}

void RemoteCursor::moveTo(int position) {
    if (position <= 0 || position >= remoteCursor->document()->characterCount()) {
        return;
    }

    int oldPosition = remoteCursor->position();
    // set position before remote cursor and delete it
    remoteCursor->setPosition(oldPosition - 1);
    remoteCursor->deleteChar();

    // put in new position
    remoteCursor->setPosition(position - 1);
    remoteCursor->insertHtml(cursorHtml);
}

int RemoteCursor::getPosition()
{
    return remoteCursor->position();
}

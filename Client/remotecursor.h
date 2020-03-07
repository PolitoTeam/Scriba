#ifndef REMOTECURSOR_H
#define REMOTECURSOR_H

#include <QTextCursor>

class RemoteCursor
{
public:
    RemoteCursor(QTextCursor cursor, QTextBlock initial_block, int initial_index, const QString& color);
    RemoteCursor(); // necessary to use QMap
    ~RemoteCursor();
    void moveTo(QTextBlock block, int index);
    int getPosition();
    int getPosition(int& line, int& index);

private:
    QString cursorHtml;
    QTextCursor remoteCursor;
};

#endif // REMOTECURSOR_H


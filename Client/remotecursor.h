#ifndef REMOTECURSOR_H
#define REMOTECURSOR_H

#include <QTextCursor>

class RemoteCursor
{
public:
    RemoteCursor(QTextDocument *document, int initial_position, const QString& color);
    ~RemoteCursor();
    void moveTo(int position);
    int getPosition();

private:
    QString cursorHtml;
    QTextCursor *remoteCursor;
};

#endif // REMOTECURSOR_H


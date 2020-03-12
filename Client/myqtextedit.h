#ifndef MYQTEXTEDIT_H
#define MYQTEXTEDIT_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QPainter>
#include "remotecursor.h"

class MyQTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MyQTextEdit(QWidget *parent = nullptr);
    QMap<int, RemoteCursor*> remote_cursors;
private:
    QTextCursor cursor;
    // <editorid, cursor>


protected:
    void paintEvent(QPaintEvent* event);
signals:

public slots:

};



#endif // MYQTEXTEDIT_H

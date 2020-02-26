#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include "appMainWindow.h"
#include "client.h"
#include "CRDT.h"
#include <QTextCursor>

namespace Ui {
class Editor;
}

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    Editor(QWidget *parent = nullptr,Client* client=0);
    void setClient(Client* client);
    ~Editor();
    void clear();

//    void setCRDT(CRDT *crdt);

signals:
    void changeWidget(int);

private slots:
    void print();
    void exit();
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void selectFont();
    void setFontBold(bool bold);
    void setFontUnderline(bool underline);
    void setFontItalic(bool italic);
    void sharedLink();

    void textChange();

    void on_contentsChange(int position, int charsRemoved, int charsAdded);
    void on_insert(int line, int index, const Symbol& s);
    void on_erase(int line, int index);
    void addUsers(const QList<QPair<QString,QString>> users);
    void updateText(const QString& text);
    void removeUser(const QString& name);
    void saveCursorPosition();
    void on_currentCharFormatChanged(const QTextCharFormat &format);

    void textColor();


private:
    Ui::Editor *ui;
    Client* client;
    CRDT *crdt;
    int line = 0, index = 0;

    void showEvent(QShowEvent* event);
    int fromStringToIntegerHash(QString str);
};

#endif // EDITOR_H

#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include "appMainWindow.h"
#include "client.h"
#include "CRDT.h"

namespace Ui {
class Editor;
}

class Editor : public QWidget
{
    Q_OBJECT

public:
    Editor(QWidget *parent = nullptr,Client* client=0);
    void setClient(Client* client);
    ~Editor();
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
    void on_contentsChange(int position, int charsRemoved, int charsAdded);
    void on_insert(int index, char value);

private:
    Ui::Editor *ui;
    Client* client;
    CRDT *crdt;
};

#endif // EDITOR_H

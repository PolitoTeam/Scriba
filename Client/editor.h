#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include "appMainWindow.h"
#include "client.h"
#include "CRDT.h"
#include "highlighter.h"
#include <QTextCursor>
#include <QtWidgets>

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
    Qt::Alignment getCurrentAlignment();
    bool undoFlag;
    bool redoFlag;
    SymbolFormat::Alignment alignmentConversion(Qt::Alignment a);
    Qt::Alignment alignmentConversion(SymbolFormat::Alignment a);
    void peerYou();

//    void setCRDT(CRDT *crdt);


signals:
    void changeWidget(int);
//    void formatChange();

private slots:
    void printPdf();
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
    void textAlign(QAction *a);
    void sharedLink();

    void textChange();

    void on_contentsChange(int position, int charsRemoved, int charsAdded);
    void on_insert(int line, int index, const Symbol& s);
    void on_insertGroup(int line, int index, const QString& s,QTextCharFormat newFormat);
    void on_changeAlignment(int align,int line, int index);
    void on_erase(int line, int index);
    void on_change(int line, int index, const Symbol& s);
    void addUsers(const QList<QPair<QPair<QString,QString>,QPixmap>> users);
    void updateText(const QString& text);
    void removeUser(const QString& username,const QString& nickname);
    void saveCursorPosition();
    void on_currentCharFormatChanged(const QTextCharFormat &format);

    void textColor();
    void clipboardDataChanged();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void moveCursorToEnd();
    void on_addCRDTterminator();
    void on_remoteCursor(int editor_id, Symbol s);
   // void correct_position(int& line, int& index);

private:
    Ui::Editor *ui;
    Client* client;
    CRDT *crdt;
    int line = 0, index = 0;
    Highlighter *highlighter;
    QMap<QString,int> username_row; //map username - row in the list widget (peers)




    QFontComboBox *comboFont;
    QComboBox *comboSize;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionShowAssigned;

    void showEvent(QShowEvent* event);
    int fromStringToIntegerHash(QString str);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);
    void on_formatChange();
    void on_formatChange(QTextCursor cursor);
    void on_showAssigned();
    void closeEvent (QCloseEvent *event);
};

#endif // EDITOR_H

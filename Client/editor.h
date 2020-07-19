#ifndef EDITOR_H
#define EDITOR_H

#include "CRDT.h"
#include "appMainWindow.h"
#include "clickablelabel.h"
#include "client.h"
#include "highlighter.h"
#include <QMainWindow>
#include <QTextCursor>
#include <QtWidgets>

namespace Ui {
class Editor;
}

class Editor : public QMainWindow {
  Q_OBJECT

public:
  Editor(QWidget *parent = nullptr, Client *client = 0);
  ~Editor();
  void clear(bool serverDisconnected);
  Qt::Alignment getCurrentAlignment();
  bool undoFlag;
  bool redoFlag;
  SymbolFormat::Alignment alignmentConversion(Qt::Alignment a);
  Qt::Alignment alignmentConversion(SymbolFormat::Alignment a);
  void peerYou();
  void clearUndoRedoStack();

signals:
  void changeWidget(int);

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
  void on_insert(int line, int index, const Symbol &s);
  void on_insertGroup(int line, int index, const QString &s,
                      QTextCharFormat newFormat);
  void on_changeAlignment(int align, int line, int index);
  void on_erase(int startLine, int startIndex, int lenght);
  void on_change(const QVector<Symbol> &symbols);
  void addUsers(const QList<QPair<QPair<QString, QString>, QPixmap>> users);
  void updateText(const QString &text);
  void removeUser(const QString &username, const QString &nickname);
  void saveCursorPosition();
  void on_currentCharFormatChanged(const QTextCharFormat &format);
  void textColor();
  void clipboardDataChanged();
  void textFamily(const QString &f);
  void textSize(const QString &p);
  void moveCursorToEnd();
  void on_addCRDTterminator();
  void on_remoteCursor(int editor_id, Symbol s);
  bool checkAlignment(int position);

private:
  Ui::Editor *ui;
  Client *client;
  CRDT *crdt;
  int line = 0, index = 0;
  Highlighter *highlighter;
  QMessageBox *popUp;
  bool alignment = false;

  QFontComboBox *comboFont;
  QComboBox *comboSize;
  QAction *actionTextColor;
  QAction *actionAlignLeft;
  QAction *actionAlignCenter;
  QAction *actionAlignRight;
  QAction *actionShowAssigned;

  void showEvent(QShowEvent *event);
  int fromStringToIntegerHash(QString str);
  void fontChanged(const QFont &f);
  void colorChanged(const QColor &c);
  void alignmentChanged(Qt::Alignment a);
  void on_formatChange(const QString &changed, int start, int end);
  void on_formatChange();
  void on_formatChange(QTextCursor cursor);
  QPixmap addImageInPeerBar(const QPixmap &orig, QColor color);
  void on_showAssigned();
  void closeEvent(QCloseEvent *event);
  void handleLocalInsertion(int position, int num_chars);
};

#endif // EDITOR_H

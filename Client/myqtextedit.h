#ifndef MYQTEXTEDIT_H
#define MYQTEXTEDIT_H

#include "remotecursor.h"
#include <QObject>
#include <QPainter>
#include <QTextEdit>
#include <QWidget>

class MyQTextEdit : public QTextEdit {
  Q_OBJECT
public:
  explicit MyQTextEdit(QWidget *parent = nullptr);
  QMap<int, RemoteCursor *> remote_cursors;
  void keyPressEvent(QKeyEvent *e) override;
  void insertFromMimeData(const QMimeData *source) override;
  void setLine(int *line);
  void setIndex(int *index);
  bool getSelected();
  bool getInserted();
  void setInserted(bool inserted);

public slots:
  void paste();



private:
  QTextCursor cursor;
  int *line, *index;
  bool selected;
  bool inserted=false;

protected:
  void paintEvent(QPaintEvent *event) override;


signals:
  void undo();
  void redo();
  void resetDefaultAlignment(bool a);
  void firstLineAlignmentOnPaste(QString alignment);
};

#endif // MYQTEXTEDIT_H

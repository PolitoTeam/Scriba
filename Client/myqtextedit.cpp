#include "myqtextedit.h"
#include <QDebug>
#include <QMimeData>
#include <QPaintEvent>
#include <QRegularExpression>
#include <QTextBlock>
#include <QtWidgets>
#include <math.h>
#include <qabstracttextdocumentlayout.h>

MyQTextEdit::MyQTextEdit(QWidget *parent) : QTextEdit(parent) {
  this->setFont(QFont("American Typewriter", 15));
  selected = false;
}

void MyQTextEdit::paintEvent(QPaintEvent *e) {

  viewport()->update();
  QPainter p(viewport());
  selected = !this->textCursor().selection().isEmpty();

  for (RemoteCursor *cursor : remote_cursors) {
    QColor c = cursor->getColor();
    QPen pen;
    QBrush pincel(c);
    pen.setColor(c);
    p.setPen(pen);
    p.setBrush(pincel);

    QRect r = this->cursorRect(cursor->getCursor());

    p.drawRect(r);
  }
  QTextEdit::paintEvent(e);
}

void MyQTextEdit::keyPressEvent(QKeyEvent *e) {

  if (e->modifiers().testFlag(Qt::NoModifier) && e->key() != 16777219)
    inserted = true;

  if (e->key() == Qt::Key_Z && e->modifiers().testFlag(Qt::ControlModifier)) {
    if (this->document()->isUndoAvailable()) {

      emit undo();
    }
  } else if (e->key() == Qt::Key_Y &&
             e->modifiers().testFlag(Qt::ControlModifier)) {
    if (this->document()->isRedoAvailable()) {
      qDebug() << "redo";
      emit redo();
    }
    // 16777219 = code of delete key
  } else if (e->key() == 16777219 && this->document()->isEmpty()) {
    emit resetDefaultAlignment(true);
  } else {
    QTextEdit::keyPressEvent(e);
  }
}

void MyQTextEdit::paste() {
  inserted = true;
  QTextEdit::paste();
}

void MyQTextEdit::insertFromMimeData(const QMimeData *source) {
  QTextCursor temp = this->textCursor();

  QRegularExpression re("(?<=<body>\\n<p align=\\\")(right)|(center)(?=\\\")");
  QRegularExpressionMatch match = re.match(source->html());

  if (match.hasMatch()) {
    QString alignment = match.captured(0);
    // qDebug()<<"alignment first line: "<<alignment;

    QTextBlockFormat newformat;

    if (*index == 0) {
      if (alignment == "right") {
        newformat.setAlignment(Qt::AlignTrailing | Qt::AlignAbsolute);
      } else if (alignment == "center") {
        newformat.setAlignment(Qt::AlignHCenter);
      } else {
        newformat.setAlignment(Qt::AlignLeft | Qt::AlignLeading);
      }

      QTextBlock block = this->document()->findBlockByLineNumber(*line);
      temp.setPosition(block.position() + *index);
      temp.mergeBlockFormat(newformat);
    }
  }
  pasted = source->text().length();
  this->QTextEdit::insertFromMimeData(source);
}

void MyQTextEdit::setLine(int *line) { this->line = line; }

void MyQTextEdit::setIndex(int *index) { this->index = index; }

bool MyQTextEdit::getSelected() { return selected; }

bool MyQTextEdit::getInserted() { return inserted; }
void MyQTextEdit::setInserted(bool inserted) { this->inserted = inserted; }
int MyQTextEdit::getPasted() { return pasted; }
void MyQTextEdit::setPasted(int pasted) { this->pasted = pasted; }

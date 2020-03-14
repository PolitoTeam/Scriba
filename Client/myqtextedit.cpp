#include "myqtextedit.h"
#include <QPaintEvent>
#include <QDebug>
#include <qabstracttextdocumentlayout.h>

MyQTextEdit::MyQTextEdit(QWidget *parent) : QTextEdit(parent)
{

}

void MyQTextEdit::paintEvent(QPaintEvent *e)
{

      QPainter p(viewport());
     // qDebug()<<"------------------PAINTEVENT------------------";
/*
      QAbstractTextDocumentLayout::Selection selection;
      selection.cursor = textCursor();
      selection.format = currentCharFormat();

      QAbstractTextDocumentLayout::PaintContext ctx;
      ctx.cursorPosition = textCursor().position();
      ctx.selections.append(selection);

      document()->documentLayout()->draw(&p,ctx);

*/
     // qDebug()<< "Num cursor: "<<remote_cursors.size();
      for (RemoteCursor *cursor :remote_cursors) {
         QColor c  = cursor->getColor();
         QPen pen;
         QBrush pincel(c);
         pen.setColor(c);
         p.setPen(pen);
         p.setBrush(pincel);
         QRect r = this->cursorRect(cursor->getCursor());
         p.drawRect(r);

      }

      this->QTextEdit::paintEvent(e);




}


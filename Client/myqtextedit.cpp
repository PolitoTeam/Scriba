#include "myqtextedit.h"
#include <QPaintEvent>
#include <QDebug>
#include <qabstracttextdocumentlayout.h>
#include <QMimeData>
#include <QRegularExpression>
#include <QTextBlock>


MyQTextEdit::MyQTextEdit(QWidget *parent) : QTextEdit(parent)
{
	this->setFont(QFont("American Typewriter"));

}

void MyQTextEdit::paintEvent(QPaintEvent *e)
{

	QPainter p(viewport());
	// //qDebug()<<"------------------PAINTEVENT------------------";
	/*
	  QAbstractTextDocumentLayout::Selection selection;
	  selection.cursor = textCursor();
	  selection.format = currentCharFormat();

	  QAbstractTextDocumentLayout::PaintContext ctx;
	  ctx.cursorPosition = textCursor().position();
	  ctx.selections.append(selection);

	  document()->documentLayout()->draw(&p,ctx);

*/
	// //qDebug()<< "Num cursor: "<<remote_cursors.size();
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


void MyQTextEdit::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Z && e->modifiers().testFlag(Qt::ControlModifier))
	{

		if(this->document()->isUndoAvailable()){
			//qDebug()<<"received undoo";
			emit undo();
		}
	}
	else if (e->key() == Qt::Key_Y && e->modifiers().testFlag(Qt::ControlModifier)){
		if(this->document()->isRedoAvailable()){

			emit redo();
		}
	}
	//delete
	else if (e->key()==16777219 && this->document()->isEmpty()){
		//qDebug()<<"empty";
		emit resetDefaultAlignment(true);
	}
	else{
		QTextEdit::keyPressEvent(e);
	}
	//qDebug()<<"KEY: "<<e->key();


}

void MyQTextEdit::insertFromMimeData(const QMimeData *source) {

	QTextCursor temp = this->textCursor();

	QRegularExpression re("(?<=<body>\\n<p align=\\\")(right)|(center)(?=\\\")");
	QRegularExpressionMatch match = re.match(source->html());

	if(match.hasMatch()){
		QString alignment = match.captured(0);
        //qDebug()<<"alignment first line: "<<alignment;

		QTextBlockFormat newformat;
        //qDebug()<<"inDEX ON PASTE: "<<*index;

		if (*index==0){
			if (alignment=="right"){
                //qDebug()<<"qui right";
				newformat.setAlignment(Qt::AlignTrailing|Qt::AlignAbsolute);
			}
			else if(alignment=="center")
				newformat.setAlignment(Qt::AlignHCenter);
			else {
				newformat.setAlignment(Qt::AlignLeft|Qt::AlignLeading);
			}
            //qDebug()<<"alignment: "<<alignment;

			QTextBlock block = this->document()->findBlockByLineNumber(*line);
			temp.setPosition(block.position() + *index);
            //qDebug()<<"Get Position: "<<temp.position();
			temp.mergeBlockFormat(newformat);
		}
	}

	this->QTextEdit::insertFromMimeData(source);



}

void MyQTextEdit::setLine(int* line){
	this->line=line;
}

void MyQTextEdit::setIndex(int* index){
	this->index=index;
}


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
	void keyPressEvent(QKeyEvent *e) override;
	void insertFromMimeData(const QMimeData *source) override;
	void setLine(int* line);
	void setIndex(int* index);
private:
	QTextCursor cursor;
	int* line,*index;
	// <editorid, cursor>


protected:
	void paintEvent(QPaintEvent* event) override;
signals:
	void undo();
	void redo();
	void resetDefaultAlignment(bool a);
	void firstLineAlignmentOnPaste(QString alignment);


public slots:

};



#endif // MYQTEXTEDIT_H

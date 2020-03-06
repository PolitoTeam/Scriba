#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include "CRDT.h"
#include "symbol.h"

class Highlighter: public QSyntaxHighlighter
{
     Q_OBJECT
public:
   Highlighter(QTextDocument *document=0,CRDT* crdt=0);
   void addClient(QString username,QColor color);


private:
   void highlightBlock(const QString &text) override;
   QMap<QString,QColor> users;
   CRDT *crdt;

};

#endif // HIGHLIGHTER_H

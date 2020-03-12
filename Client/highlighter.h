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
   void addClient(int editor_id,QColor color);
   QColor getColor(int editor_id);


private:
   void highlightBlock(const QString &text) override;
   QMap<int,QColor> users;
   CRDT *crdt;

};

#endif // HIGHLIGHTER_H

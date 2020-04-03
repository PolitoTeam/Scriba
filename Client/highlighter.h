#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include "CRDT.h"
#include "symbol.h"
#include "colors.h"

class Highlighter: public QSyntaxHighlighter
{
     Q_OBJECT
public:
   Highlighter(QTextDocument *document=0,CRDT* crdt=0);
   bool addClient(int editor_id);
   void addLocal(int editor_id);
   QColor getColor(int editor_id);
   void freeColor(int editor_id);
   void freeAll();


private:
   void highlightBlock(const QString &text) override;
   QMap<int,int> users;
   CRDT *crdt;
   Colors list_colors;

};

#endif // HIGHLIGHTER_H

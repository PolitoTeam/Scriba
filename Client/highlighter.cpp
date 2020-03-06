#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *document,CRDT* crdt)
    : QSyntaxHighlighter(document),crdt(crdt){}

void Highlighter::addClient(QString username,QColor color){

    if (this->users.contains(username))
        return;
    qDebug()<<"Insert: "<<username<<" with color: "<<color;
    users.insert(username,color);
}

void Highlighter::highlightBlock(const QString &text){
    qDebug()<<" in highlight block";
    // save cursor position
    int line = this->currentBlock().blockNumber();
    qDebug()<<" Line: "<<line;
    for (int index = 0; index < text.length(); index++) {
            qDebug()<<"Retrieving simbol at: "<<line<<" ,"<<index;
            Symbol s = this->crdt->getSymbol(line,index);
            qDebug()<<"Highilithing value: "<<s.getValue();
            QString username = s.getUsername();
            QTextCharFormat format = s.getQTextCharFormat();

            QColor color = users.value(username);

            format.setBackground(QBrush(color,Qt::SolidPattern));
            setFormat(index,1,format);
            }
}


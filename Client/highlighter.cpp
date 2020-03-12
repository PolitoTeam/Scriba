#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *document,CRDT* crdt)
    : QSyntaxHighlighter(document),crdt(crdt){


}

void Highlighter::addClient(int editor_id,QColor color){

    if (this->users.contains(editor_id))
        return;
    qDebug()<<"Insert: "<<editor_id<<" with color: "<<color;
    users.insert(editor_id,color);
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
            int editor_id = s.getUsername();
            QTextCharFormat format = s.getQTextCharFormat();

            QColor color = users.value(editor_id);

            format.setBackground(QBrush(color,Qt::SolidPattern));
            setFormat(index,1,format);
            }
}

QColor Highlighter::getColor(int editor_id){
    if (users.contains(editor_id))
        return users[editor_id];
    //da gestire
    return QColor('white');
}


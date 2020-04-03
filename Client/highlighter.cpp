#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *document,CRDT* crdt)
    : QSyntaxHighlighter(document),crdt(crdt){
     list_colors = Colors();
}

bool Highlighter::addClient(int editor_id){

    if (this->users.contains(editor_id))
        return false;

    int i = list_colors.getIndex();
    //qDebug()<<i;
    users.insert(editor_id,i);
    return true;
}

void Highlighter::addLocal(int editor_id){
    if (this->users.contains(editor_id))
        return;
    users.insert(editor_id,-1);
}

void Highlighter::highlightBlock(const QString &text){
    //qDebug()<<" in highlight block";
    // save cursor position
    int line = this->currentBlock().blockNumber();
    //qDebug()<<" Line: "<<line;
    for (int index = 0; index < text.length(); index++) {
            //qDebug()<<"Retrieving simbol at: "<<line<<" ,"<<index;
            Symbol s = this->crdt->getSymbol(line,index);

            int editor_id = s.getUsername();
            QTextCharFormat format = s.getQTextCharFormat();

            QColor color = list_colors.getColor(users.value(editor_id));

            format.setBackground(QBrush(color,Qt::SolidPattern));
            setFormat(index,1,format);
            }
}

QColor Highlighter::getColor(int editor_id){
    if (users.contains(editor_id))
        return list_colors.getColor(users[editor_id]);
    //da gestire
    return QColor('white');
}

void Highlighter::freeColor(int editor_id){
     list_colors.freeColor(users.value(editor_id));
     if(users.remove(editor_id)!=0) //return 0 if editor_id not in the map
         qDebug()<<"removed";
}

void Highlighter::freeAll(){
    list_colors.clear();
    users.clear();
}


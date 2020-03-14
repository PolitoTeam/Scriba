#include "CRDT.h"
#include <QFont>

CRDT::CRDT(Client *client) : client(client) {
    connect(client, &Client::remoteInsert, this, &CRDT::handleRemoteInsert);
    connect(client, &Client::remotePaste, this, &CRDT::handleRemotePaste);
    connect(client, &Client::remoteErase, this, &CRDT::handleRemoteErase);
    connect(client, &Client::remoteChange, this, &CRDT::handleRemoteChange);
    connect(client, &Client::remoteAlignChange, this, &CRDT::handleRemoteAlignChange);

    _symbols.push_back(QVector<Symbol>{});
    // terminator ('\0') should not be included in the count
    this->size = -1;
}

int CRDT::getId() { return _siteId; }
void CRDT::setId(int site){this->_siteId = site;}

void CRDT::localInsert(int line, int index, ushort value, QFont font, QColor color,Qt::Alignment align) {
    if (line < 0 || index < 0)
        throw std::runtime_error("Error: index out of bound.\n");

    // calculate position
    QVector<Identifier> posBefore = findPosBefore(line, index);
    QVector<Identifier> posAfter = findPosAfter(line, index);
    QVector<Identifier> newPos;
    QVector<Identifier> position = generatePositionBetween(posBefore, posAfter, newPos);

    // generate symbol
    Symbol s(value, newPos, ++_counter, font, color);
    s.setUsername(this->_siteId);

    if (s.getValue()=='\0' || s.getValue()=='\n'){

         qDebug()<<"ALIGNMENT: "<<align;
        if (align==Qt::AlignLeft)
           s.setAlignment(SymbolFormat::Alignment::ALIGN_LEFT);
        else if (align==Qt::AlignHCenter)
           s.setAlignment(SymbolFormat::Alignment::ALIGN_CENTER);
        else if (align==Qt::AlignRight)
           s.setAlignment(SymbolFormat::Alignment::ALIGN_RIGHT);
    }

    insertChar(s, line, index);
    this->size++;

    // broadcast
    QJsonObject message;
    message["type"] = QStringLiteral("operation");
    message["editorId"] = _siteId;
    message["operation_type"] = INSERT;
    message["symbol"] = s.toJson();

    qDebug() << "LOCAL insert";
    qDebug().noquote() << to_string(); // very useful for debugging
    client->sendJson(message);
}

void CRDT::localInsertGroup(int& line, int& index, QString partial, QFont font, QColor color,Qt::Alignment align) {
    qDebug()<<"Trying to insert "<<partial <<" in ("<<line<<" , "<<index<<")";
    if (line < 0 || index < 0)
        throw std::runtime_error("Error: index out of bound.\n");
    QJsonArray symbols;
    for (int i =0; i<partial.length(); i++){
        // calculate position
        QVector<Identifier> posBefore = findPosBefore(line, index);
        QVector<Identifier> posAfter = findPosAfter(line, index);
        QVector<Identifier> newPos;
        QVector<Identifier> position = generatePositionBetween(posBefore, posAfter, newPos);

        // generate symbol
        Symbol s(partial.at(i).unicode(), newPos, ++_counter, font, color);
        s.setUsername(this->_siteId);

        if (s.getValue()=='\0' || s.getValue()=='\n'){
             qDebug()<<"ALIGNMENT: "<<align;
            if (align==(Qt::AlignLeft|Qt::AlignLeading))
               s.setAlignment(SymbolFormat::Alignment::ALIGN_LEFT);
            else if (align==Qt::AlignHCenter)
               s.setAlignment(SymbolFormat::Alignment::ALIGN_CENTER);
            else if (align==(Qt::AlignTrailing | Qt::AlignAbsolute))
               s.setAlignment(SymbolFormat::Alignment::ALIGN_RIGHT);
        }
        symbols.append(s.toJson());

        insertChar(s, line, index);
        this->size++;
        if (s.getValue()=='\n'){
            line+=1;
            index=0;
        }
        else {
            index+=1;
        }
    }



    // broadcast
    QJsonObject message;
    message["type"] = QStringLiteral("operation");
    message["editorId"] = _siteId;
    message["operation_type"] = PASTE;
    message["symbols"] = symbols;

    qDebug().noquote() << to_string(); // very useful for debugging
    client->sendJson(message);
}

void CRDT::localChangeAlignment(int line,SymbolFormat::Alignment align){

    Symbol s = _symbols[line][_symbols[line].size()-1];
    s.setAlignment(align);
    qDebug()<<" Alignment local: "<<align<<" Symbol: "<<s.getValue();

    // broadcast
    QJsonObject message;
    message["type"] = QStringLiteral("operation");
    message["editorId"] = _siteId;
    message["operation_type"] = ALIGN;
    message["symbol"] = s.toJson();

    qDebug().noquote() << to_string(); // very useful for debugging
    client->sendJson(message);
}

SymbolFormat::Alignment CRDT::getAlignmentLine(int line){
    return _symbols[line][_symbols[line].size()-1].getAlignment();
}

QVector<Identifier> CRDT::findPosBefore(int line, int index) {
    int newLine = line;
    int newIndex = index;

    if (index == 0 && line == 0)
        return QVector<Identifier>{};
    else if (index == 0 && line != 0) {
        newLine = line - 1;
        newIndex = _symbols[newLine].size();
    }
    return _symbols[newLine][newIndex - 1].getPosition();
}

QVector<Identifier> CRDT::findPosAfter(int line, int index) {
    int newLine = line;
    int newIndex = index;

    int numLines = _symbols.size();
    int numChars = line < _symbols.size() ? _symbols[line].size() : 0;

    if ((line == numLines - 1) && index == numChars)
        return QVector<Identifier>{};
    else if ((line < numLines - 1) && index == numChars) {
        newLine = line + 1;
        newIndex = 0;
    } else if ((line > numLines - 1) && index == 0) {
        return QVector<Identifier>{};
    }
    return _symbols[newLine][newIndex].getPosition();
}

QVector<Identifier> CRDT::generatePositionBetween(QVector<Identifier>& pos1, QVector<Identifier> pos2, QVector<Identifier>& newPos, int level) { // TODO: pass vectors by reference?
    Identifier id1 = level < pos1.size() ? pos1[level] : Identifier(0, this->_siteId);
    Identifier id2 = level < pos2.size() ? pos2[level] : Identifier(BASE, this->_siteId); // == BASE * std::pow(2, 0)

    if (id2.digit - id1.digit > 1) {
        // case 1: enough space to add in between
        int newDigit = generateIdBetween(id1.digit, id2.digit, level);
        newPos.push_back(Identifier(newDigit, this->_siteId));
        return newPos;
    } else if (id2.digit - id1.digit == 1) {
        // case 2: no space in between, use identifier of first position
        newPos.push_back(id1);
        return generatePositionBetween(pos1, QVector<Identifier> {}, newPos, level + 1);
    } else if (id1.digit == id2.digit) {
        // case 3: same digit, use site id to discriminate
        if (id1.site < id2.site) {
            newPos.push_back(id1);
            return generatePositionBetween(pos1, QVector<Identifier> {}, newPos, level + 1);
        } else if (id1.site == id2.site) {
            newPos.push_back(id1);
            return generatePositionBetween(pos1, pos2, newPos, level + 1);
        } else {
            throw std::runtime_error("Invalid ordering");
        }
    }
}

int CRDT::generateIdBetween(int id1, int id2, int level) {
    int interval = id2 - id1;

    if (strategyCache.find(level) == strategyCache.end()) {
        strategyCache[level] = generateRandomBool();
    }

    int step = std::min(BOUNDARY, interval);
    if (strategyCache[level]) { //boundary+
        int delta = generateRandomNumBetween(1, step - 1);
        return id1 + delta;
    }
    else{ //boundary-
        int delta = generateRandomNumBetween(1, step - 1);
        return id2 - delta;
    }
}

bool CRDT::generateRandomBool() {
    static auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    return gen();
}

// returns n1 <= x <= n2
int CRDT::generateRandomNumBetween(int n1,int n2) { // TODO: check if better to use uniform_int_distribution
    return n1 + (std::rand() % (n2 - n1 + 1));
}

void CRDT::localErase(int line, int index) {
    Symbol s = _symbols[line][index];
    bool newLineRemoved = (s.getValue() == '\n');
    _symbols[line].erase(_symbols[line].begin() + index);

    if (newLineRemoved && line + 1 < _symbols.size()) { // non-empty line after current line
        std::copy(_symbols[line + 1].begin(), _symbols[line + 1].end(), std::back_inserter(_symbols[line]));
        _symbols.erase(_symbols.begin() + line + 1);
    }

    this->size--;

    // broadcast
    QJsonObject message;
    message["type"] = QStringLiteral("operation");
    message["editorId"] = _siteId;
    message["operation_type"] = DELETE;
    message["symbol"] = s.toJson();

    qDebug().noquote() << to_string();
    client->sendJson(message);
}

void CRDT::localChange(int line, int index, QFont font, QColor color) {
    Symbol s = _symbols[line][index];

    // update font and color
    s.setFormat(font, color);
    _symbols[line][index] = s;

    // broadcast
    QJsonObject message;
    message["type"] = QStringLiteral("operation");
    message["editorId"] = _siteId;
    message["operation_type"] = CHANGE;
    message["symbol"] = s.toJson();

    client->sendJson(message);
}

void CRDT::cursorPositionChanged(int line, int index) {
    qDebug() << "curosr position changed" << line << index;
    Symbol s = _symbols[line][index];
    qDebug() << "symbol AFTER: " << QChar(s.getValue());

    // broadcast
    QJsonObject message;
    message["type"] = QStringLiteral("operation");
    message["editorId"] = _siteId;
    message["operation_type"] = CURSOR;
    message["symbol"] = s.toJson();

    client->sendJson(message);
}

int CRDT::getSize()
{
    return size;
}

bool CRDT::findPosition(Symbol s, int& line, int& index) {
    int minLine = 0;
    int totalLines = _symbols.size();
    int maxLine = totalLines - 1;
    QVector<Symbol> lastLine = _symbols[maxLine], currentLine, minCurrentLine, maxCurrentLine;
    int midLine;
    Symbol minLastChar, maxLastChar;

    if ((_symbols.size() == 1 && _symbols[0].size() == 0) || Symbol::compare(s, _symbols[0][0]) < 0)
        return false;

    Symbol lastChar = lastLine[lastLine.size() - 1];

    if (Symbol::compare(s, lastChar) > 0)
        return false;

    // binary search
    while (minLine + 1 < maxLine) {
        midLine = minLine + (maxLine - minLine) / 2;
        currentLine = _symbols[midLine];
        lastChar = currentLine[currentLine.size() - 1];

        if (Symbol::compare(s, lastChar) == 0) {
            line = midLine;
            index = currentLine.size() - 1;
            return true;
        } else if (Symbol::compare(s, lastChar) < 0) {
            maxLine = midLine;
        } else {
            minLine = midLine;
        }
    }

    minCurrentLine = _symbols[minLine];
    minLastChar = minCurrentLine[minCurrentLine.size() - 1];
    maxCurrentLine = _symbols[maxLine];
    maxLastChar = maxCurrentLine[maxCurrentLine.size() - 1];

    if (Symbol::compare(s, minLastChar) <= 0) {
        index = findIndexInLine(s, minCurrentLine);
        line = minLine;
        return true;
    } else {
        index = findIndexInLine(s, maxCurrentLine);
        line = maxLine;
        return true;
    }
}

int CRDT::findIndexInLine(Symbol s, QVector<Symbol> line) {
    int left = 0, right = line.size() - 1, mid, compareNum;

    if (line.size() == 0 || Symbol::compare(s, line[left]) < 0) {
        return left;
    } else if (Symbol::compare(s, line[right]) > 0) {
        return _symbols.size();
    }

    while (left + 1 < right) {
        mid = left + (right - left) / 2;
        compareNum = Symbol::compare(s, line[mid]);

        if (compareNum == 0) {
            return mid;
        } else if (compareNum > 0) {
            left = mid;
        } else {
            right = mid;
        }
    }

    if (Symbol::compare(s, line[left]) == 0) {
        return left;
    } else if (Symbol::compare(s, line[right]) == 0) {
        return right;
    } else {
        return false;
    }
}

QString CRDT::to_string(){
    QString str = "";
    for(QVector<Symbol> line: _symbols){
        bool first = true;
        for (Symbol s : line) {
            if (first) {
                first = false;
            } else {
                str += ", ";
            }
            str += s.to_string();
        }
        str += "\n";
    }
    return str;
}

void CRDT::handleRemoteAlignChange(const Symbol& s) {
    int line,index;
    SymbolFormat::Alignment align = s.getAlignment();
    qDebug()<<" GET ALIGNMENT: "<<align;
    bool res;
    if (!(res=findPosition(s,line,index))){
        qDebug()<<"---RES: "<<res;
        return;
    }


    //    qDebug() << "remote insert" << s.getValue() << line << index;
    // insert in editor
    emit changeAlignment(align,line, index);

}

void CRDT::handleRemotePaste(const QJsonArray& symbols){
    qDebug() << "REMOTE PASTE";
    QString partial;
    int firstLine, firstIndex;
    QTextCharFormat newFormat;
    QVector<Symbol> alignChanges;
    for (int i = 0; i < symbols.size(); i++) {
            QJsonObject symbol = symbols[i].toObject();
            Symbol s = Symbol::fromJson(symbol);
            int line, index;
            if (_symbols.size() != 0) {
                findInsertPosition(s, line, index);
            } else {
                line=0;
                index=0;
            }

            if (i==0){
                firstLine=line;
                firstIndex=index;
                newFormat=s.getQTextCharFormat();
            }

            // insert in crdt structure
            insertChar(s, line, index);

            this->size++;

            qDebug() << "remote insert" << s.getValue() << line << index;
            // insert in editor
        //    if (line==_symbols.size()-1 && index>0)
        //        index-=1;
            if (s.getValue()=='\0' || s.getValue()=='\n')
                alignChanges.append(s);
            if (s.getValue() == '\0') {
                continue;
            }

            partial.append(s.getValue());
    }
    emit insertGroup(firstLine,firstIndex,partial,newFormat);
    //not optimal
    for (int i=0;i<alignChanges.size();i++){
          handleRemoteAlignChange(alignChanges.at(i));
    }
}

void CRDT::handleRemoteInsert(const Symbol& s) {
//    qDebug() << "REMOTE INSERT" << QString(s.getValue()); // << QString(1, s.getValue());
    int line, index;
    if (_symbols.size() != 0) {
        findInsertPosition(s, line, index);
    } else {
        line=0;
        index=0;
    }

    // insert in crdt structure
    insertChar(s, line, index);

    this->size++;

    qDebug() << "REMOTE insert" << QString(s.getValue()) << line << index;
    // insert in editor
//    if (line==_symbols.size()-1 && index>0)
//        index-=1;
    if (s.getValue() == '\0') {
        qDebug() << "RETURN";
        return;
    }
    // insert in text editor
    emit insert(line, index, s);
}

void CRDT::insertChar(Symbol s, int line, int index) {
//    qDebug()<<"SIZE SYMBOLS: " << _symbols.size();
//    qDebug()<<" Trying to isnert "<< s.getValue()<<" to (line,index): ("<<line<<","<<index<<")";

    if (s.getValue() == '\n')  {// split line into two, before and after the '\n'
            QVector<Symbol> lineBefore;
//            qDebug()<<"Line: "<<line<< " index: "<<index;
            std::copy(_symbols[line].begin(), _symbols[line].begin() + index, std::back_inserter(lineBefore));
            QVector<Symbol> lineAfter;
//             qDebug()<<"Line: "<<line<< " index: "<<index;
            std::copy(_symbols[line].begin() + index, _symbols[line].end(), std::back_inserter(lineAfter));

            lineBefore.push_back(s); // include '\n' in line before
            _symbols[line] = lineBefore;

            _symbols.insert(line+1,lineAfter);


    } else {
        _symbols[line].insert(index, s);
    }
}

void CRDT::findInsertPosition(Symbol s, int& line, int& index) {
    int minLine = 0;
    int totalLines = _symbols.size();
    int maxLine = totalLines - 1;

    QVector<Symbol> lastLine = _symbols[maxLine], currentLine, minCurrentLine, maxCurrentLine;
    int midLine;
    Symbol minLastChar, maxLastChar;
    if ((_symbols.size() == 1 && _symbols[0].size() == 0) || Symbol::compare(s, _symbols[0][0]) <= 0) {
        line = 0;
        index = 0;
        return;
    }

    Symbol lastChar = lastLine[lastLine.size() - 1];

    if (Symbol::compare(s, lastChar) > 0) {
        findEndPosition(lastChar, lastLine, totalLines, line, index);
        return;
    }

    // binary search
    while (minLine + 1 < maxLine) {
        midLine = minLine + (maxLine - minLine) / 2;
        currentLine = _symbols[midLine];
        lastChar = currentLine[currentLine.size() - 1];

        if (Symbol::compare(s, lastChar) == 0) {
            line = midLine;
            index = currentLine.size() - 1;
            return;
        } else if (Symbol::compare(s, lastChar) < 0) {
            maxLine = midLine;
        } else {
            minLine = midLine;
        }
    }

    minCurrentLine = _symbols[minLine];
    minLastChar = minCurrentLine[minCurrentLine.size() - 1];
    maxCurrentLine = _symbols[maxLine];
    maxLastChar = maxCurrentLine[maxCurrentLine.size() - 1];

    if (Symbol::compare(s, minLastChar) <= 0) {
        index = findInsertIndexInLine(s, minCurrentLine);
        line = minLine;
    } else {
        index = findInsertIndexInLine(s, maxCurrentLine);
        line = maxLine;
    }
}

int CRDT::findInsertIndexInLine(Symbol s, QVector<Symbol> line) {
    int left = 0, right = line.size() - 1, mid, compareNum;

    if (line.size() == 0 || Symbol::compare(s, line[left]) < 0) {
        return left;
    } else if (Symbol::compare(s, line[right]) > 0) {
        return _symbols.size();
    }

    while (left + 1 < right) {
        mid = left + (right - left) / 2;
        compareNum = Symbol::compare(s, line[mid]);

        if (compareNum == 0) {
            return mid;
        } else if (compareNum > 0) {
            left = mid;
        } else {
            right = mid;
        }
    }

    if (Symbol::compare(s, line[left]) == 0) {
        return left;
    } else {
        return right;
    }
}

void CRDT::findEndPosition(Symbol lastChar, QVector<Symbol> lastLine, int totalLines, int& line, int& index) {
    if (lastChar.getValue() == '\n') {
        line = totalLines;
        index = 0;
    } else {
        line = totalLines - 1;
        index = lastLine.size();
    }
}

void CRDT::handleRemoteErase(const Symbol& s) {
    int line, index;
    bool res = findPosition(s, line, index);
    qDebug()<<"ERASE res= "<<res<< " LINE: "<<line<<" INDEX: "<<index;
    if (!res){

        return;
      }

    bool newLineRemoved = (s.getValue() == '\n');
    if (index >= 0 && line >= 0) { // otherwise already deleted by another editor (i.e. another site)

        if (newLineRemoved && line + 1 < _symbols.size()) { // non-empty line after current line
            _symbols[line].erase(_symbols[line].begin() + index);
            std::copy(_symbols[line + 1].begin(), _symbols[line + 1].end(), std::back_inserter(_symbols[line]));
            _symbols.erase(_symbols.begin() + line + 1);
        } else if (index==0 && _symbols[line].size()==1){
             _symbols[line].erase(_symbols[line].begin() + index);
             _symbols.erase(_symbols.begin()+line);
        }
        else
            _symbols[line].erase(_symbols[line].begin() + index);
        qDebug() << "deleted";
        this->size--;
        emit erase(line, index);
        return;
    }
}

void CRDT::handleRemoteChange(const Symbol& s) {
    int line, index;
    bool res = findPosition(s, line, index);
    if (!res)
        return;

    // update symbol
    _symbols[line][index] = s;
    qDebug() << "updated";

    emit change(line, index, s);
}

Symbol CRDT::getSymbol(int line,int index) {
    qDebug()<<"getSymbol "<<line<<" , "<<index;

    Symbol s = _symbols[line][index];
    qDebug()<<"symbol= "<<s.getValue();
    return s;
}

void CRDT::getPositionFromSymbol(const Symbol& s, int& line, int& index) {
    findPosition(s, line, index);
}

int CRDT::getSiteID(){
    return _siteId;
}

int  CRDT::lineSize(int line){
    return _symbols[line].size();
}

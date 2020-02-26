#ifndef CRDT_H
#define CRDT_H

#include <QJsonObject>
#include <map>
#include <random>
#include "symbol.h"
#include "client.h"

#define BASE 32
#define BOUNDARY 10

typedef enum {INSERT, DELETE} OperationType;

class CRDT : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CRDT)

public:
    CRDT(int site, Client *client);
    int getId();
    void localInsert(int line, int index, char value, QFont font, QColor color);
    void localErase(int line, int index);
    int getSize();
    QString to_string();

private slots:
    void handleRemoteInsert(const Symbol& s);
    void handleRemoteErase(const Symbol& s);

signals:
    void insert(int line, int index, const Symbol& s);
    void erase(int line, int index);

private:
    int _siteId;
    QVector<QVector<Symbol>> _symbols;
    int _counter = 0;
    std::map<int,bool> strategyCache;
    Client *client;
    int size = 0;

    QVector<Identifier> generatePositionBetween(QVector<Identifier>& pos1, QVector<Identifier> pos2, QVector<Identifier>& newPos, int level=0);
    int generateIdBetween(int id1, int id2, int level);
    bool generateRandomBool();
    int generateRandomNumBetween(int n1,int n2);
    bool findPosition(Symbol s, int& line, int& index); // TODO: use reference
    int findIndexInLine(Symbol s, QVector<Symbol> line);
    void findInsertPosition(Symbol s, int& line, int& index);
    int findInsertIndexInLine(Symbol s, QVector<Symbol> line);
    void findEndPosition(Symbol lastChar, QVector<Symbol> lastLine, int totalLines, int& line, int& index);
    void insertChar(Symbol s, int line, int index);

    QVector<Identifier> findPosBefore(int line, int index);
    QVector<Identifier> findPosAfter(int line, int index);
};

#endif // CRDT_H

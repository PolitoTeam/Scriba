#ifndef CRDT_H
#define CRDT_H

#include <QJsonObject>
#include <map>
#include <random>
#include "message.h"
#include "symbol.h"
#include "client.h"

#define BASE 32
#define BOUNDARY 10

class CRDT : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CRDT)

public:
    CRDT(int site, Client *client);
    int getId();
    void localInsert(int index, char value);
    void localErase(int index);
    int getSize();

private slots:
    void handleRemoteInsert(const Symbol& s);
    void handleRemoteErase(const Symbol& s);

signals:
    void insert(int index, char value);

private:
    int _siteId;
    std::vector<Symbol> _symbols;
    int _counter = 0;
    std::map<int,bool> strategyCache;
    Client *client;

    std::vector<Identifier> generatePositionBetween(std::vector<Identifier>& pos1, std::vector<Identifier> pos2, std::vector<Identifier>& newPos, int level=0);
    int generateIdBetween(int id1, int id2, int level);
    bool generateRandomBool();
    int generateRandomNumBetween(int n1,int n2);
    int findIndexByPosition(Symbol s);
    int findInsertIndex(Symbol s);
    QString to_string();
};

#endif // CRDT_H

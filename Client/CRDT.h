#ifndef CRDT_H
#define CRDT_H

#include "../Utility/common.h"
#include "../Utility/symbol.h"
#include "client.h"
#include <QJsonObject>
#include <map>
#include <random>

#define BASE 32
#define BOUNDARY 10

class CRDT : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(CRDT)

public:
  CRDT(Client *client);
  void clear();
  int getId();
  void setId(int site);
  void localInsert(int line, int index, ushort value, QFont font, QColor color,
                   Qt::Alignment align);
  void localInsertGroup(int &line, int &index, QString partial, QFont font,
                        QColor color, Qt::Alignment align);
  void localErase(int &line, int &index, int length);

  int getSiteID();
  void localChangeAlignment(int line, SymbolFormat::Alignment align);
  void localChange(int line, int index, QFont font, QColor color);
  void localChangeGroup(int startLine, int endLine, int startIndex,
                        int endIndex, QFont font, QColor color);
  int getSize();
  QString to_string();
  Symbol getSymbol(int line, int index);
  void cursorPositionChanged(int line, int index);
  void getPositionFromSymbol(const Symbol &s, int &line, int &index);
  SymbolFormat::Alignment getAlignmentLine(int line);
  QTextCharFormat getSymbolFormat(int line, int index);
  int lineSize(int line);
  bool findPosition(Symbol s, int &line, int &index);

private slots:
  void handleRemoteInsert(const Symbol &s);
  void handleRemotePaste(const QVector<Symbol> &s);
  void handleRemoteErase(const QVector<Symbol> &s);
  void handleRemoteChange(const QVector<Symbol> &s);
  void handleRemoteAlignChange(const Symbol &s);

signals:
  void insert(int line, int index, const Symbol &s);
  void insertGroup(int firstLine, int firstIndex, QString partial,
                   QTextCharFormat newFormat);
  void erase(int startLine, int startIndex, int lenght);
  void change(const QVector<Symbol> &symbols);
  void changeAlignment(int align, int line, int index);

private:
  int _siteId;
  QVector<QVector<Symbol>> _symbols;
  int _counter = 0;
  std::map<int, bool> strategyCache;
  Client *client;
  int size = 0;

  QVector<Identifier> generatePositionBetween(QVector<Identifier> &pos1,
                                              const QVector<Identifier> &pos2,
                                              QVector<Identifier> &newPos,
                                              int level = 0);
  int generateIdBetween(int id1, int id2, int level);
  bool generateRandomBool();
  int generateRandomNumBetween(int n1, int n2);

  int findIndexInLine(Symbol s, QVector<Symbol> line);
  void findInsertPosition(Symbol s, int &line, int &index);
  int findInsertIndexInLine(Symbol s, QVector<Symbol> line);
  void findEndPosition(Symbol lastChar, QVector<Symbol> lastLine,
                       int totalLines, int &line, int &index);
  void insertChar(Symbol s, int line, int index);

  QVector<Identifier> findPosBefore(int line, int index);
  QVector<Identifier> findPosAfter(int line, int index);
};

#endif // CRDT_H

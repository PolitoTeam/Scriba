#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include "symbol.h"
class SharedEditor;

typedef enum {INSERT, DELETE} OperationType;

class Message {
private:
    int editorId;
    Symbol symbol;
    OperationType operation;

public:
    Message(int editorId, Symbol symbol, OperationType operation) : editorId(editorId), symbol(symbol), operation(operation) {}
    int getEditorId() { return editorId; }
    OperationType getOperation() const { return operation; }
    Symbol getSymbol() const { return symbol; }
};

#endif // MESSAGE_H

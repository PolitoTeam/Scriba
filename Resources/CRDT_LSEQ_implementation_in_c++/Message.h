//
// Created by enrico on 06/05/19.
//

#ifndef LAB_03_MESSAGE_H
#define LAB_03_MESSAGE_H

#include <string>
#include "Symbol.h"
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

#endif //LAB_03_MESSAGE_H

#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *document, CRDT* crdt)
		: QSyntaxHighlighter(document),crdt(crdt) {
	list_colors = Colors();
}

bool Highlighter::addClient(int editor_id) {
	if (this->users.contains(editor_id))
		return false;

	int i = list_colors.getIndex();
	users.insert(editor_id, i);
	return true;
}

void Highlighter::addLocal(int editor_id){
	if (this->users.contains(editor_id))
		return;
	users.insert(editor_id, -1);
}

void Highlighter::highlightBlock(const QString &text){
	// Save cursor position
	int line = this->currentBlock().blockNumber();

	for (int index = 0; index < text.length(); index++) {
		// Retrieve symbol at line, index;
		Symbol s = this->crdt->getSymbol(line,index);
		int editor_id = s.getUsername();
		QTextCharFormat format = s.getQTextCharFormat();

		int id;
		if (users.contains(editor_id)) {
			id = users.value(editor_id);
		} else {
			id = -2; // Remote but offline
		}
		QColor color = list_colors.getColor(id);

		format.setBackground(QBrush(color,Qt::SolidPattern));
		setFormat(index, 1, format);
	}
}

QColor Highlighter::getColor(int editor_id){
	if (users.contains(editor_id)) {
		return list_colors.getColor(users[editor_id]);
	}
	// TODO: da gestire
	return QColor('white');
}

void Highlighter::freeColor(int editor_id){
	list_colors.freeColor(users.value(editor_id));
	users.remove(editor_id);
}

void Highlighter::freeAll(){
	list_colors.clear();
	users.clear();
}

void Highlighter::setCRDT(CRDT* crdt){
	this->crdt=crdt;
}

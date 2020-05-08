#include <QFont>
#include "CRDT.h"

CRDT::CRDT(Client *client) : client(client) {
	connect(client, &Client::remoteInsert, this, &CRDT::handleRemoteInsert);
	connect(client, &Client::remotePaste, this, &CRDT::handleRemotePaste);
	connect(client, &Client::remoteErase, this, &CRDT::handleRemoteErase);
	connect(client, &Client::remoteChange, this, &CRDT::handleRemoteChange);
	connect(client, &Client::remoteAlignChange,
			this, &CRDT::handleRemoteAlignChange);

	_symbols.push_back(QVector<Symbol>{});
	// Terminator ('\0') should not be included in the count
	this->size = -1;
}


void CRDT::clear(){
	disconnect(client, &Client::remoteInsert, this, &CRDT::handleRemoteInsert);
	disconnect(client, &Client::remotePaste, this, &CRDT::handleRemotePaste);
	disconnect(client, &Client::remoteErase, this, &CRDT::handleRemoteErase);
	disconnect(client, &Client::remoteChange, this, &CRDT::handleRemoteChange);
	disconnect(client, &Client::remoteAlignChange,
			   this, &CRDT::handleRemoteAlignChange);

	for (QVector<Symbol> v: _symbols){
		v.clear();
	}
	_symbols.clear();
	_symbols.push_back(QVector<Symbol>{});
	this->size=-1;
	_siteId=0;
	_counter=0;
	strategyCache.clear();

	connect(client, &Client::remoteInsert, this, &CRDT::handleRemoteInsert);
	connect(client, &Client::remotePaste, this, &CRDT::handleRemotePaste);
	connect(client, &Client::remoteErase, this, &CRDT::handleRemoteErase);
	connect(client, &Client::remoteChange, this, &CRDT::handleRemoteChange);
	connect(client, &Client::remoteAlignChange,
			this, &CRDT::handleRemoteAlignChange);
}

int CRDT::getId() { return _siteId; }
void CRDT::setId(int site) { this->_siteId = site; }

QTextCharFormat CRDT::getSymbolFormat(int line,int index){
	return _symbols[line][index].getQTextCharFormat();
}

void CRDT::localInsert(int line, int index, ushort value, QFont font,
					   QColor color,Qt::Alignment align) {
	if (line < 0 || index < 0)
		throw std::runtime_error("Error: index out of bound.\n");

	// Calculate position
	QVector<Identifier> posBefore = findPosBefore(line, index);
	QVector<Identifier> posAfter = findPosAfter(line, index);
	QVector<Identifier> newPos;
	QVector<Identifier> position = generatePositionBetween(posBefore,
														   posAfter, newPos);

	// Generate symbol
	Symbol s(value, newPos, ++_counter, font, color);

	if (s.getValue()=='\0' || s.getValue()=='\n'){
		if (align==Qt::AlignLeft)
			s.setAlignment(SymbolFormat::Alignment::ALIGN_LEFT);
		else if (align==Qt::AlignHCenter)
			s.setAlignment(SymbolFormat::Alignment::ALIGN_CENTER);
		else if (align==Qt::AlignRight)
			s.setAlignment(SymbolFormat::Alignment::ALIGN_RIGHT);
	}

	insertChar(s, line, index);
	this->size++;

	// Broadcast
	QJsonObject message;
	message["type"] = QStringLiteral("operation");
	message["editorId"] = _siteId;
	message["operation_type"] = INSERT;
	message["symbol"] = s.toJson();

	client->sendJson(message);
}

void CRDT::localInsertGroup(int& line, int& index, QString partial,
							QFont font, QColor color,Qt::Alignment align) {
	if (line < 0 || index < 0)
		throw std::runtime_error("Error: index out of bound.\n");
	QJsonArray symbols;
	for (int i =0; i<partial.length(); i++){
		// Calculate position
		QVector<Identifier> posBefore = findPosBefore(line, index);
		QVector<Identifier> posAfter = findPosAfter(line, index);
		QVector<Identifier> newPos;
		QVector<Identifier> position = generatePositionBetween(posBefore,
															   posAfter, newPos);

		// Generate symbol
		Symbol s(partial.at(i).unicode(), newPos, ++_counter, font, color);
		if (s.getValue()=='\0' || s.getValue()=='\n'){
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
		if (s.getValue()=='\n') {
			line+=1;
			index=0;
		} else {
			index+=1;
		}
	}

	// Broadcast
	QJsonObject message;
	message["type"] = QStringLiteral("operation");
	message["editorId"] = _siteId;
	message["operation_type"] = PASTE;
	message["symbols"] = symbols;

	client->sendJson(message);
}

void CRDT::localChangeAlignment(int line,SymbolFormat::Alignment align) {
	Symbol s = _symbols[line][_symbols[line].size()-1];
	s.setAlignment(align);

	// Broadcast
	QJsonObject message;
	message["type"] = QStringLiteral("operation");
	message["editorId"] = _siteId;
	message["operation_type"] = ALIGN;
	message["symbol"] = s.toJson();

	client->sendJson(message);
}

SymbolFormat::Alignment CRDT::getAlignmentLine(int line) {
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

QVector<Identifier> CRDT::generatePositionBetween(QVector<Identifier>& pos1,
												  const QVector<Identifier>& pos2,
												  QVector<Identifier>& newPos,
												  int level) {
	Identifier id1 =
			level < pos1.size() ? pos1[level] : Identifier(0, this->_siteId);
	Identifier id2 =
			level < pos2.size() ? pos2[level] : Identifier(BASE, this->_siteId);
													// == BASE * std::pow(2, 0)

	if (id2.digit - id1.digit > 1) {
		// Case 1: enough space to add in between
		int newDigit = generateIdBetween(id1.digit, id2.digit, level);
		newPos.push_back(Identifier(newDigit, this->_siteId));
		return newPos;
	} else if (id2.digit - id1.digit == 1) {
		// Case 2: no space in between, use identifier of first position
		newPos.push_back(id1);
		return generatePositionBetween(pos1, QVector<Identifier> {},
									   newPos, level + 1);
	} else if (id1.digit == id2.digit) {
		// Case 3: same digit, use site id to discriminate
		if (id1.site < id2.site) {
			newPos.push_back(id1);
			return generatePositionBetween(pos1, QVector<Identifier> {},
										   newPos, level + 1);
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
	if (strategyCache[level]) { // Boundary+
		int delta = generateRandomNumBetween(1, step - 1);
		return id1 + delta;
	}
	else{ // Boundary-
		int delta = generateRandomNumBetween(1, step - 1);
		return id2 - delta;
	}
}

bool CRDT::generateRandomBool() {
	static auto gen = std::bind(std::uniform_int_distribution<>(0,1),
								std::default_random_engine());
	return gen();
}

// Returns n1 <= x <= n2
int CRDT::generateRandomNumBetween(int n1,int n2) {
	return n1 + (std::rand() % (n2 - n1 + 1));
}

void CRDT::localErase(int& line, int& index,int lenght) {
	QJsonArray symbols;

	for (int i=0;i<lenght;i++){
		Symbol s = _symbols[line][index];
		symbols.append(s.toJson());
		bool newLineRemoved = (s.getValue() == '\n');
		_symbols[line].erase(_symbols[line].begin() + index);

		// Non-empty line after current line
		if (newLineRemoved && line + 1 < _symbols.size()) {
			std::copy(_symbols[line + 1].begin(), _symbols[line + 1].end(),
					std::back_inserter(_symbols[line]));
			_symbols.erase(_symbols.begin() + line + 1);
		}
		this->size--;
	}

	// Broadcast
	QJsonObject message;
	message["type"] = QStringLiteral("operation");
	message["editorId"] = _siteId;
	message["operation_type"] = DELETE;
	message["symbols"] = symbols;

	client->sendJson(message);
}

void CRDT::localChange(int line, int index, QFont font, QColor color) {
	Symbol s = _symbols[line][index];

	// Update font and color
	s.setFormat(font, color);
	_symbols[line][index] = s;

	// Broadcast
	QJsonObject message;
	message["type"] = QStringLiteral("operation");
	message["editorId"] = _siteId;
	message["operation_type"] = CHANGE;
	message["symbol"] = s.toJson();

	client->sendJson(message);
}

void CRDT::cursorPositionChanged(int line, int index) {
	Symbol s = _symbols[line][index];

	// Broadcast
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

	// Binary search
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
	bool res;
	if (!(res=findPosition(s,line,index))) {
		return;
	}

	// Insert in editor
	emit changeAlignment(align,line, index);
}

void CRDT::handleRemotePaste(const QJsonArray& symbols){
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

		if (i == 0) {
			firstLine=line;
			firstIndex=index;
			newFormat=s.getQTextCharFormat();
		}

		// Insert in crdt structure
		insertChar(s, line, index);
		this->size++;

		if (s.getValue()=='\0' || s.getValue()=='\n') {
			alignChanges.append(s);
		}
		if (s.getValue() == '\0') {
			continue;
		}

		partial.append(s.getValue());
	}
	emit insertGroup(firstLine,firstIndex,partial,newFormat);
	// TODO: not optimal
	for (int i=0;i<alignChanges.size();i++) {
		handleRemoteAlignChange(alignChanges.at(i));
	}
}

void CRDT::handleRemoteInsert(const Symbol& s) {
	int line, index;
	if (_symbols.size() != 0) {
		findInsertPosition(s, line, index);
	} else {
		line=0;
		index=0;
	}

	// Insert in crdt structure
	insertChar(s, line, index);
	this->size++;

	if (s.getValue() == '\0') {
		return;
	}

	// Insert in text editor
	emit insert(line, index, s);
}

void CRDT::insertChar(Symbol s, int line, int index) {
	if (s.getValue() == '\n')  {	// Split line into two,
									// before and after the '\n'
		QVector<Symbol> lineBefore;
		std::copy(_symbols[line].begin(), _symbols[line].begin() + index,
				  std::back_inserter(lineBefore));
		QVector<Symbol> lineAfter;
		std::copy(_symbols[line].begin() + index, _symbols[line].end(),
				  std::back_inserter(lineAfter));

		lineBefore.push_back(s);  // Include '\n' in line before
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

	QVector<Symbol> lastLine = _symbols[maxLine], currentLine,
							   minCurrentLine, maxCurrentLine;
	int midLine;
	Symbol minLastChar, maxLastChar;
	if ((_symbols.size() == 1 && _symbols[0].size() == 0)
			|| Symbol::compare(s, _symbols[0][0]) <= 0) {
		line = 0;
		index = 0;
		return;
	}

	Symbol lastChar = lastLine[lastLine.size() - 1];

	if (Symbol::compare(s, lastChar) > 0) {
		findEndPosition(lastChar, lastLine, totalLines, line, index);
		return;
	}

	// Binary search
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

void CRDT::findEndPosition(Symbol lastChar, QVector<Symbol> lastLine,
						   int totalLines, int& line, int& index) {
	if (lastChar.getValue() == '\n') {
		line = totalLines;
		index = 0;
	} else {
		line = totalLines - 1;
		index = lastLine.size();
	}
}

void CRDT::handleRemoteErase(const QJsonArray& symbols) {
	int startLine,startIndex,endLine,endIndex;

	for (int i = 0; i < symbols.size(); i++) {
		QJsonObject symbol = symbols[i].toObject();
		Symbol s = Symbol::fromJson(symbol);

		int line, index;
		bool res = findPosition(s, line, index);
		if (i == 0) {
			startLine=line;
			startIndex=index;
		}
		endLine=line;
		endIndex=index;

		if (!res) {
			return;
		}

		bool newLineRemoved = (s.getValue() == '\n');
		if (index >= 0 && line >= 0) {	// Otherwise already deleted
										// by another editor (i.e. another site)

			// Non-empty line after current line
			if (newLineRemoved && line + 1 < _symbols.size()) {
				_symbols[line].erase(_symbols[line].begin() + index);
				std::copy(_symbols[line + 1].begin(), _symbols[line + 1].end(),
						  std::back_inserter(_symbols[line]));
				_symbols.erase(_symbols.begin() + line + 1);
			} else if (index==0 && _symbols[line].size()==1) {
				_symbols[line].erase(_symbols[line].begin() + index);
				_symbols.erase(_symbols.begin()+line);
			} else {
				_symbols[line].erase(_symbols[line].begin() + index);
			}
			this->size--;
		}
	}
	emit erase(startLine, startIndex,symbols.size());
}

void CRDT::handleRemoteChange(const Symbol& s) {
	int line, index;
	bool res = findPosition(s, line, index);
	if (!res)
		return;

	// Update symbol
	_symbols[line][index] = s;

	emit change(line, index, s);
}

Symbol CRDT::getSymbol(int line,int index) {
	Symbol s = _symbols[line][index];
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

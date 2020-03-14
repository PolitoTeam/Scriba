#include <QFontDialog>
#include "editor.h"
#include "ui_editor.h"
#include "client.h"
#include <QDateTime>
#include <QIcon>
#include <QMessageBox>
#include <QClipboard>
#include <QMessageBox>
#include <QTextBlock>
#include <QCryptographicHash>
#include <QtWidgets>
#include <typeinfo>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#if QT_CONFIG(printpreviewdialog)
#include <QPrintPreviewDialog>
#endif
#endif
#endif

Editor::Editor(QWidget *parent,Client* client) :
    QMainWindow(parent),
    ui(new Ui::Editor),
    client(client)
{
    ui->setupUi(this);

   // this->setCentralWidget(ui->textEdit);
//    QTextDocument *document = ui->textEdit->document(); //cursore
//    QTextCursor cursor(document);
//    cursor.insertText(tr("Hello world!"));
//    cursor.movePosition(QTextCursor::End);

    connect(ui->actionPrint, &QAction::triggered, this, &Editor::printPdf);
    connect(ui->actionExit, &QAction::triggered, this, &Editor::exit);
    connect(ui->actionCopy, &QAction::triggered, this, &Editor::copy);
    connect(ui->actionCut, &QAction::triggered, this, &Editor::cut);
    connect(ui->actionPaste, &QAction::triggered, this, &Editor::paste);
    connect(ui->actionUndo, &QAction::triggered, this, &Editor::undo);
    connect(ui->actionRedo, &QAction::triggered, this, &Editor::redo);
    connect(ui->actionFont, &QAction::triggered, this, &Editor::selectFont);
    connect(ui->actionBold, &QAction::triggered, this, &Editor::setFontBold);
    connect(ui->actionUnderline, &QAction::triggered, this, &Editor::setFontUnderline);
    connect(ui->actionItalic, &QAction::triggered, this, &Editor::setFontItalic);
    connect(client,&Client::usersConnectedReceived,this,&Editor::addUsers);
    connect(client,&Client::contentReceived,this,&Editor::updateText);
    connect(ui->textEdit,&QTextEdit::textChanged,this,&Editor::textChange);
    connect(client,&Client::userDisconnected,this,&Editor::removeUser);
    connect(client, &Client::addCRDTterminator, this, &Editor::on_addCRDTterminator);
    connect(client, &Client::remoteCursor, this, &Editor::on_remoteCursor);
    connect(ui->actionSharedLink, &QAction::triggered, this, &Editor::sharedLink);

    crdt = new CRDT(client);
    highlighter = new Highlighter(0,crdt);
    connect(this->client,&Client::loggedIn,this->highlighter,[this]{this->highlighter->addClient(this->crdt->getSiteID(),QColor(124,252,0,127));});
    connect(this->client,&Client::loggedIn,this,[this]{this->crdt->setId(fromStringToIntegerHash(this->client->getUsername()));});
    connect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);
    connect(crdt, &CRDT::insert, this, &Editor::on_insert);
    connect(crdt, &CRDT::insertGroup, this, &Editor::on_insertGroup);
    connect(crdt, &CRDT::erase, this, &Editor::on_erase);
    connect(crdt, &CRDT::change, this, &Editor::on_change);
    connect(crdt, &CRDT::changeAlignment, this, &Editor::on_changeAlignment);

    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &Editor::saveCursorPosition);
    connect(ui->textEdit, &QTextEdit::currentCharFormatChanged, this, &Editor::on_currentCharFormatChanged);

    // ADD font, size and color to toolbar (cannot be otherwise achieved using Qt creator GUI):
    // 1. font
    comboFont = new QFontComboBox(ui->toolBar);
    ui->toolBar->addWidget(comboFont);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &Editor::textFamily);

    // 2. size
    comboSize = new QComboBox(ui->toolBar);
    ui->toolBar->addWidget(comboSize);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &Editor::textSize);

    // 3. color
    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = ui->toolBar->addAction(pix, tr("&Color..."), this, &Editor::textColor);

    // undo/redo config
    connect(ui->textEdit->document(), &QTextDocument::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(ui->textEdit->document(), &QTextDocument::redoAvailable, ui->actionRedo, &QAction::setEnabled);
    ui->actionUndo->setEnabled(ui->textEdit->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->textEdit->document()->isRedoAvailable());

    // copy/paste/cut config
#ifndef QT_NO_CLIPBOARD
    ui->actionCut->setEnabled(false);
    connect(ui->textEdit, &QTextEdit::copyAvailable, ui->actionCut, &QAction::setEnabled);
    ui->actionCopy->setEnabled(false);
    connect(ui->textEdit, &QTextEdit::copyAvailable, ui->actionCopy, &QAction::setEnabled);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &Editor::clipboardDataChanged);
#endif

    // add alignment icons
    const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(":/images/textleft.png"));
    actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(":/images/textcenter.png"));
    actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(":/images/textright.png"));
    actionAlignRight = new QAction(rightIcon, tr("&Right"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    ui->toolBar->addAction(actionAlignLeft);
    ui->toolBar->addAction(actionAlignCenter);
    ui->toolBar->addAction(actionAlignRight);

    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &Editor::textAlign);
    alignGroup->addAction(actionAlignLeft);
    alignGroup->addAction(actionAlignCenter);
    alignGroup->addAction(actionAlignRight);
    ui->toolBar->addSeparator();
    ui->toolBar->addActions(alignGroup->actions());
    ui->toolBar->addSeparator();

    //show assigned text
    const QIcon assigned = QIcon::fromTheme("cursor", QIcon(":/images/cursor.png"));
    actionShowAssigned = new QAction(assigned, tr("cursor"), this);
    actionShowAssigned->setCheckable(true);
    actionShowAssigned->setChecked(false);
    ui->toolBar->addAction(actionShowAssigned);
    connect(actionShowAssigned,&QAction::triggered,this,&Editor::on_showAssigned);



    //
//    connect(ui->actionFont, &QAction::triggered, this, &Editor::on_formatChange);
//    connect(ui->actionBold, &QAction::triggered, this, &Editor::on_formatChange);
//    connect(ui->actionUnderline, &QAction::triggered, this, &Editor::on_formatChange);
//    connect(comboFont, &QFontComboBox::currentFontChanged, this, &Editor::on_formatChange);
//    connect(comboSize, &QComboBox::, this, &Editor::on_formatChange);
//    connect(actionTextColor, &QAction::triggered, this, &Editor::on_formatChange);
//      connect(this, &Editor::formatChange, this, &Editor::on_formatChange);



}



int Editor::fromStringToIntegerHash(QString str) {
    qDebug()<<"Hash of: "<<str;
    auto hash = QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5);
    QDataStream data(hash);
    qDebug()<< "String to hash: "<<hash;
    qDebug()<< " type of hash: "<<typeid(hash).name();
    int intHash;
    data >> intHash;
    return intHash;
}

void Editor::on_showAssigned(){
    if (this->highlighter->document()==0){
        qDebug()<< "Assigning file";
        this->highlighter->setDocument(ui->textEdit->document());
         qDebug()<< "Assigned file";
    }
    else {
        this->highlighter->setDocument(0);
        qDebug()<< "Removing file";
    }
}

Editor::~Editor()
{
    delete ui;
}

void Editor::textChange(){
//    qDebug()<<"Cursor position: "<<ui->textEdit->textCursor().position()<<endl;
}

void Editor::setClient(Client *client){
    this->client=client;
}

void Editor::printPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    ui->textEdit->document()->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));
//! [0]
#endif
}
void Editor::exit()
{
    client->closeFile();

    // clean the editor: disconnect...
    disconnect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);
    disconnect(client, &Client::remoteCursor, this, &Editor::on_remoteCursor);
    disconnect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &Editor::saveCursorPosition);

    this->clear();
    // create new CRDT with connections
    delete crdt;
    crdt = new CRDT(client);
    crdt->setId(fromStringToIntegerHash(client->getUsername()));
    connect(crdt, &CRDT::insert, this, &Editor::on_insert);
    connect(crdt, &CRDT::insertGroup, this, &Editor::on_insertGroup);
    connect(crdt, &CRDT::erase, this, &Editor::on_erase);
    connect(crdt, &CRDT::change, this, &Editor::on_change);
    connect(crdt, &CRDT::changeAlignment, this, &Editor::on_changeAlignment);
    // ... and then riconnect (because we want to remove chars locally without deleteling them in server)
    connect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);
    connect(client, &Client::remoteCursor, this, &Editor::on_remoteCursor);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &Editor::saveCursorPosition);

    emit changeWidget(HOME);
}

void Editor::copy()
{
#if QT_CONFIG(clipboard)
    ui->textEdit->copy();
#endif
}

void Editor::cut()
{
#if QT_CONFIG(clipboard)
    ui->textEdit->cut();
#endif
}

void Editor::paste()
{
#if QT_CONFIG(clipboard)
    ui->textEdit->paste();
#endif
}

void Editor::undo()
{
     ui->textEdit->undo();
}

void Editor::redo()
{
    ui->textEdit->redo();
}

void Editor::selectFont()
{
    bool fontSelected;
    QFont font = QFontDialog::getFont(&fontSelected, this);
    if (fontSelected)
        ui->textEdit->setFont(font);
}

void Editor::setFontUnderline(bool underline)
{
    ui->textEdit->setFontUnderline(underline);
    on_formatChange();
}

void Editor::setFontItalic(bool italic)
{
    ui->textEdit->setFontItalic(italic);
    on_formatChange();

}

void Editor::sharedLink()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(client->getSharedLink());

    QMessageBox::information(this, tr("Shared Link"), tr("Link copied to clipboard."), QMessageBox::Ok);
}

void Editor::setFontBold(bool bold)
{
    bold ? ui->textEdit->setFontWeight(QFont::Bold) :
           ui->textEdit->setFontWeight(QFont::Normal);
    on_formatChange();
}

void Editor::textAlign(QAction *a)
{
    QString changed = ui->textEdit->textCursor().selectedText();
    QTextCursor cursor = ui->textEdit->textCursor();
    int start = ui->textEdit->textCursor().selectionStart();
    cursor.setPosition(start);
    int line_start = cursor.blockNumber();
    qDebug()<<"START POSITION: "<< line_start;
    int end = ui->textEdit->textCursor().selectionEnd();
    cursor.setPosition(end);
    int line_end = cursor.blockNumber();
    qDebug()<<"END POSITION: "<< line_end;

    for (int i = line_start; i <= line_end; i++) {
        if (a == actionAlignLeft){
            ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
            crdt->localChangeAlignment(i,SymbolFormat::Alignment::ALIGN_LEFT);
        }
        else if (a == actionAlignCenter){
            ui->textEdit->setAlignment(Qt::AlignHCenter);
            crdt->localChangeAlignment(i,SymbolFormat::Alignment::ALIGN_CENTER);
        }
        else if (a == actionAlignRight){
            ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
            crdt->localChangeAlignment(i,SymbolFormat::Alignment::ALIGN_RIGHT);
        }
    }
}

/****************************************************
    LOCAL OPERATION: update textedit THEN crdt
    REMOTE OPERATION: update crdt THEN textedit
****************************************************/
void Editor::on_contentsChange(int position, int charsRemoved, int charsAdded) {    
    qDebug() << "[total text size" << ui->textEdit->toPlainText().size() << "crdt size" << crdt->getSize();
    qDebug() << "added" << charsAdded << "removed" << charsRemoved;
    qDebug() << "line" << this->line << "index" << this->index << ui->textEdit->remote_cursors.size() << "]";

    // REMOTE OPERATION: insert/delete received from remote client
    // nothing to update
    // "charsRemoved == 0" and "charsAdded == 0" are conditions added to handle QTextDocument::contentsChange bug QTBUG-3495
   /* if ((charsAdded > 0 && charsRemoved == 0 && ui->textEdit->toPlainText().size() - ui->textEdit->remote_cursors.size() <= crdt->getSize())
            || (charsRemoved > 0 && charsAdded == 0 && ui->textEdit->toPlainText().size() - ui->textEdit->remote_cursors.size() >= crdt->getSize())) {
        qDebug() << "rem";
        return;
    } */

    if ((charsAdded > 0 && ui->textEdit->toPlainText().size() <= crdt->getSize())
            || (charsRemoved > 0 && ui->textEdit->toPlainText().size() >= crdt->getSize())) {
        return;
    }

    // LOCAL OPERATION: insert/deleted performed in this editor
    // update CRDT structure
    // "charsAdded - charsRemoved" and "charsRemoved - charsAdded" are conditions added to handle QTextDocument::contentsChange bug QTBUG-3495
    if (charsAdded > 0 && charsAdded - charsRemoved > 0) {
        QString added = ui->textEdit->toPlainText().mid(position, charsAdded - charsRemoved);
        if (added.at(0) == '\0')
            return;
        // move cursor before first char to insert
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.setPosition(ui->textEdit->textCursor().position() - (charsAdded - charsRemoved));
        if ((charsAdded - charsRemoved)==1){
            //single character
            int line = cursor.blockNumber();
            int index = cursor.positionInBlock();
            //correct_position(line, index);
            qDebug() << "Added " << added.at(0) << "in position (" << line << "," << index << ")";

            // to retrieve the format it is necessary to be on the RIGHT of the target char
            cursor.movePosition(QTextCursor::Right);
            QFont font = cursor.charFormat().font();
            ui->textEdit->update();
            crdt->localInsert(line, index, added.at(0).unicode(), font, cursor.charFormat().foreground().color());
        } else{
            QFont fontPrec;
            QColor colorPrec;
            QString partial;
            Qt::Alignment alignPrec = ui->textEdit->document()->findBlockByNumber(line).blockFormat().alignment();
            int linePrec = line;
            int numLines = line;
            // add multiple chars
            qDebug() << "Multiple chars: position" << cursor.position()<<" (line,index): ( "<<line<<","<< index<<")";
            for (int i = 0; i < charsAdded - charsRemoved; i++) {

                //int line = cursor.blockNumber();
                //int index = cursor.positionInBlock();

                // to retrieve the format it is necessary to be on the RIGHT of the target char
                cursor.movePosition(QTextCursor::Right);
                QFont font = cursor.charFormat().font();
                QColor color = cursor.charFormat().foreground().color();
                Qt::Alignment align = alignPrec;
                if (i==0){
                    fontPrec=font;
                    colorPrec=color;
                }

                if (numLines!=linePrec){
                    QTextBlock block = ui->textEdit->document()->findBlockByNumber(numLines);
                    QTextBlockFormat textBlockFormat = block.blockFormat();
                    align = textBlockFormat.alignment();
                    qDebug()<<"line: "<<numLines<<" alignment: "<<align;
                }



                if (font == fontPrec && color==colorPrec && align==alignPrec){
                    qDebug()<<"concatenated: "<<added.at(i).unicode();
                    partial.append(added.at(i).unicode());
                }else{
                    crdt->localInsertGroup(line, index, partial, fontPrec, colorPrec,alignPrec);
                    fontPrec=font;
                    colorPrec=color;
                    alignPrec = align;
                    partial.clear();
                    partial.append(added.at(i).unicode());
                }

                if (i == (charsAdded - charsRemoved-1))
                    crdt->localInsertGroup(line, index, partial, font, color,align);
                linePrec=numLines;
                if (added.at(i)=='\n'){
                    numLines++;
                }
            }
        }
    } else if (charsRemoved > 0  && charsRemoved - charsAdded > 0) {
        // undo to retrieve the content deleted
        disconnect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);
        ui->textEdit->undo();
        QString removed = ui->textEdit->document()->toPlainText().mid(position, charsRemoved);
        ui->textEdit->redo();
        connect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);

        // remove multiple chars
        for (int i = 0; i < removed.length(); i++) {
            qDebug() << "Removed " << removed.at(i) << "in position (" << this->line << "," << this->index << ")";
            crdt->localErase(line, index);
        }
    }
}

void Editor::on_changeAlignment(int align,int line, int index)
{

    qDebug() << "ON_CHANGE_ALIGNMENT";
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
    cursor.setPosition(block.position() + index);
    QTextBlockFormat textBlockFormat = block.blockFormat();
    if (align==SymbolFormat::Alignment::ALIGN_LEFT){
        textBlockFormat.setAlignment(Qt::AlignLeft);
        if (line==this->line)
          actionAlignLeft->setChecked(true);
    }
    else if (align==SymbolFormat::Alignment::ALIGN_CENTER){
        textBlockFormat.setAlignment(Qt::AlignCenter);
        if (line==this->line)
            actionAlignCenter->setChecked(true);
        }
    else if (align==SymbolFormat::Alignment::ALIGN_RIGHT){
        textBlockFormat.setAlignment(Qt::AlignRight);
        if (line==this->line)
            actionAlignRight->setChecked(true);
        }
    cursor.mergeBlockFormat(textBlockFormat);

    qDebug().noquote() << crdt->to_string();
}


void Editor::on_insert(int line, int index, const Symbol& s)
{
    qDebug() << "ON_INSERT REMOTE";
    QTextCursor cursor = ui->textEdit->textCursor();
//    cursor.setPosition(index);

    QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
    cursor.setPosition(block.position() + index);

    // save old format to restore it later
    QTextCharFormat oldFormat = ui->textEdit->currentCharFormat();
//    qDebug() << "oldFormat" << oldFormat.font().italic() << oldFormat.font().bold() << oldFormat.font().underline();
    QTextCharFormat newFormat = s.getQTextCharFormat();
//    qDebug() << "format" << newFormat.font().bold();
    cursor.setCharFormat(newFormat);
    cursor.insertText(QChar(s.getValue()));
    ui->textEdit->setCurrentCharFormat(oldFormat);

    qDebug().noquote() << crdt->to_string();
}

void Editor::on_insertGroup(int line, int index, const QString& s,QTextCharFormat newFormat){
    qDebug() << "ON_INSERT";
    QTextCursor cursor = ui->textEdit->textCursor();
//    cursor.setPosition(index);

    QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
    cursor.setPosition(block.position() + index);

    // save old format to restore it later
    QTextCharFormat oldFormat = ui->textEdit->currentCharFormat();
//    qDebug() << "oldFormat" << oldFormat.font().italic() << oldFormat.font().bold() << oldFormat.font().underline();

//    qDebug() << "format" << newFormat.font().bold();
    cursor.setCharFormat(newFormat);
    cursor.insertText(s);
    ui->textEdit->setCurrentCharFormat(oldFormat);

    qDebug().noquote() << crdt->to_string();
}

void Editor::on_erase(int line, int index)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(index);

//    qDebug() << line << index;
    QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);

    cursor.setPosition(block.position() + index);
//    qDebug() << "block position" << block.position();

//    qDebug() << "before deleting";
    cursor.deleteChar();
//    qDebug() << "after deleting";

    qDebug().noquote() << crdt->to_string();
}

void Editor::on_change(int line, int index, const Symbol& s) {
    qDebug() << "ON_CHANGE";
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
    cursor.setPosition(block.position() + index);

    // save old format to restore it later
    QTextCharFormat oldFormat = ui->textEdit->currentCharFormat();
    QTextCharFormat newFormat = s.getQTextCharFormat();
//    cursor.setCharFormat(newFormat);
//    cursor.insertText(QString(1, s.getValue()));
    QTextCursor tempCursor = cursor;
    tempCursor.setPosition(block.position() + index);
    tempCursor.setPosition(block.position() + index + 1, QTextCursor::KeepAnchor);
    tempCursor.setCharFormat(newFormat);
    ui->textEdit->setCurrentCharFormat(oldFormat);

    qDebug().noquote() << crdt->to_string();
}

//da cambiare
void Editor::updateText(const QString& text){
    qDebug() << "Update text!";
    ui->listWidget->clear();
//    this->ui->listWidget->addItem(new QListWidgetItem(QIcon(*client->getProfile()),client->getUsername()));

    QListWidgetItem *item = new QListWidgetItem;
    item->setIcon(*client->getProfile());
    item->setText(client->getUsername());
    qDebug() << "color: " << client->getColor();
    item->setTextColor(QColor(client->getColor()));
    this->ui->listWidget->addItem(item);
    this->ui->textEdit->setText(text);
}

void Editor::addUsers(const QList<QPair<QString,QString>> users){

    for (int i=0;i<users.count();i++){
        highlighter->addClient(fromStringToIntegerHash(users.at(i).first),list_colors.getColor()); // for now all red
        this->ui->listWidget->addItem(new QListWidgetItem(QIcon(*client->getProfile()),users.at(i).first));

    }//per ora è visualizzato l'username per faciliatare la cancellazione senza riferimenti alla riga
}

void Editor::clear(){
    ui->listWidget->clear();
    ui->textEdit->clear();
    list_colors.clear();
}

void Editor::removeUser(const QString& name){
   qDebug()<<"Here remove"<<endl;
   QListWidgetItem* item = this->ui->listWidget->findItems(name,Qt::MatchFixedString).first();
   qDebug()<<"ITEM TO REMOVE: "<<item->text();
   this->ui->listWidget->removeItemWidget(item);
   delete item;
}

void Editor::saveCursorPosition()
{
    // update alignment icon
    alignmentChanged(ui->textEdit->alignment());

    // save cursor position
    QTextCursor cursor = ui->textEdit->textCursor();
    this->line = cursor.blockNumber();
    this->index = cursor.positionInBlock();

    // use positon of symbol AFTER cursor as reference
    qDebug() << "cursor position before" << this->line << this->index;
    //correct_position(this->line, this->index);
    crdt->cursorPositionChanged(this->line, this->index);

    // select format icon of the first char before the remote cursor
    int pos = cursor.position();
    while (pos >= 0) {
        bool not_found = true;
        for (RemoteCursor *c : ui->textEdit->remote_cursors.values()) {
//            qDebug() << "pos" <<  c->getPosition() << pos;
            if (c->getPosition() == pos)
                not_found = false;
        }
        if (not_found)
            break;
        pos--;
    }
    cursor.setPosition(pos);
    ui->textEdit->setCurrentCharFormat(cursor.charFormat());
}

void Editor::showEvent(QShowEvent *)
{
    this->setWindowTitle(client->getOpenedFile() + " - Shared Editor");
    moveCursorToEnd();
}

// update icons in toolbar (italic, bold, ...) depending on the char before cursor
void Editor::on_currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
    void on_remoteCursor(int editor_id, Symbol s);

//    qDebug() << "***FORMAT CHANGED***";
//    qDebug() << "changed" << ui->textEdit->textCursor().selectedText();
//    QString changed = ui->textEdit->textCursor().selectedText();
//    if (changed.isEmpty()) {
//        qDebug() << "nothing to do";
//        return;
//    }
////    int a = ui->textEdit->textCursor().selectionStart();
////    ui->textEdit->textCursor().selection().
////    this->line = cursor.blockNumber();
////    this->index = cursor.positionInBlock();
//    // change multiple chars
////    for (int i = 0; i < changed.length(); i++) {
////        qDebug() << "Changed " << changed.at(i) << "in position (" << this->line << "," << this->index << ")";
//////        crdt->localErase(line, index);
////    }

//    int start = ui->textEdit->textCursor().selectionStart();
//    int end = ui->textEdit->textCursor().selectionEnd();
//    qDebug() << "start/end selection" << start << end;
//    QTextCursor cursor = ui->textEdit->textCursor();
//    for (int i = start; i < end; i++) {
//        cursor.setPosition(i);
//        int line = cursor.blockNumber();
//        int index = cursor.positionInBlock();
//        qDebug() << "line/index/char" << line << index << changed.at(i - start) << QChar(0x2029);
////        qDebug() << "aaa" << (changed.at(i - start).toInt()=='\u2029');
//        QString a =  changed.at(i - start);
//        QChar c = a.data()[0];
//        if (changed.at(i - start) == QChar(0x2029))
//            qDebug() << "NEWLINE";
//    }
////    QTextCursor cursor = ui->textEdit->textCursor();
////    int start = cursor.selectionStart();
////    int end = cursor.selectionEnd();

////    cursor.setPosition(end, QTextCursor::KeepAnchor);
////    QTextBlock endBlock = cursor.block();
////    cursor.setPosition(start, QTextCursor::KeepAnchor);
////    QTextBlock block = cursor.block();
}

void Editor::textColor()
{
    QColor col = QColorDialog::getColor(ui->textEdit->textColor(), this);
    if (!col.isValid())
        return;
    ui->textEdit->setTextColor(col);
    on_formatChange();
}

void Editor::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        ui->actionPaste->setEnabled(md->hasText());
#endif
}

void Editor::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    ui->actionBold->setChecked(f.bold());
    ui->actionItalic->setChecked(f.italic());
    ui->actionUnderline->setChecked(f.underline());
}

void Editor::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void Editor::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
}

void Editor::textFamily(const QString &f)
{
    ui->textEdit->setFontFamily(f);
    on_formatChange();
}

void Editor::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        ui->textEdit->setFontPointSize(pointSize);
    }
    on_formatChange();
}

void Editor::moveCursorToEnd() {
    // move the cursor to end of the text
    QTextCursor cursor(ui->textEdit->document());
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
}

void Editor::on_formatChange() {
    qDebug() << "CHANGED" << ui->textEdit->textCursor().selectedText();
    QString changed = ui->textEdit->textCursor().selectedText();

    int start = ui->textEdit->textCursor().selectionStart();
    int end = ui->textEdit->textCursor().selectionEnd();
    qDebug() << "start/end selection" << start << end;
    QTextCursor cursor = ui->textEdit->textCursor();
    for (int i = start; i < end; i++) {
        cursor.setPosition(i);
        int line = cursor.blockNumber();
        int index = cursor.positionInBlock();
        qDebug() << "line/index/char" << line << index << changed.at(i - start);

        // if newline ('\n') do nothing
        if (changed.at(i - start) == QChar(0x2029)) {
//            qDebug() << "NEWLINE";
            continue;
        }

        // position AFTER the char to read its format
        cursor.setPosition(i + 1);
        QFont font = cursor.charFormat().font();
        crdt->localChange(line, index, font, cursor.charFormat().foreground().color());
    }
}

void Editor::on_addCRDTterminator() {
    QFont font;
    QColor color;
    this->crdt->localInsert(0, 0, '\0', font, color);
}

void Editor::on_remoteCursor(int editor_id, Symbol s) {
    int line, index;
    qDebug() << QChar(s.getValue());
    crdt->getPositionFromSymbol(s, line, index);
    qDebug() << "LINE/INDEX" << line << index;
   // disconnect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);
   // disconnect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &Editor::saveCursorPosition);

    QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
    if (!ui->textEdit->remote_cursors.contains(editor_id)) {
        // TODO: get the color instead of using red
        qDebug()<<"add new cursor: "<<editor_id;
        RemoteCursor *remote_cursor = new RemoteCursor(ui->textEdit->textCursor(), block, index, highlighter->getColor(editor_id));
        ui->textEdit->remote_cursors.insert(editor_id, remote_cursor);
    } else {
        RemoteCursor *remote_cursor = ui->textEdit->remote_cursors.value(editor_id);
        remote_cursor->moveTo(block, index);
    }

    //connect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);
    //connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &Editor::saveCursorPosition);
}

/*
void Editor::correct_position(int& line, int& index) {
    for (RemoteCursor *cursor : ui->textEdit->remote_cursors) {
        int cline, cindex;
        cursor->getPosition(cline, cindex);
        // TODO: less or less/equal? <= seems to work
        if (line == cline && cindex <= index)
            index--;
    }
}
*/

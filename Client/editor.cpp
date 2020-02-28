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

    connect(ui->actionSharedLink, &QAction::triggered, this, &Editor::sharedLink);

    crdt = new CRDT(fromStringToIntegerHash(client->getUsername()), client);
    connect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);
    connect(crdt, &CRDT::insert, this, &Editor::on_insert);
    connect(crdt, &CRDT::erase, this, &Editor::on_erase);
    connect(crdt, &CRDT::change, this, &Editor::on_change);
    connect(crdt, &CRDT::changeAlignment, this, &Editor::on_changeAlignment);

    connect(client, &Client::moveCursorToEnd, this, &Editor::on_moveCursorToEnd);
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
    const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(":images/textleft.png"));
    actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(":images/textcenter.png"));
    actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(":images/textright.png"));
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

    //
//    connect(ui->actionFont, &QAction::triggered, this, &Editor::on_formatChange);
//    connect(ui->actionBold, &QAction::triggered, this, &Editor::on_formatChange);
//    connect(ui->actionUnderline, &QAction::triggered, this, &Editor::on_formatChange);
//    connect(comboFont, &QFontComboBox::currentFontChanged, this, &Editor::on_formatChange);
//    connect(comboSize, &QComboBox::, this, &Editor::on_formatChange);
//    connect(actionTextColor, &QAction::triggered, this, &Editor::on_formatChange);
      connect(this, &Editor::formatChange, this, &Editor::on_formatChange);

}

int Editor::fromStringToIntegerHash(QString str) {
    auto hash = QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5);
    QDataStream data(hash);
    int intHash;
    data >> intHash;
    return intHash;
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
    ui->textEdit->clear();
    // create new CRDT with connections
    delete crdt;
    crdt = new CRDT(fromStringToIntegerHash(client->getUsername()), client);
    connect(crdt, &CRDT::insert, this, &Editor::on_insert);
    connect(crdt, &CRDT::erase, this, &Editor::on_erase);
    connect(crdt, &CRDT::change, this, &Editor::on_change);
    // ... and then riconnect (because we want to remove chars locally without deleteling them in server)
    connect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);

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
    emit formatChange();
}

void Editor::setFontItalic(bool italic)
{
    ui->textEdit->setFontItalic(italic);
    emit formatChange();

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
    emit formatChange();
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
            crdt->localChangeAlignment(i,LEFT);
        }
        else if (a == actionAlignCenter){
            ui->textEdit->setAlignment(Qt::AlignHCenter);
            crdt->localChangeAlignment(i,MIDDLE);
        }
        else if (a == actionAlignRight){
            ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
            crdt->localChangeAlignment(i,RIGHT);
        }
    }
}

/****************************************************
    LOCAL OPERATION: update textedit THEN crdt
    REMOTE OPERATION: update crdt THEN textedit
****************************************************/
void Editor::on_contentsChange(int position, int charsRemoved, int charsAdded) {    
    qDebug() << "total text size" << ui->textEdit->toPlainText().size() << "crdt size" << crdt->getSize();
    qDebug() << "added" << charsAdded << "removed" << charsRemoved;
    qDebug() << "line" << this->line << "index" << this->index;

    // REMOTE OPERATION: insert/delete received from remote client
    // nothing to update
    // "charsRemoved == 0" and "charsAdded == 0" are conditions added to handle QTextDocument::contentsChange bug QTBUG-3495
    if ((charsAdded > 0 && charsRemoved == 0 && ui->textEdit->toPlainText().size() <= crdt->getSize())
            || (charsRemoved > 0 && charsAdded == 0 && ui->textEdit->toPlainText().size() >= crdt->getSize())) {
        return;
    }

    // LOCAL OPERATION: insert/deleted performed in this editor
    // update CRDT structure
    // "charsAdded - charsRemoved" and "charsRemoved - charsAdded" are conditions added to handle QTextDocument::contentsChange bug QTBUG-3495
    if (charsAdded > 0 && charsAdded - charsRemoved > 0) {
        QString added = ui->textEdit->toPlainText().mid(position, charsAdded - charsRemoved);

        // add multiple chars
        QTextCursor cursor = ui->textEdit->textCursor();
        qDebug() << "position" << cursor.position();
        for (int i = 0; i < charsAdded - charsRemoved; i++) {
            // move cursor after each char inserted, starting from the first one
            cursor.setPosition(ui->textEdit->textCursor().position() - (charsAdded - charsRemoved) + 1 + i);
            QFont font = cursor.charFormat().font();
//            qDebug() << font.bold() << font.italic() << font.underline();
            qDebug() << "Added " << added.at(i) << "in position (" << this->line << "," << this->index + i << ")";
            crdt->localInsert(line, index + i, added.at(i).toLatin1(), font, cursor.charFormat().foreground().color());

            cursor.movePosition(QTextCursor::Left);
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
    if (align==LEFT)
        textBlockFormat.setAlignment(Qt::AlignLeft);
    else if (align==MIDDLE)
        textBlockFormat.setAlignment(Qt::AlignCenter);
    else if (align==RIGHT)
        textBlockFormat.setAlignment(Qt::AlignRight);
    cursor.mergeBlockFormat(textBlockFormat);

    qDebug().noquote() << crdt->to_string();
}


void Editor::on_insert(int line, int index, const Symbol& s)
{
    qDebug() << "ON_INSERT";
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
    cursor.insertText(QString(1, s.getValue()));
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
        this->ui->listWidget->addItem(new QListWidgetItem(QIcon(*client->getProfile()),users.at(i).first));

    }//per ora è visualizzato l'username per faciliatare la cancellazione senza riferimenti alla riga
}

void Editor::clear(){
    ui->listWidget->clear();
    ui->textEdit->clear();
}

void Editor::removeUser(const QString& name){
//    qDebug()<<"Here"<<endl;

    // this->ui->listWidget->removeItemWidget(this->uformattazionei->listWidget->findItems(name,Qt::MatchFixedString).first());
}

void Editor::saveCursorPosition()
{
    // update alignment icon
    alignmentChanged(ui->textEdit->alignment());

    // save cursor position
    QTextCursor cursor = ui->textEdit->textCursor();
    this->line = cursor.blockNumber();
    this->index = cursor.positionInBlock();
//    qDebug() << "X: " << cursor.blockNumber() << ", Y: " << cursor.positionInBlock();
}

void Editor::showEvent(QShowEvent *)
{
    this->setWindowTitle(client->getOpenedFile() + " - Shared Editor");
}

// update icons in toolbar (italic, bold, ...) depending on the char before cursor
void Editor::on_currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());

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
}

void Editor::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        ui->textEdit->setFontPointSize(pointSize);
    }
}

void Editor::on_moveCursorToEnd() {
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

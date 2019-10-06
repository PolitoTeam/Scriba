#include <QFontDialog>
#include "editor.h"
#include "ui_editor.h"
#include "client.h"
#include <QDateTime>
#include <QIcon>
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

    connect(ui->actionPrint, &QAction::triggered, this, &Editor::print);
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


    // TODO: create/load new crdt for every file created/opened; here just to test
    crdt = new CRDT(QDateTime::currentMSecsSinceEpoch(), client);
    connect(ui->textEdit->document(), &QTextDocument::contentsChange, this, &Editor::on_contentsChange);
    connect(crdt, &CRDT::insert, this, &Editor::on_insert);
    connect(crdt, &CRDT::erase, this, &Editor::on_erase);

}

Editor::~Editor()
{
    delete ui;
}

void Editor::textChange(){
    qDebug()<<"Position: "<<ui->textEdit->textCursor().position()<<endl;
}

void Editor::setClient(Client *client){
    this->client=client;
}

void Editor::print()
{
//#if QT_CONFIG(printer)
//    QPrinter printDev;
//#if QT_CONFIG(printdialog)
//    QPrintDialog dialog(&printDev, this);
//    if (dialog.exec() == QDialog::Rejected)
//        return;
//#endif // QT_CONFIG(printdialog)
//    ui->textEdit->print(&printDev);
//#endif // QT_CONFIG(printer)
}

void Editor::exit()
{
    client->closeFile();
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
}

void Editor::setFontItalic(bool italic)
{
    ui->textEdit->setFontItalic(italic);


}

void Editor::setFontBold(bool bold)
{
    bold ? ui->textEdit->setFontWeight(QFont::Bold) :
           ui->textEdit->setFontWeight(QFont::Normal);
}

//void Editor::setCRDT(CRDT *crdt) {
//    this->crdt = crdt;
//}

void Editor::on_contentsChange(int position, int charsRemoved, int charsAdded) {
    // if symbol received from remote (not entered by client), returns without doing anything
    if ((charsAdded > 0 && ui->textEdit->toPlainText().size() <= crdt->getSize())
            || (charsRemoved > 0 && ui->textEdit->toPlainText().size() >= crdt->getSize()))
        return;

    if (charsAdded > 0) {
        QString added = ui->textEdit->toPlainText().mid(position,charsAdded);
        qDebug() << "Added " << added << " in position " << position;

//        if (added != "")
            crdt->localInsert(position, added.at(0).toLatin1());

    } else if (charsRemoved > 0) {
        ui->textEdit->undo();
        QString removed = ui->textEdit->document()->toPlainText().mid(position, charsRemoved);
        qDebug() << "Removed " << removed << " in position " << position;
        ui->textEdit->redo();

        crdt->localErase(position);
    }
}

void Editor::on_insert(int index, char value)
{
//    qDebug() << "ON INSERT " << index << " " << QString(1, value);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(index);
    cursor.insertText(QString(1, value));

    qDebug() << crdt->to_string();
}

void Editor::on_erase(int index)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(index);
    cursor.deleteChar();

    qDebug() << crdt->to_string();
}

//da cambiare
void Editor::updateText(const QString& text){
    ui->listWidget->clear();
    this->ui->listWidget->addItem(new QListWidgetItem(QIcon(*client->getProfile()),client->getUsername()));
    this->ui->textEdit->setText(text);

}

void Editor::addUsers(const QList<QPair<QString,QString>> users){

    for (int i=0;i<users.count();i++)
        this->ui->listWidget->addItem(new QListWidgetItem(QIcon(*client->getProfile()),users.at(i).first)); //per ora è visualizzato l'username per faciliatare la cancellazione senza riferimenti alla riga
}

void Editor::clear(){
    ui->listWidget->clear();
    ui->textEdit->clear();
}

void Editor::removeUser(const QString& name){
    qDebug()<<"Here"<<endl;

   this->ui->listWidget->removeItemWidget(this->ui->listWidget->findItems(name,Qt::MatchFixedString).first());
}



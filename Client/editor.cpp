#include <QFontDialog>
#include "editor.h"
#include "ui_editor.h"
#include "client.h"

Editor::Editor(QWidget *parent,Client* client) :
    QWidget(parent),
    ui(new Ui::Editor),
    client(client)
{
    ui->setupUi(this);
   // this->setCentralWidget(ui->textEdit);
    QTextCursor cursor(ui->textEdit->textCursor()); //cursore

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
}

Editor::~Editor()
{
    delete ui;
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

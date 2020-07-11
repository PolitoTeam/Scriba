#include <QClipboard>
#include <QCryptographicHash>
#include <QDateTime>
#include <QFontDialog>
#include <QIcon>
#include <QMessageBox>
#include <QPainter>
#include <QTextBlock>
#include <QTimer>
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
#include "client.h"
#include "editor.h"
#include "ui_editor.h"

Editor::Editor(QWidget *parent, Client *client)
    : QMainWindow(parent), ui(new Ui::Editor), client(client) {
  ui->setupUi(this);
  this->popUp = new QMessageBox(this);

  connect(ui->actionPrint, &QAction::triggered, this, &Editor::printPdf);
  connect(ui->actionExit, &QAction::triggered, this, &Editor::exit);
  connect(ui->actionCopy, &QAction::triggered, this, &Editor::copy);
  connect(ui->actionCut, &QAction::triggered, this, &Editor::cut);
  connect(ui->actionPaste, &QAction::triggered, this, &Editor::paste);
  // connect(ui->actionUndo, &QAction::triggered, this, &Editor::undo);
  connect(ui->actionRedo, &QAction::triggered, this, &Editor::redo);
  connect(ui->actionFont, &QAction::triggered, this, &Editor::selectFont);
  connect(ui->actionBold, &QAction::triggered, this, &Editor::setFontBold);
  connect(ui->actionUnderline, &QAction::triggered, this,
          &Editor::setFontUnderline);
  connect(ui->actionItalic, &QAction::triggered, this, &Editor::setFontItalic);
  connect(client, &Client::usersConnectedReceived, this, &Editor::addUsers);
  connect(client, &Client::contentReceived, this, &Editor::updateText);
  connect(ui->textEdit, &QTextEdit::textChanged, this, &Editor::textChange);
  connect(client, &Client::userDisconnected, this, &Editor::removeUser);
  connect(client, &Client::addCRDTterminator, this,
          &Editor::on_addCRDTterminator);
  connect(client, &Client::remoteCursor, this, &Editor::on_remoteCursor);
  connect(ui->actionSharedLink, &QAction::triggered, this, &Editor::sharedLink);
  undoFlag = false;
  crdt = new CRDT(client);
  highlighter = new Highlighter(0, crdt);

  connect(this->client, &Client::loggedIn, this, [this] {
    int site_id = fromStringToIntegerHash(this->client->getUsername());
    this->crdt->setId(site_id);
    this->highlighter->addLocal(site_id);
  });

  connect(ui->textEdit->document(), &QTextDocument::contentsChange, this,
          &Editor::on_contentsChange);
  connect(crdt, &CRDT::insert, this, &Editor::on_insert);
  connect(crdt, &CRDT::insertGroup, this, &Editor::on_insertGroup);
  connect(crdt, &CRDT::erase, this, &Editor::on_erase);
  connect(crdt, &CRDT::change, this, &Editor::on_change);
  connect(crdt, &CRDT::changeAlignment, this, &Editor::on_changeAlignment);

  connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this,
          &Editor::saveCursorPosition);
  connect(ui->textEdit, &QTextEdit::currentCharFormatChanged, this,
          &Editor::on_currentCharFormatChanged);

  // ADD font, size and color to toolbar (cannot be otherwise achieved using Qt
  // creator GUI):
  // 1. font
  comboFont = new QFontComboBox(ui->toolBar);
  ui->toolBar->addWidget(comboFont);
  connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated),
          this, &Editor::textFamily);
  comboFont->setCurrentFont(QFont("American Typewriter"));
  // this->textFamily("American Typewriter");

  // 2. size
  comboSize = new QComboBox(ui->toolBar);
  ui->toolBar->addWidget(comboSize);

  QList<int> standardSizes = QFontDatabase::standardSizes();
  standardSizes.append(15);
  std::sort(standardSizes.begin(), standardSizes.end());
  foreach (int size, standardSizes)
    comboSize->addItem(QString::number(size));

  comboSize->setCurrentIndex(
      standardSizes.indexOf(15));

  connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated),
          this, &Editor::textSize);

  // 3. color
  QPixmap pix(16, 16);
  pix.fill(Qt::black);
  actionTextColor =
      ui->toolBar->addAction(pix, tr("&Color..."), this, &Editor::textColor);

  // undo/redo config
  connect(ui->textEdit->document(), &QTextDocument::undoAvailable,
          ui->actionUndo, &QAction::setEnabled);
  connect(ui->textEdit->document(), &QTextDocument::redoAvailable,
          ui->actionRedo, &QAction::setEnabled);
  ui->actionUndo->setEnabled(ui->textEdit->document()->isUndoAvailable());
  ui->actionRedo->setEnabled(ui->textEdit->document()->isRedoAvailable());
  connect(ui->textEdit, &MyQTextEdit::undo,
          []() { qDebug() << "Received UNDO"; });
  connect(ui->textEdit, &MyQTextEdit::undo, this, &Editor::undo);

  connect(ui->textEdit, &MyQTextEdit::redo,
          []() { qDebug() << "Received REDO"; });
  connect(ui->textEdit, &MyQTextEdit::redo, this, &Editor::redo);

  // copy/paste/cut config
#ifndef QT_NO_CLIPBOARD
  ui->actionCut->setEnabled(false);
  connect(ui->textEdit, &QTextEdit::copyAvailable, ui->actionCut,
          &QAction::setEnabled);
  ui->actionCopy->setEnabled(false);
  connect(ui->textEdit, &QTextEdit::copyAvailable, ui->actionCopy,
          &QAction::setEnabled);
  connect(QApplication::clipboard(), &QClipboard::dataChanged, this,
          &Editor::clipboardDataChanged);
#endif

  // add alignment icons
  const QIcon leftIcon =
      QIcon::fromTheme("format-justify-left", QIcon(":/images/textleft.png"));
  actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
  actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
  actionAlignLeft->setCheckable(true);
  actionAlignLeft->setPriority(QAction::LowPriority);
  const QIcon centerIcon = QIcon::fromTheme("format-justify-center",
                                            QIcon(":/images/textcenter.png"));
  actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
  actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
  actionAlignCenter->setCheckable(true);
  actionAlignCenter->setPriority(QAction::LowPriority);
  const QIcon rightIcon =
      QIcon::fromTheme("format-justify-right", QIcon(":/images/textright.png"));
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
  connect(ui->textEdit, &MyQTextEdit::resetDefaultAlignment, actionAlignLeft,
          &QAction::trigger);
  // connect(ui->textEdit,&MyQTextEdit::resetDefaultAlignment,actionAlignLeft,&QAction::trigger);

  // show assigned text
  const QIcon assigned =
      QIcon::fromTheme("Cursor", QIcon(":/images/cursor.png"));
  actionShowAssigned = new QAction(assigned, tr("Cursor"), this);
  actionShowAssigned->setCheckable(true);
  actionShowAssigned->setChecked(false);
  ui->toolBar->addAction(actionShowAssigned);
  connect(actionShowAssigned, &QAction::triggered, this,
          &Editor::on_showAssigned);

  //
  //    connect(ui->actionFont, &QAction::triggered, this,
  //    &Editor::on_formatChange); connect(ui->actionBold, &QAction::triggered,
  //    this, &Editor::on_formatChange); connect(ui->actionUnderline,
  //    &QAction::triggered, this, &Editor::on_formatChange); connect(comboFont,
  //    &QFontComboBox::currentFontChanged, this, &Editor::on_formatChange);
  //    connect(comboSize, &QComboBox::, this, &Editor::on_formatChange);
  //    connect(actionTextColor, &QAction::triggered, this,
  //    &Editor::on_formatChange);
  //      connect(this, &Editor::formatChange, this, &Editor::on_formatChange);

  ui->textEdit->setLine(&line);
  ui->textEdit->setIndex(&index);
}

int Editor::fromStringToIntegerHash(QString str) {
  // qDebug()<<"Hash of: "<<str;
  auto hash = QCryptographicHash::hash(str.toLatin1(), QCryptographicHash::Md5);
  QDataStream data(hash);
  // qDebug()<< "String to hash: "<<hash;
  // qDebug()<< " type of hash: "<<typeid(hash).name();
  int intHash;
  data >> intHash;
  return intHash;
}

void Editor::on_showAssigned() {
  if (this->highlighter->document() == 0) {
    // qDebug()<< "Assigning file";
    this->highlighter->setDocument(ui->textEdit->document());
    // qDebug()<< "Assigned file";
  } else {
    this->highlighter->setDocument(0);
    // qDebug()<< "Removing file";
  }
}

Editor::~Editor() { delete ui; }

void Editor::textChange() {
  //    //qDebug()<<"Cursor position:
  //    "<<ui->textEdit->textCursor().position()<<endl;
}

void Editor::printPdf() {
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
  statusBar()->showMessage(
      tr("Exported \"%1\"").arg(QDir::toNativeSeparators(fileName)));
  //! [0]
#endif
}
void Editor::exit() {
  this->clear(false);

  //  crdt = new CRDT(client);
  crdt->setId(fromStringToIntegerHash(client->getUsername()));
  // this->highlighter->setCRDT(crdt);
  this->highlighter->addLocal(fromStringToIntegerHash(client->getUsername()));


  // doppio controllo
  if ( actionShowAssigned->isChecked()){
      actionShowAssigned->trigger();
  }else if (this->highlighter->document() != 0) {
        this->highlighter->setDocument(0);
  }

  emit changeWidget(HOME);
}

void Editor::closeEvent(QCloseEvent *) { this->clear(false); }

void Editor::peerYou() {
  QListWidgetItem *item = new QListWidgetItem();
  QPixmap orig = *client->getProfile();
  // getting size if the original picture is not square
  int size = qMin(orig.width(), orig.height());
  // creating a new transparent pixmap with equal sides
  // creating circle clip area
  QPixmap rounded = QPixmap(size, size);
  rounded.fill(Qt::transparent);
  QPainterPath path;
  path.addEllipse(rounded.rect());
  QPainter painter(&rounded);
  painter.setClipPath(path);
  // filling rounded area if needed
  painter.fillRect(rounded.rect(), Qt::black);
  // getting offsets if the original picture is not square
  int x = qAbs(orig.width() - size) / 2;
  int y = qAbs(orig.height() - size) / 2;
  painter.drawPixmap(-x, -y, orig.width(), orig.height(), orig);

  QPixmap background = QPixmap(size + 50, size + 50);
  background.fill(Qt::transparent);
  QPainterPath path1;
  path1.addEllipse(background.rect());
  QPainter painter1(&background);
  painter1.setClipPath(path1);
  // filling rounded area if needed
  painter1.fillRect(background.rect(), QColor(0,	136,	86));
  // getting offsets if the original picture is not square
  x = qAbs(rounded.width() - size - 50) / 2;
  y = qAbs(rounded.height() - size - 50) / 2;
  painter1.drawPixmap(x, y, rounded.width(), rounded.height(), rounded);
  item->setIcon(QIcon(background));
  item->setText(this->client->getNickname() + " (You)");
  item->setData(Qt::UserRole, this->client->getUsername());
  // item->setTextAlignment(Qt::AlignHCenter);
  // item->setBackground( QColor(124,252,0,127) );
  item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
  item->setWhatsThis(this->client->getUsername());
  this->ui->listWidget->addItem(item);
}

void Editor::copy() {
#if QT_CONFIG(clipboard)
  ui->textEdit->copy();
#endif
}

void Editor::cut() {
#if QT_CONFIG(clipboard)
  ui->textEdit->cut();
#endif
}

void Editor::paste() {
#if QT_CONFIG(clipboard)
  ui->textEdit->paste();
#endif
}

void Editor::undo() {
  // qDebug()<<"undo()";
  this->undoFlag = true;
  ui->textEdit->document()
      ->undo(); // the change due to the insert/delete are automatically managed
                // by on_contents_change
  // update alignment icon
  alignmentChanged(ui->textEdit->alignment());
}

void Editor::redo() {
  // qDebug()<<"redo";
  this->redoFlag = true;
  ui->textEdit->document()->redo();
  // update alignment icon
  alignmentChanged(ui->textEdit->alignment());
}

void Editor::selectFont() {
  bool fontSelected;
  QFont font = QFontDialog::getFont(&fontSelected, this);
  if (fontSelected)
    ui->textEdit->setFont(font);
}

void Editor::setFontUnderline(bool underline) {
  ui->textEdit->setFontUnderline(underline);
  on_formatChange();
}

void Editor::setFontItalic(bool italic) {
  ui->textEdit->setFontItalic(italic);
  on_formatChange();
}

void Editor::sharedLink() {
  QClipboard *clipboard = QGuiApplication::clipboard();
  clipboard->setText(client->getSharedLink());

  // Show popup for 1 second
  this->popUp->setText("Link copied to clipboard.");
  this->popUp->setWindowTitle("Shared Link");
  this->popUp->setStandardButtons(this->popUp->NoButton);
  this->popUp->setModal(false);
  QTimer::singleShot(1000, this->popUp, &QMessageBox::hide); // 1000 ms
  this->popUp->show();
}

void Editor::setFontBold(bool bold) {
  bold ? ui->textEdit->setFontWeight(QFont::Bold)
       : ui->textEdit->setFontWeight(QFont::Normal);
  on_formatChange();
}

void Editor::textAlign(QAction *a) {
  QString changed = ui->textEdit->textCursor().selectedText();
  QTextCursor cursor = ui->textEdit->textCursor();
  int start = ui->textEdit->textCursor().selectionStart();
  cursor.setPosition(start);
  int line_start = cursor.blockNumber();
  // qDebug()<<"START POSITION: "<< line_start;
  int end = ui->textEdit->textCursor().selectionEnd();
  cursor.setPosition(end);
  int line_end = cursor.blockNumber();
  // qDebug()<<"END POSITION: "<< line_end;

  QTextBlockFormat n;
  SymbolFormat::Alignment sf;
  if (a == actionAlignLeft) {
    sf = SymbolFormat::Alignment::ALIGN_LEFT;
    n.setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
  } else if (a == actionAlignRight) {
    sf = SymbolFormat::Alignment::ALIGN_RIGHT;
    n.setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
  } else if (a == actionAlignCenter) {
    sf = SymbolFormat::Alignment::ALIGN_CENTER;
    n.setAlignment(Qt::AlignHCenter);
  }

  for (int i = line_start; i <= line_end; i++)
    crdt->localChangeAlignment(i, sf);

  ui->textEdit->textCursor().mergeBlockFormat(n);
}

SymbolFormat::Alignment Editor::alignmentConversion(Qt::Alignment a) {

  if (a == (Qt::AlignLeft | Qt::AlignLeading) ||
      a == (Qt::AlignLeading | Qt::AlignAbsolute)) {
    // qDebug()<<"Convertsion: "<<a<<" -> SymbolFormat::Alignment::ALIGN_LEFT";
    return SymbolFormat::Alignment::ALIGN_LEFT;
  }

  else if (a == Qt::AlignCenter || a == Qt::AlignHCenter) {
    // qDebug()<<"Convertsion: "<<a<<" ->
    // SymbolFormat::Alignment::ALIGN_CENTER";
    return SymbolFormat::Alignment::ALIGN_CENTER;
  }

  else if (a == Qt::AlignRight || a == Qt::AlignTrailing | Qt::AlignAbsolute) {
    // qDebug()<<"Convertsion: "<<a<<" -> SymbolFormat::Alignment::ALIGN_RIGHT";
    return SymbolFormat::Alignment::ALIGN_RIGHT;
  }
}

Qt::Alignment Editor::alignmentConversion(SymbolFormat::Alignment a) {

  if (a == SymbolFormat::Alignment::ALIGN_LEFT) {
    // qDebug()<<"Conversion: "<<a<<" -> "<< "Qt::AlignLeft|Qt::AlignLeading";
    return (Qt::AlignLeft | Qt::AlignLeading);
  }

  if (a == SymbolFormat::Alignment::ALIGN_CENTER) {
    // qDebug()<<"Conversion: "<<a<<" -> "<< "Qt::AlignHCenter";
    return Qt::AlignHCenter;
  }

  if (a == SymbolFormat::Alignment::ALIGN_RIGHT) {
    // qDebug()<<"Conversion: "<<a<<" -> "<<
    // "Qt::AlignTrailing|Qt::AlignAbsolute";
    return Qt::AlignTrailing | Qt::AlignAbsolute;
  }
}

/****************************************************
        LOCAL OPERATION: update textedit THEN crdt
        REMOTE OPERATION: update crdt THEN textedit
****************************************************/
void Editor::on_contentsChange(int position, int charsRemoved, int charsAdded) {
  // qDebug() << "[total text size" << ui->textEdit->toPlainText().size() <<
  // "crdt size" << crdt->getSize(); qDebug() << "added" << charsAdded <<
  // "removed" << charsRemoved; qDebug() << "line" << this->line << "index" <<
  // this->index; qDebug() << "position" << position <<"]";

  // REMOTE OPERATION: insert/delete received from remote client
  // nothing to update
  // "charsRemoved == 0" and "charsAdded == 0" are conditions added to handle
  // QTextDocument::contentsChange bug QTBUG-3495
  /* if ((charsAdded > 0 && charsRemoved == 0 &&
  ui->textEdit->toPlainText().size() - ui->textEdit->remote_cursors.size() <=
  crdt->getSize())
                  || (charsRemoved > 0 && charsAdded == 0 &&
  ui->textEdit->toPlainText().size() - ui->textEdit->remote_cursors.size() >=
  crdt->getSize())) {
          //qDebug() << "rem";
          return;
  } */
  // qDebug()<<"text edit size: "<<ui->textEdit->toPlainText().size();
  // qDebug()<<"text edit: "<<ui->textEdit->toPlainText();
  // qDebug()<<"crdt size: "<<crdt->getSize();
  if (((charsAdded - charsRemoved) > 0 &&
       ui->textEdit->toPlainText().size() <= crdt->getSize()) ||
      ((charsRemoved - charsAdded) > 0 &&
       ui->textEdit->toPlainText().size() >= crdt->getSize())) {
    return;
  }

  // LOCAL OPERATION: insert/deleted performed in this editor
  // update CRDT structure
  // "charsAdded - charsRemoved" and "charsRemoved - charsAdded" are conditions
  // added to handle QTextDocument::contentsChange bug QTBUG-3495
  if (charsAdded > 0 && charsAdded - charsRemoved > 0) {
    this->undoFlag = false;
    QString added =
        ui->textEdit->toPlainText().mid(position, charsAdded - charsRemoved);
    if (added.at(0) == '\0')
      return;
    // move cursor before first char to insert
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(position);
    // single character
    int line = cursor.blockNumber();
    int index = cursor.positionInBlock();
    if ((charsAdded - charsRemoved) == 1) {

      // qDebug() << "Added " << added.at(0) << "in position (" << line << ","
      // << index << ")";

      // to retrieve the format it is necessary to be on the RIGHT of the target
      // char
      cursor.movePosition(QTextCursor::Right);
      QFont font = cursor.charFormat().font();
      ui->textEdit->update();
      crdt->localInsert(line, index, added.at(0).unicode(), font,
                        cursor.charFormat().foreground().color(),
                        getCurrentAlignment());
    } else {
      this->undoFlag = false;
      QFont fontPrec;
      QColor colorPrec;
      QString partial;
      QFont font;
      QColor color;
      Qt::Alignment align;
      Qt::Alignment alignPrec = ui->textEdit->document()
                                    ->findBlockByNumber(line)
                                    .blockFormat()
                                    .alignment();
      int linePrec = line;
      int numLines = line;
      // add multiple chars
      // qDebug() << "Multiple chars: position" << cursor.position()<<"
      // (line,index): ( "<<line<<","<< index<<")";
      for (int i = 0; i < charsAdded - charsRemoved; i++) {

        // int line = cursor.blockNumber();
        // int index = cursor.positionInBlock();

        // to retrieve the format it is necessary to be on the RIGHT of the
        // target char
        cursor.movePosition(QTextCursor::Right);
        // qDebug()<<"Added at: "<<i<<" -> "<<added.at(i).unicode();
        font = cursor.charFormat().font();
        color = cursor.charFormat().foreground().color();
        align = alignPrec;
        if (i == 0) {
          fontPrec = font;
          colorPrec = color;
        }

        if (numLines != linePrec) {
          QTextBlock block =
              ui->textEdit->document()->findBlockByNumber(numLines);
          QTextBlockFormat textBlockFormat = block.blockFormat();
          align = textBlockFormat.alignment();
          // qDebug()<<"line: "<<numLines<<" alignment: "<<align;
        }

        if (font == fontPrec && color == colorPrec && align == alignPrec) {
          // qDebug()<<"concatenated: "<<added.at(i).unicode();
          partial.append(added.at(i).unicode());
        } else {
          crdt->localInsertGroup(line, index, partial, fontPrec, colorPrec,
                                 alignPrec);
          // qDebug()<<"Inserted: "<<partial;
          fontPrec = font;
          colorPrec = color;
          alignPrec = align;
          partial.clear();
          partial.append(added.at(i).unicode());
        }

        linePrec = numLines;
        if (added.at(i) == '\n') {
          numLines++;
        }
      }

      if (!partial.isNull() && !partial.isEmpty()) {
        crdt->localInsertGroup(line, index, partial, font, color, align);
      }
    }
  } else if (charsRemoved > 0 && charsRemoved - charsAdded > 0) {
    // qDebug()<<"----DEL";
    // undo to retrieve the content deleted
    disconnect(ui->textEdit->document(), &QTextDocument::contentsChange, this,
               &Editor::on_contentsChange);
    disconnect(ui->textEdit, &QTextEdit::cursorPositionChanged, this,
               &Editor::saveCursorPosition);
    QString removed;
    if (this->undoFlag == true) {
      // qDebug()<<"redo - undo";
      ui->textEdit->document()->redo();
      removed =
          ui->textEdit->document()->toPlainText().mid(position, charsRemoved);
      ui->textEdit->document()->undo();
      this->undoFlag = false;
    } else {

      ui->textEdit->document()->undo();
      removed =
          ui->textEdit->document()->toPlainText().mid(position, charsRemoved);
      ui->textEdit->document()->redo();
    }
    connect(ui->textEdit->document(), &QTextDocument::contentsChange, this,
            &Editor::on_contentsChange);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this,
            &Editor::saveCursorPosition);
    saveCursorPosition();
    // remove multiple chars
    // qDebug()<<"Before remove line and index: "<<"("<<line<<","<<index<<")";
    if (removed.length()) {
      crdt->localErase(line, index, removed.length());
    }
    // qDebug()<<"After remove line and index: "<<"("<<line<<","<<index<<")";
  } else if (charsRemoved == charsAdded &&
             (this->undoFlag == true || this->redoFlag == true)) {
    // format/alignment change by redo/undo
    // qDebug()<<"format change";
    // save cursor position
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(position);
    int line_m;
    int index_m;
    bool formatChange = false;
    // per ogni carattere nel documento lo confronto con quello nella struttura
    // dati
    while (true) {

      line_m = cursor.blockNumber();
      index_m = cursor.positionInBlock();

      // non sono sicuro di cosa succeda all'ultimo carattere
      if (cursor.movePosition(QTextCursor::NextCharacter,
                              QTextCursor::KeepAnchor) == false) {
        // qDebug()<<"ciclo break perchè movePosition failed";
        break;
      }

      QTextCharFormat formatDoc = cursor.charFormat();

      QTextCharFormat formatSL = this->crdt->getSymbolFormat(line_m, index_m);

      if (formatSL.font() == formatDoc.font() &&
          formatSL.foreground().color() == formatDoc.foreground().color()) {
        ////qDebug()<<"formatSL == formatDOC";

        break;
      }
      if (formatChange == false)
        formatChange = true;
    }

    if (formatChange == true) {
      // qDebug()<<"After the while true loop: "<<cursor.selectedText();
      on_formatChange(cursor);
    } else {
      // change alignment
      // TO DO: multiple lines alignment chnaged
      line_m = cursor.blockNumber();
      index_m = cursor.positionInBlock();
      QTextBlockFormat a = cursor.blockFormat();
      Qt::Alignment align = a.alignment();
      // qDebug()<<"First alignment from the editor:
      // "<<alignmentConversion(align);
      Qt::Alignment align_SL =
          alignmentConversion(this->crdt->getAlignmentLine(line_m));
      // qDebug()<<"First alignment  present in the crdt: "<<align_SL;
      this->crdt->localChangeAlignment(line_m, alignmentConversion(align));

      while (true) {
        // non sono sicuro di cosa succeda all'ultimo carattere
        if (cursor.movePosition(QTextCursor::NextBlock,
                                QTextCursor::MoveAnchor) == false) {
          ////qDebug()<<"ciclo break perchè movePosition failed";
          break;
        }
        a = cursor.blockFormat();
        align = a.alignment();
        // qDebug()<<"Alignment fromn the editor: "<<align;
        line_m = cursor.blockNumber();
        index_m = cursor.positionInBlock();
        align_SL = alignmentConversion(this->crdt->getAlignmentLine(line_m));
        // qDebug()<<"Alignment from the CRDT: "<<align_SL;
        this->crdt->localChangeAlignment(line_m, alignmentConversion(align));
      }
    }

    this->undoFlag = false;
    this->redoFlag = false;
  }
}

void Editor::on_changeAlignment(int align, int line, int index) {

  // qDebug() << "ON_CHANGE_ALIGNMENT: "<<align;
  QTextCursor cursor = ui->textEdit->textCursor();
  QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
  cursor.setPosition(block.position() + index);
  QTextBlockFormat textBlockFormat = block.blockFormat();
  if (align == SymbolFormat::Alignment::ALIGN_LEFT) {
    textBlockFormat.setAlignment(Qt::AlignLeft);
    if (line == this->line)
      actionAlignLeft->setChecked(true);
  } else if (align == SymbolFormat::Alignment::ALIGN_CENTER) {
    textBlockFormat.setAlignment(Qt::AlignCenter);
    if (line == this->line)
      actionAlignCenter->setChecked(true);
  } else if (align == SymbolFormat::Alignment::ALIGN_RIGHT) {
    textBlockFormat.setAlignment(Qt::AlignRight);
    if (line == this->line)
      actionAlignRight->setChecked(true);
  }
  cursor.mergeBlockFormat(textBlockFormat);

  // qDebug().noquote() << crdt->to_string();
}

void Editor::on_insert(int line, int index, const Symbol &s) {
  // qDebug() << "ON_INSERT REMOTE";
  QTextCursor cursor = ui->textEdit->textCursor();
  //    cursor.setPosition(index);

  QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
  cursor.setPosition(block.position() + index);

  // save old format to restore it later
  QTextCharFormat oldFormat = ui->textEdit->currentCharFormat();
  //    //qDebug() << "oldFormat" << oldFormat.font().italic() <<
  //    oldFormat.font().bold() << oldFormat.font().underline();
  QTextCharFormat newFormat = s.getQTextCharFormat();
  //    //qDebug() << "format" << newFormat.font().bold();
  cursor.setCharFormat(newFormat);
  cursor.insertText(QChar(s.getValue()));
  ui->textEdit->setCurrentCharFormat(oldFormat);

  // qDebug().noquote() << crdt->to_string();
}

void Editor::on_insertGroup(int line, int index, const QString &s,
                            QTextCharFormat newFormat) {
  // qDebug() << "ON_INSERT";
  QTextCursor cursor = ui->textEdit->textCursor();
  //    cursor.setPosition(index);

  QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
  cursor.setPosition(block.position() + index);

  // save old format to restore it later
  QTextCharFormat oldFormat = ui->textEdit->currentCharFormat();
  //    //qDebug() << "oldFormat" << oldFormat.font().italic() <<
  //    oldFormat.font().bold() << oldFormat.font().underline();

  //    //qDebug() << "format" << newFormat.font().bold();
  cursor.setCharFormat(newFormat);
  cursor.insertText(s);
  ui->textEdit->setCurrentCharFormat(oldFormat);

  // qDebug().noquote() << crdt->to_string();
}

void Editor::on_erase(int line, int index, int lenght) {
  // qDebug()<<"ON ERASE";
  QTextCursor cursor = ui->textEdit->textCursor();
  //    //qDebug() << line << index;
  QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
  cursor.setPosition(block.position() + index);
  for (int i = 0; i < lenght; i++) {
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
  }
  //    //qDebug() << "block position" << block.position();

  //    //qDebug() << "before deleting";
  cursor.removeSelectedText();
  //    //qDebug() << "after deleting";

  // qDebug().noquote() << crdt->to_string();
}

void Editor::on_change(int line, int index, const Symbol &s) {
  // qDebug() << "ON_CHANGE";
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

  // qDebug().noquote() << crdt->to_string();
}

// da cambiare
void Editor::updateText(const QString &text) {
  // qDebug() << "Update text!";
  ui->listWidget->clear();
  //    this->ui->listWidget->addItem(new
  //    QListWidgetItem(QIcon(*client->getProfile()),client->getUsername()));

  QListWidgetItem *item = new QListWidgetItem;
  QPixmap orig = *client->getProfile();
  // getting size if the original picture is not square
  int size = qMin(orig.width(), orig.height());
  // creating a new transparent pixmap with equal sides
  QPixmap rounded = QPixmap(size, size);
  rounded.fill(Qt::transparent);
  // creating circle clip area
  QPainterPath path;
  path.addEllipse(rounded.rect());
  QPainter painter(&rounded);
  painter.setClipPath(path);
  // filling rounded area if needed
  painter.fillRect(rounded.rect(), Qt::black);
  // getting offsets if the original picture is not square
  int x = qAbs(orig.width() - size) / 2;
  int y = qAbs(orig.height() - size) / 2;
  painter.drawPixmap(-x, -y, orig.width(), orig.height(), orig);

  QPixmap background = QPixmap(size + 50, size + 50);
  background.fill(Qt::transparent);
  QPainterPath path1;
  path1.addEllipse(background.rect());
  QPainter painter1(&background);
  painter1.setClipPath(path1);
  // filling rounded area if needed
  painter1.fillRect(background.rect(), QColor(0,	136,	86));
  // getting offsets if the original picture is not square
  x = qAbs(rounded.width() - size - 50) / 2;
  y = qAbs(rounded.height() - size - 50) / 2;
  painter1.drawPixmap(x, y, rounded.width(), rounded.height(), rounded);
  item->setIcon(QIcon(background));
  item->setText(client->getUsername());
  // qDebug() << "color: " << client->getColor();
  // item->setTextColor(QColor(client->getColor()));
  this->ui->listWidget->addItem(item);
  this->ui->textEdit->setText(text);
}

void Editor::addUsers(
    const QList<QPair<QPair<QString, QString>, QPixmap>> users) {

  for (int i = 0; i < users.count(); i++) {
    // QColor color = list_colors.getColor();
    int user = fromStringToIntegerHash(users.at(i).first.first);
    //  //qDebug()<<" --------Username: "<<user<<" Color: "<<color;
    // qDebug()<<"Try adding: "<<users.at(i).first.first;
    if (highlighter->addClient(user)) { // prevents duplicates
      if (this->highlighter->document() != 0) {
        // qDebug()<< "Assigning file";
        this->highlighter->setDocument(ui->textEdit->document());
        // qDebug()<< "Assigned file";
      }
      if (ui->textEdit->remote_cursors.contains(user)) {
        RemoteCursor *remote_cursor = ui->textEdit->remote_cursors.value(user);
        remote_cursor->setColor(this->highlighter->getColor(user));
      }

      QListWidgetItem *item = new QListWidgetItem();

      QPixmap orig = users.at(i).second;
      // getting size if the original picture is not square
      int size = qMin(orig.width(), orig.height());
      // creating a new transparent pixmap with equal sides
      QPixmap rounded = QPixmap(size, size);
      rounded.fill(Qt::transparent);
      // creating circle clip area
      QPainterPath path;
      path.addEllipse(rounded.rect());
      QPainter painter(&rounded);
      painter.setClipPath(path);
      // filling rounded area if needed
      painter.fillRect(rounded.rect(), Qt::black);
      // getting offsets if the original picture is not square
      int x = qAbs(orig.width() - size) / 2;
      int y = qAbs(orig.height() - size) / 2;
      painter.drawPixmap(-x, -y, orig.width(), orig.height(), orig);

      QPixmap background = QPixmap(size + 50, size + 50);
      background.fill(Qt::transparent);
      QPainterPath path1;
      path1.addEllipse(background.rect());
      QPainter painter1(&background);
      painter1.setClipPath(path1);
      // filling rounded area if needed
      painter1.fillRect(background.rect(), highlighter->getColor(user));
      // getting offsets if the original picture is not square
      x = qAbs(rounded.width() - size - 50) / 2;
      y = qAbs(rounded.height() - size - 50) / 2;
      painter1.drawPixmap(x, y, rounded.width(), rounded.height(), rounded);

      item->setIcon(QIcon(background));
      item->setText(users.at(i).first.second);
      item->setData(Qt::UserRole, users.at(i).first.first);
      // item->setTextAlignment(Qt::AlignHCenter);
      // item->setBackground( highlighter->getColor(user));
      item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
      item->setWhatsThis(users.at(i).first.first);
      this->ui->listWidget->addItem(item);
    }

  } // per ora è visualizzato l'username per faciliatare la cancellazione senza
    // riferimenti alla riga
}

void Editor::clear(bool serverDisconnected) {
  if (!serverDisconnected)
    client->closeFile();
  highlighter->freeAll();
  // clean the editor: disconnect...
  disconnect(ui->textEdit->document(), &QTextDocument::contentsChange, this,
             &Editor::on_contentsChange);
  disconnect(client, &Client::remoteCursor, this, &Editor::on_remoteCursor);
  disconnect(ui->textEdit, &QTextEdit::cursorPositionChanged, this,
             &Editor::saveCursorPosition);
  // create new CRDT with connections
  crdt->clear();
  ui->listWidget->clear();
  ui->textEdit->clear();

  // ... and then riconnect (because we want to remove chars locally without
  // deleteling them in server)
  connect(ui->textEdit->document(), &QTextDocument::contentsChange, this,
          &Editor::on_contentsChange);
  connect(client, &Client::remoteCursor, this, &Editor::on_remoteCursor);
  connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this,
          &Editor::saveCursorPosition);
}

void Editor::removeUser(const QString &username, const QString &nickname) {
  // qDebug()<<"Here remove"<<endl;
  QList<QListWidgetItem *> items =
      this->ui->listWidget->findItems(nickname, Qt::MatchFixedString);

  for (QListWidgetItem *item : items) {
    if (item->data(Qt::UserRole).toString() == username) {
      // qDebug()<<"ITEM TO REMOVE: "<<username;
      highlighter->freeColor(fromStringToIntegerHash(username));
      if (this->highlighter->document() != 0) {
        // qDebug()<< "Assigning file";
        this->highlighter->setDocument(ui->textEdit->document());
        // qDebug()<< "Assigned file";
      }
      this->ui->listWidget->removeItemWidget(item);
      //  ui->textEdit->remote_cursors.remove(fromStringToIntegerHash(username));
      delete item;
      break;
    }
  }
}

void Editor::saveCursorPosition() {
  // update alignment icon
  alignmentChanged(ui->textEdit->alignment());

  // update font

  // save cursor position
  QTextCursor cursor = ui->textEdit->textCursor();
  this->line = cursor.blockNumber();
  this->index = cursor.positionInBlock();
  fontChanged(cursor.charFormat().font());

  // use positon of symbol AFTER cursor as reference
  // qDebug() << "cursor position before" << this->line << this->index;
  // correct_position(this->line, this->index);
  crdt->cursorPositionChanged(this->line, this->index);
}

void Editor::showEvent(QShowEvent *) {
  QString windowTitle = client->getOpenedFile().replace(",", " (") + ")";
  this->setWindowTitle(windowTitle + " - Shared Editor");
  moveCursorToEnd();
}

// update icons in toolbar (italic, bold, ...) depending on the char before
// cursor
void Editor::on_currentCharFormatChanged(const QTextCharFormat &format) {
  fontChanged(format.font());
  colorChanged(format.foreground().color());
  void on_remoteCursor(int editor_id, Symbol s);
}

void Editor::textColor() {
  QColor col = QColorDialog::getColor(ui->textEdit->textColor(), this);
  if (!col.isValid())
    return;
  ui->textEdit->setTextColor(col);
  on_formatChange();
}

void Editor::clipboardDataChanged() {
#ifndef QT_NO_CLIPBOARD
  if (const QMimeData *md = QApplication::clipboard()->mimeData())
    ui->actionPaste->setEnabled(md->hasText());
#endif
}

void Editor::fontChanged(const QFont &f) {
  // qDebug()<<"font:"<<f;
  comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
  comboSize->setCurrentIndex(
      comboSize->findText(QString::number(f.pointSize())));
  ui->actionBold->setChecked(f.bold());
  ui->actionItalic->setChecked(f.italic());
  ui->actionUnderline->setChecked(f.underline());
}

void Editor::colorChanged(const QColor &c) {
  QPixmap pix(16, 16);
  pix.fill(c);
  actionTextColor->setIcon(pix);
}

void Editor::alignmentChanged(Qt::Alignment a) {
  if (a & Qt::AlignLeft)
    actionAlignLeft->setChecked(true);
  else if (a & Qt::AlignHCenter)
    actionAlignCenter->setChecked(true);
  else if (a & Qt::AlignRight)
    actionAlignRight->setChecked(true);
}

Qt::Alignment Editor::getCurrentAlignment() {
  if (actionAlignLeft->isChecked())
    return Qt::AlignLeft;
  else if (actionAlignCenter->isChecked())
    return Qt::AlignCenter;
  else if (actionAlignRight->isChecked())
    return Qt::AlignRight;
}

void Editor::textFamily(const QString &f) {
  // qDebug()<<"Text Family: "<<f;
  ui->textEdit->setFontFamily(f);
  on_formatChange();
}

void Editor::textSize(const QString &p) {
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
  // qDebug() << "CHANGED" << ui->textEdit->textCursor().selectedText();
  QString changed = ui->textEdit->textCursor().selectedText();

  int start = ui->textEdit->textCursor().selectionStart();

  int end = ui->textEdit->textCursor().selectionEnd();

  QFont fontPrec;
  QColor colorPrec;
  QFont font;
  QColor color;
  int startIndex;
  int endIndex;
  int startLine;
  int endLine;
  // qDebug() << "start/end selection" << start << end;
  QTextCursor cursor = ui->textEdit->textCursor();
  for (int i = start; i < end; i++) {
    cursor.setPosition(i);
    int line = cursor.blockNumber();
    int index = cursor.positionInBlock();
    if (i == start) {
      startIndex = endIndex = index;
      startLine = endLine = line;
    }
    // qDebug() << "line/index/char" << line << index << changed.at(i - start);

    // if newline ('\n') do nothing
    if (changed.at(i - start) == QChar(0x2029)) {
      //            //qDebug() << "NEWLINE";
      continue;
    }

    qDebug() << "Font: " << font;
    qDebug() << "FontPrec: " << fontPrec;
    if (font == fontPrec)
      qDebug() << "Font equals";

    qDebug() << "Color: " << color;
    qDebug() << "ColorPrec: " << colorPrec;
    if (color == colorPrec)
      qDebug() << "Color equals";

    // position AFTER the char to read its format
    cursor.setPosition(i + 1);
    font = cursor.charFormat().font();
    color = cursor.charFormat().foreground().color();
    if (i == start) {
      fontPrec = font;
      colorPrec = color;
    }

    if (font == fontPrec && color == colorPrec) {
      // qDebug()<<"concatenated: "<<added.at(i).unicode();
      endIndex = index;
      endLine = line;
    } else {
      crdt->localChangeGroup(startLine, endLine, startIndex, endIndex, fontPrec,
                             colorPrec);
      qDebug() << "Local change group; startLine: " << startLine
               << " endLine: " << endLine << " startIndex: " << startIndex
               << " endIndex: " << endIndex;
      fontPrec = font;
      colorPrec = color;
      startIndex = index;
      startLine = line;
      endIndex = index;
      endLine = line;
    }
  }
  crdt->localChangeGroup(startLine, endLine, startIndex, endIndex, fontPrec,
                         colorPrec);
}

void Editor::on_formatChange(QTextCursor c) {
  // qDebug() << "CHANGED" << c.selectedText();
  QString changed = c.selectedText();
  QFont fontPrec;
  QColor colorPrec;
  QFont font;
  QColor color;

  int start = c.selectionStart();

  int end = c.selectionEnd();

  int startIndex;
  int endIndex;
  int startLine;
  int endLine;
  // qDebug() << "start/end selection" << start << end;
  QTextCursor cursor = ui->textEdit->textCursor();
  for (int i = start; i < end; i++) {
    cursor.setPosition(i);
    int line = cursor.blockNumber();
    int index = cursor.positionInBlock();
    if (i == start) {
      startIndex = endIndex = index;
      startLine = endLine = line;
    }
    // qDebug() << "line/index/char" << line << index << changed.at(i - start);

    // if newline ('\n') do nothing
    if (changed.at(i - start) == QChar(0x2029)) {
      //            //qDebug() << "NEWLINE";
      continue;
    }

    // position AFTER the char to read its format
    cursor.setPosition(i + 1);
    font = cursor.charFormat().font();
    color = cursor.charFormat().foreground().color();
    if (i == start) {
      fontPrec = font;
      colorPrec = color;
    }
    qDebug() << "Font: " << font;
    qDebug() << "FontPrec: " << fontPrec;

    if (font == fontPrec && color == colorPrec) {
      // qDebug()<<"concatenated: "<<added.at(i).unicode();
      endIndex = index;
      endLine = line;
    } else {
      crdt->localChangeGroup(startLine, endLine, startIndex, endIndex, fontPrec,
                             colorPrec);
      qDebug() << "Local change group; startLine: " << startLine
               << " endLine: " << endLine << " startIndex: " << startIndex
               << " endIndex: " << endIndex;
      fontPrec = font;
      colorPrec = color;
      startIndex = index;
      startLine = line;
      endIndex = index;
      endLine = line;
    }
  }
  crdt->localChangeGroup(startLine, endLine, startIndex, endIndex, fontPrec,
                         colorPrec);
}

void Editor::on_addCRDTterminator() {
  QFont font;
  QColor color;
  this->crdt->localInsert(0, 0, '\0', font, color, getCurrentAlignment());
}

void Editor::on_remoteCursor(int editor_id, Symbol s) {
  int line, index;
  // qDebug() << QChar(s.getValue());
  crdt->getPositionFromSymbol(s, line, index);
  // qDebug() << "LINE/INDEX" << line << index;
  // disconnect(ui->textEdit->document(), &QTextDocument::contentsChange, this,
  // &Editor::on_contentsChange); disconnect(ui->textEdit,
  // &QTextEdit::cursorPositionChanged, this, &Editor::saveCursorPosition);

  QTextBlock block = ui->textEdit->document()->findBlockByNumber(line);
  if (!ui->textEdit->remote_cursors.contains(editor_id)) {
    // TODO: get the color instead of using red
    // qDebug()<<"add new cursor: "<<editor_id;
    RemoteCursor *remote_cursor =
        new RemoteCursor(ui->textEdit->textCursor(), block, index,
                         highlighter->getColor(editor_id));
    ui->textEdit->remote_cursors.insert(editor_id, remote_cursor);
  } else {
    RemoteCursor *remote_cursor = ui->textEdit->remote_cursors.value(editor_id);
    remote_cursor->moveTo(block, index);
  }

  // connect(ui->textEdit->document(), &QTextDocument::contentsChange, this,
  // &Editor::on_contentsChange); connect(ui->textEdit,
  // &QTextEdit::cursorPositionChanged, this, &Editor::saveCursorPosition);
}

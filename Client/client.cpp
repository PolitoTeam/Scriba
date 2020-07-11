#include "client.h"
#include "../Utility/symbol.h"
#include "CRDT.h"
#include <QBuffer>
#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHostAddress>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QMessageBox>
#include <QPixmap>
#include <QSslConfiguration>
#include <QTcpSocket>
#include <QtEndian>

Client::Client(QObject *parent, QString addr, quint16 port)
    : QObject(parent), m_clientSocket(new QSslSocket(this)), m_loggedIn(false) {
  this->addr = addr;
  this->port = port;

  connect(m_clientSocket, &QSslSocket::connected, this, &Client::connected);
  connect(m_clientSocket,
          static_cast<void (QSslSocket::*)(QAbstractSocket::SocketError)>(
              &QAbstractSocket::error),
          this, &Client::error);
  connect(m_clientSocket, &QSslSocket::disconnected, this, [this]() -> void {
    this->m_received_data.clear();
    this->m_exptected_json_size = 0;
  });
  connect(this, &Client::byteArrayReceived, this, &Client::on_byteArrayReceived,
          Qt::QueuedConnection);
  connect(this, &Client::jsonReceived, this, &Client::on_jsonReceived,
          Qt::QueuedConnection);

  // Connect readyRead() to the slot
  // that will take care of reading the data in
  connect(m_clientSocket, &QSslSocket::readyRead, this, &Client::onReadyRead,
          Qt::QueuedConnection);

  // QSslSocket::VerifyPeer doesn't work on macOS
  m_clientSocket->addCaCertificates(":/resources/certificates/rootCA.crt");
  m_clientSocket->setPeerVerifyMode(QSslSocket::VerifyNone);

  profile = new QPixmap();
  profile->load(":/images/anonymous");
}

void Client::sendByteArray(const QByteArray &byteArray) {
  SerializeSize size;
  quint64 json_size = size(byteArray);

  QDataStream socketStream(m_clientSocket);
  socketStream.setVersion(QDataStream::Qt_5_7);
  socketStream << json_size << byteArray;
}

void Client::login(const QString &username, const QString &password) {
  connectToServer(QHostAddress(this->addr), this->port);

  QJsonObject message;
  message["type"] = QStringLiteral("login");
  message["username"] = username;
  message["password"] = password;
  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

void Client::signup(const QString &username, const QString &password,
                    QPixmap *image) {
  connectToServer(QHostAddress(this->addr), this->port);

  QJsonObject message;
  message["type"] = QStringLiteral("signup");
  message["username"] = username;
  message["password"] = password;

  QByteArray obj = QJsonDocument(message).toJson(QJsonDocument::Compact);
  quint32 size_json = obj.size();
  QByteArray ba((const char *)&size_json, sizeof(size_json));
  ba.append(obj);

  // If profile image uploaded by user
  if (image != nullptr) {
    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);
    image->save(&buffer, "PNG");
    quint32 size_img = bArray.size();

    QByteArray p((const char *)&size_img, sizeof(size_img));
    p.append(bArray);
    ba.append(p);
  } else {
    quint32 size_img = 0;
    QByteArray p((const char *)&size_img, sizeof(size_img));
    ba.append(p);
  }

  sendByteArray(ba);
}

void Client::getFiles(bool shared) {
  QJsonObject message;
  if (shared) {
    message["type"] = QStringLiteral("list_shared_files");
  } else {
    message["type"] = QStringLiteral("list_files");
  }
  message["username"] = this->username;

  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

void Client::getFilenameFromLink(const QString &sharedLink) {
  QJsonObject message;
  message["type"] = QStringLiteral("filename_from_sharedLink");
  message["sharedLink"] = sharedLink;

  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

void Client::updateNickname(const QString &nickname) {
  QJsonObject message;
  message["type"] = QStringLiteral("nickname");
  message["username"] = this->username;
  message["nickname"] = nickname;

  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
  this->nickname = nickname;
}

void Client::updatePassword(const QString &oldpassword,
                            const QString &newpassword) {
  QJsonObject message;
  message["type"] = QStringLiteral("password");
  message["username"] = this->username;
  message["oldpass"] = oldpassword;
  message["newpass"] = newpassword;

  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

// TODO: GUARDARE QUESTA IN CASO DI ERRORE PER RIPRISTINARE SE IN CASO LA
// SENDBYTEARRAY NON VA
void Client::checkOldPassword(const QString &old_password) {
  QJsonObject message;
  message["type"] = QStringLiteral("check_old_password");
  message["username"] = this->username;
  message["old_password"] = old_password;
  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

void Client::checkExistingOrNotUsername(const QString &username) {
  connectToServer(QHostAddress(this->addr), this->port);

  QJsonObject message;
  message["type"] = QStringLiteral("check_username");
  message["username"] = username;
  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

// Attempts to close the socket.
// If there is pending data waiting to be written, QAbstractSocket will enter
// ClosingState and wait until all data has been written.
void Client::disconnectFromHost() {
  if (this->m_loggedIn == true) {
    this->username.clear();
    this->nickname.clear();
    this->files.clear();
    this->m_loggedIn = false;
    this->profile->load(":/images/anonymous");
  }

  m_clientSocket->disconnectFromHost();
  if (m_clientSocket->state() == QAbstractSocket::UnconnectedState ||
      m_clientSocket->waitForDisconnected(1000)) {
  }
}

void Client::on_jsonReceived(const QJsonObject &docObj) {
  //  qDebug() << docObj;

  // Actions depend on the type of message
  const QJsonValue typeVal = docObj.value(QLatin1String("type"));
  if (typeVal.isNull() || !typeVal.isString())
    return; // A message with no type was received so we just ignore it

  if (typeVal.toString().compare(QLatin1String("signup"),
                                 Qt::CaseInsensitive) == 0) {
    if (m_loggedIn)
      return; // If we are already logged in we ignore

    const QJsonValue resultVal = docObj.value(QLatin1String("success"));
    if (resultVal.isNull() || !resultVal.isBool())
      return;
    const bool signupSuccess = resultVal.toBool();
    if (signupSuccess) {
      emit signedUp();
      return;
    }
    // The signup attempt failed, we extract the reason of the failure
    // from the JSON and notify it via the signupError signal
    const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
    emit signupError(reasonVal.toString());
  } else if (typeVal.toString().compare(QLatin1String("old_password_checked"),
                                        Qt::CaseInsensitive) == 0) {
    const QJsonValue resultVal = docObj.value(QLatin1String("success"));
    if (resultVal.isNull() || !resultVal.isBool())
      return;
    const bool oldPasswordCheckSuccess = resultVal.toBool();
    const QString reason = docObj.value(QLatin1String("reason")).toString();
    if (!oldPasswordCheckSuccess)
      emit wrongOldPassword(reason);
    else
      emit correctOldPassword();
  } else if (typeVal.toString().compare(QLatin1String("check_username"),
                                        Qt::CaseInsensitive) == 0) {
    const QJsonValue resultVal = docObj.value(QLatin1String("success"));
    if (resultVal.isNull() || !resultVal.isBool())
      return;
    const bool usernameCheckSuccess = resultVal.toBool();
    const QString reason = docObj.value(QLatin1String("reason")).toString();
    const QJsonValue username = docObj.value(QLatin1String("username"));
    if (username.isNull() || !username.isString())
      return;
    if (!usernameCheckSuccess)
      emit existingUsername(username.toString());
    else
      emit successUsernameCheck(username.toString());
  } else if (typeVal.toString().compare(QLatin1String("operation"),
                                        Qt::CaseInsensitive) == 0) {
    int operation_type = docObj["operation_type"].toInt();
    if (operation_type == INSERT) {
      QJsonObject symbol = docObj["symbol"].toObject();
      Symbol s = Symbol::fromJson(symbol);

      emit remoteInsert(s);
    } else if (operation_type == ALIGN) {
      QJsonObject symbol = docObj["symbol"].toObject();
      Symbol s = Symbol::fromJson(symbol);

      emit remoteAlignChange(s);
    } else if (operation_type == CURSOR) {
      QJsonObject symbol = docObj["symbol"].toObject();
      Symbol s = Symbol::fromJson(symbol);

      int editor_id = docObj["editorId"].toInt();
      emit remoteCursor(editor_id, s);
    }
  } else if (typeVal.toString().compare(QLatin1String("list_files"),
                                        Qt::CaseInsensitive) == 0) {
    const QJsonValue resultVal = docObj.value(QLatin1String("success"));
    if (resultVal.isNull() || !resultVal.isBool())
      return;
    const bool getFile = resultVal.toBool();

    if (getFile) {
      const QJsonValue array = docObj.value(QLatin1String("files"));
      if (array.isNull() || !array.isArray())
        return;
      const QJsonArray array_files = array.toArray();

      this->files.clear();
      foreach (const QJsonValue &v, array_files) {
        this->files.push_back(
            QPair<QString, QString>(v.toObject().value("name").toString(),
                                    v.toObject().value("owner").toString()));
      }

      const QJsonValue sharedJson = docObj.value(QLatin1String("shared"));
      if (sharedJson.isNull() || !sharedJson.isBool())
        return;
      const bool shared = sharedJson.toBool();
      emit filesReceived(shared);
    } else {
      const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
      emit openFilesError(reasonVal.toString());
    }
  } else if (typeVal.toString().compare(QLatin1String("new_file"),
                                        Qt::CaseInsensitive) == 0) {
    const QJsonValue resultVal = docObj.value(QLatin1String("success"));
    if (resultVal.isNull() || !resultVal.isBool())
      return;
    const bool success = resultVal.toBool();
    if (success) {
      sharedLink = docObj.value(QLatin1String("shared_link")).toString();
      emit addCRDTterminator();
      emit correctNewFile();
    } else {
      const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
      emit wrongNewFile(reasonVal.toString());
    }
  } else if (typeVal.toString().compare(QLatin1String("disconnection"),
                                        Qt::CaseInsensitive) == 0) {
    const QJsonValue file = docObj.value(QLatin1String("filename"));
    if (file.isNull() || !file.isString())
      return;
    const QJsonValue name = docObj.value(QLatin1String("user"));
    if (name.isNull() || !name.isString())
      return;
    const QJsonValue nickname = docObj.value(QLatin1String("nickname"));
    if (nickname.isNull() || !nickname.isString())
      return;
    if (!file.toString().compare(this->openfile)) {
      emit userDisconnected(name.toString(), nickname.toString());
    }

  } else if (typeVal.toString().compare(
                 QLatin1String("filename_from_sharedLink"),
                 Qt::CaseInsensitive) == 0) {
    const QJsonValue resultVal = docObj.value(QLatin1String("success"));
    if (resultVal.isNull() || !resultVal.isBool())
      return;
    const bool success = resultVal.toBool();
    if (success) {
      QString filename = docObj.value(QLatin1String("filename")).toString();
      this->openFile(filename);
    } else {
      const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
      emit wrongSharedLink(reasonVal.toString());
    }
  } else if (typeVal.toString().compare(QLatin1String("password"),
                                        Qt::CaseInsensitive) == 0) {
    const QJsonValue resultVal = docObj.value(QLatin1String("success"));
    if (resultVal.isNull() || !resultVal.isBool())
      return;
    const bool success = resultVal.toBool();
    if (success) {
      emit successUpdatePassword();
    } else {
      const QString reasonVal =
          docObj.value(QLatin1String("reason")).toString();
      emit failedUpdatePassword(reasonVal);
    }
  }
}

void Client::createNewFile(QString filename) {
  QJsonObject message;
  message["type"] = QStringLiteral("new_file");
  message["filename"] = filename;
  message["author"] = this->username;

  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

void Client::connectToServer(const QHostAddress &address, quint16 port) {
  if (m_clientSocket->state() != QAbstractSocket::UnconnectedState)
    return;

  m_clientSocket->connectToHost(address, port);
  if (m_clientSocket->waitForConnected()) {
    // Start handshake
    m_clientSocket->startClientEncryption();
  }

  // Wait up to 1 minute
  if (!m_clientSocket->waitForEncrypted(60000)) {
    qDebug("Unable to connect to server");
  }
}

void Client::onReadyRead() {
  onReadyRead_helper(m_clientSocket, m_received_data, m_exptected_json_size,
                     m_buffer, *this);
}

void Client::on_byteArrayReceived(const QByteArray &doc) {
  quint32 size = qFromLittleEndian<qint32>(
      reinterpret_cast<const uchar *>(doc.left(4).data()));
  QByteArray json = doc.mid(4, size);
  QByteArray content_image_array = doc.mid(4 + size, -1);

  QJsonParseError parseError;
  const QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &parseError);
  if (parseError.error == QJsonParseError::NoError) {
    if (jsonDoc.isObject()) {
      QJsonObject docObj = jsonDoc.object();
      const QJsonValue typeVal = docObj.value(QLatin1String("type"));
      if (typeVal.isNull() || !typeVal.isString())
        return;

      if (typeVal.toString().compare(QLatin1String("file_to_open"),
                                     Qt::CaseInsensitive) == 0) {
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
          return;

        const bool success = resultVal.toBool();
        if (success) {
          const QJsonValue tot_symbolsVal =
              docObj.value(QLatin1String("tot_symbols"));
          if (tot_symbolsVal.isNull() || !tot_symbolsVal.toInt())
            return;
          int tot_symbols = tot_symbolsVal.toInt();

          progress = new QProgressDialog(nullptr);
          progress_counter = 0;
          progress->setWindowTitle("Loading...");
          progress->setRange(0, tot_symbols - 1);
          progress->setModal(true);
          progress->setValue(0);
          progress->setMinimumDuration(0);
          progress->setWindowFlags(Qt::Window | Qt::WindowTitleHint |
                                   Qt::CustomizeWindowHint);
          progress->setCancelButton(nullptr);

          quint32 content_size =
              qFromLittleEndian<qint32>(reinterpret_cast<const uchar *>(
                  content_image_array.left(4).data()));

          QVector<Symbol> vec;
          if (content_size != 0) {
            QByteArray content = content_image_array.mid(4, content_size);
            QDataStream out(&content, QIODevice::ReadOnly);
            out >> vec;
          } else {
            throw std::runtime_error("Empty content received.");
          }
          content_image_array = content_image_array.mid(content_size + 4);

          // Add in editor and CRDT all the symbols received from server
          for (int i = vec.size() - 1; i >= 0; i--) {
            Symbol s = vec[i];
            emit remoteInsert(s);
            if (s.getValue() == '\n' || s.getValue() == '\0')
              emit remoteAlignChange(s);

            progress_counter++;
            progress->setValue(progress_counter);
          }

          progress->hide();
          progress->cancel();

          const QJsonValue name = docObj.value(QLatin1String("filename"));
          if (name.isNull() || !name.isString())
            return;
          this->openfile = name.toString();

          const QJsonValue shared_link =
              docObj.value(QLatin1String("shared_link"));
          if (shared_link.isNull() || !shared_link.isString())
            return;
          this->sharedLink = shared_link.toString();

          const QJsonValue array = docObj.value(QLatin1String("users"));
          if (array.isNull() || !array.isArray())
            return;
          const QJsonArray array_users = array.toArray();
          QList<QPair<QPair<QString, QString>, QPixmap>> connected;

          // Retrieve information (image, username/nickname) about users
          // currently using the file
          foreach (const QJsonValue &v, array_users) {
            quint32 img_size =
                qFromLittleEndian<qint32>(reinterpret_cast<const uchar *>(
                    content_image_array.left(4).data()));

            // Retrieve image
            QPixmap p;
            if (img_size != 0) {
              QByteArray img = content_image_array.mid(4, img_size);
              p.loadFromData(img);
            } else {
              // Users don't send their image if it is the default one
              p.load(":/images/anonymous");
            }

            // Retrieve username/nickname
            content_image_array = content_image_array.mid(img_size + 4);
            connected.append(QPair<QPair<QString, QString>, QPixmap>(
                QPair<QString, QString>(
                    v.toObject().value("username").toString(),
                    v.toObject().value("nickname").toString()),
                p));
          }

          emit usersConnectedReceived(connected);
          emit correctOpenedFile();
        } else {
          this->openfile.clear();
          const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
          emit wrongListFiles(reasonVal.toString());
        }

        // Information about new user connected to the file currently open
      } else if (typeVal.toString().compare(QLatin1String("connection"),
                                            Qt::CaseInsensitive) == 0) {
        const QJsonValue file = docObj.value(QLatin1String("filename"));
        if (file.isNull() || !file.isString())
          return;
        const QJsonValue name = docObj.value(QLatin1String("username"));
        if (name.isNull() || !name.isString())
          return;

        if (!file.toString().compare(this->openfile)) {
          QList<QPair<QPair<QString, QString>, QPixmap>> connected;
          quint32 img_size =
              qFromLittleEndian<qint32>(reinterpret_cast<const uchar *>(
                  content_image_array.left(4).data()));

          // Add image and username/nickname
          if (img_size == 0) {
            connected.append(QPair<QPair<QString, QString>, QPixmap>(
                QPair<QString, QString>(docObj.value("username").toString(),
                                        docObj.value("nickname").toString()),
                QPixmap(":/images/anonymous")));
          } else {
            QByteArray img = content_image_array.mid(4);
            QPixmap p;
            p.loadFromData(img);
            connected.append(QPair<QPair<QString, QString>, QPixmap>(
                QPair<QString, QString>(docObj.value("username").toString(),
                                        docObj.value("nickname").toString()),
                p));
          }

          emit usersConnectedReceived(connected);
        }

      } else if (typeVal.toString().compare(QLatin1String("login"),
                                            Qt::CaseInsensitive) == 0) {
        if (m_loggedIn)
          return;
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
          return;
        const bool loginSuccess = resultVal.toBool();
        if (loginSuccess) {
          const QJsonValue user = docObj.value(QLatin1String("username"));
          if (user.isNull() || !user.isString())
            return;
          const QString username = user.toString().simplified();
          if (username.isEmpty())
            return;

          const QJsonValue nick = docObj.value(QLatin1String("nickname"));
          if (nick.isNull() || !nick.isString())
            return;
          const QString nickname = nick.toString().simplified();
          if (nickname.isEmpty())
            return;

          this->username = username;
          this->nickname = nickname;

          quint32 img_size =
              qFromLittleEndian<qint32>(reinterpret_cast<const uchar *>(
                  content_image_array.left(4).data()));

          if (img_size != 0) {
            QByteArray img = content_image_array.mid(4);
            profile->loadFromData(img);
          }

          m_loggedIn = true;
          emit loggedIn();
          return;
        }
        const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
        emit loginError(reasonVal.toString());

        // Operation (insertion, deletion, modification) received
      } else if (typeVal.toString().compare(QLatin1String("operation"),
                                            Qt::CaseInsensitive) == 0) {
        const QJsonValue opType = docObj.value(QLatin1String("operation_type"));
        if (opType.isNull()) {
          return;
        }
        int operation_type = opType.toInt();

        if (operation_type != PASTE && operation_type != CHANGE &&
            operation_type != DELETE)
          return;

        const QJsonValue tot_symbolsVal =
            docObj.value(QLatin1String("tot_symbols"));
        if (tot_symbolsVal.isNull()) {
          return;
        }
        int tot_symbols = tot_symbolsVal.toInt();

        quint32 content_size =
            qFromLittleEndian<qint32>(reinterpret_cast<const uchar *>(
                content_image_array.left(4).data()));

        // Retrieve symbols
        QVector<Symbol> vec(tot_symbols);
        if (content_size != 0) {
          QByteArray content = content_image_array.mid(4, content_size);
          QDataStream out(&content, QIODevice::ReadOnly);
          out >> vec;
        } else {
          throw std::runtime_error("Empty content received.");
        }

        if (operation_type == PASTE)
          emit remotePaste(vec);
        else if (operation_type == CHANGE)
          emit remoteChange(vec);
        else
          emit remoteErase(vec);
      }
    }
  } else {
    throw std::runtime_error("Invalid json received.");
  }
}

QString Client::getNickname() { return this->nickname; }

QString Client::getUsername() { return this->username; }

void Client::setNickname(const QString &nickname) { this->nickname = nickname; }

QPixmap *Client::getProfile() { return profile; }

void Client::sendProfileImage() {
  QJsonObject message;
  message["type"] = QStringLiteral("update_image");
  message["username"] = username;

  QByteArray obj = QJsonDocument(message).toJson(QJsonDocument::Compact);
  quint32 size_json = obj.size();
  QByteArray ba((const char *)&size_json, sizeof(size_json));
  ba.append(obj);

  QByteArray bArray;
  QBuffer buffer(&bArray);
  buffer.open(QIODevice::WriteOnly);
  profile->save(&buffer, "PNG");
  quint32 size_img = bArray.size();

  QByteArray p((const char *)&size_img, sizeof(size_img));
  p.append(bArray);
  ba.append(p);

  sendByteArray(ba);
}

void Client::overrideProfileImage(const QPixmap &pixmap) {
  *this->profile = pixmap;
}

QList<QPair<QString, QString>> Client::getActiveFiles() { return files; }

void Client::sendJson(const QJsonObject &message) {
  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

void Client::openFile(const QString &filename) {
  QJsonObject message;
  message["type"] = QStringLiteral("file_to_open");
  message["filename"] = filename;

  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

void Client::closeFile() {
  QJsonObject message;
  message["type"] = QStringLiteral("close");
  message["filename"] = this->openfile;
  message["username"] = this->username;
  message["nickname"] = this->nickname;

  sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

QString Client::getSharedLink() { return this->sharedLink; }

QString Client::getOpenedFile() { return this->openfile; }

void Client::setOpenedFile(const QString &name) { this->openfile = name; }

QByteArray Client::createByteArrayFileContent(QJsonObject message,
                                              QVector<Symbol> c) {
  QByteArray byte_array_msg = QJsonDocument(message).toJson();
  quint32 size_json = byte_array_msg.size();

  QByteArray byte_array_content;
  QDataStream in(&byte_array_content, QIODevice::WriteOnly);
  in << c;
  quint32 size_content = byte_array_content.size();

  QByteArray ba((const char *)&size_json, sizeof(size_json));
  ba.append(byte_array_msg);
  QByteArray ba_c((const char *)&size_content, sizeof(size_content));
  ba_c.append(byte_array_content);
  ba.append(ba_c);

  return ba;
}

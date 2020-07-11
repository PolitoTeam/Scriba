#ifndef BYTEREADER_H
#define BYTEREADER_H

//#include "byteReader.h"
#include <QBuffer>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSslSocket>

class ByteReader {
public:
  virtual void jsonReceived(const QJsonObject &jsonDoc) = 0;
  virtual void byteArrayReceived(const QByteArray &jsonDoc) = 0;
};

static void extract_content_size(QSslSocket *socket, QByteArray &received_data,
                                 quint64 &expected_json_size) {
  received_data.append(socket->readAll());

  QDataStream in;
  QBuffer in_buffer;
  in_buffer.setBuffer(&received_data);
  in_buffer.open(QIODevice::ReadOnly);
  in.setDevice(&in_buffer);
  in.setVersion(QDataStream::Qt_5_7);

  quint64 size = 0;
  in >> size;
  expected_json_size = size;
  in_buffer.close();
}

static bool parseJson(QByteArray &received_data, QBuffer &buffer,
                      ByteReader &obj) {
  QByteArray json_data;
  QDataStream in;
  buffer.setBuffer(&received_data);
  if (!buffer.open(QIODevice::ReadOnly)) {
    return false;
  }

  in.setDevice(&buffer);
  in.setVersion(QDataStream::Qt_5_7);
  in.startTransaction();
  quint64 json_size;
  in >> json_size >> json_data;
  json_data.truncate(json_size);

  if (!in.commitTransaction()) {
    buffer.close();
    return false;
  }
  buffer.close();

  QJsonParseError parseError;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(json_data, &parseError);

  // If not able to parse the json, it means that a QByteArray
  // containing images or symbols has been received
  if (parseError.error == QJsonParseError::NoError) {
    if (jsonDoc.isObject()) {
      emit obj.jsonReceived(jsonDoc.object());
    } else {
      throw std::runtime_error("Invalid json received.");
    }
  } else {
    emit obj.byteArrayReceived(json_data);
  }
  received_data.remove(0, 8 + json_size);
  return true;
}

static void onReadyRead_helper(QSslSocket *socket, QByteArray &received_data,
                               quint64 &expected_json_size, QBuffer &buffer,
                               ByteReader &obj) {
  if (socket->bytesAvailable() > 0) {
    received_data.append(socket->readAll());
  }

  // 8 is the size of the integer (that contains content size)
  if (received_data.isNull() || received_data.size() < 8) {
    return;
  }

  if (expected_json_size == 0) {
    // Update m_received_data and m_exptected_json_size
    extract_content_size(socket, received_data, expected_json_size);
  }

  // If data completely received
  if (expected_json_size > 0 &&
      received_data.size() >= expected_json_size + 8) {
    if (parseJson(received_data, buffer, obj)) {
      expected_json_size = 0;
      onReadyRead_helper(socket, received_data, expected_json_size, buffer,
                         obj);
    }
  }
}

#endif // BYTEREADER_H

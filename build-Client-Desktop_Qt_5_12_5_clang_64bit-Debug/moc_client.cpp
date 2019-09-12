/****************************************************************************
** Meta object code from reading C++ file 'client.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Client/client.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Client_t {
    QByteArrayData data[27];
    char stringdata0[278];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Client_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Client_t qt_meta_stringdata_Client = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Client"
QT_MOC_LITERAL(1, 7, 9), // "connected"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 8), // "loggedIn"
QT_MOC_LITERAL(4, 27, 8), // "signedUp"
QT_MOC_LITERAL(5, 36, 11), // "signupError"
QT_MOC_LITERAL(6, 48, 6), // "reason"
QT_MOC_LITERAL(7, 55, 10), // "loginError"
QT_MOC_LITERAL(8, 66, 12), // "disconnected"
QT_MOC_LITERAL(9, 79, 15), // "messageReceived"
QT_MOC_LITERAL(10, 95, 6), // "sender"
QT_MOC_LITERAL(11, 102, 4), // "text"
QT_MOC_LITERAL(12, 107, 5), // "error"
QT_MOC_LITERAL(13, 113, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(14, 142, 11), // "socketError"
QT_MOC_LITERAL(15, 154, 10), // "userJoined"
QT_MOC_LITERAL(16, 165, 8), // "username"
QT_MOC_LITERAL(17, 174, 8), // "userLeft"
QT_MOC_LITERAL(18, 183, 15), // "connectToServer"
QT_MOC_LITERAL(19, 199, 12), // "QHostAddress"
QT_MOC_LITERAL(20, 212, 7), // "address"
QT_MOC_LITERAL(21, 220, 4), // "port"
QT_MOC_LITERAL(22, 225, 5), // "login"
QT_MOC_LITERAL(23, 231, 8), // "password"
QT_MOC_LITERAL(24, 240, 6), // "signup"
QT_MOC_LITERAL(25, 247, 18), // "disconnectFromHost"
QT_MOC_LITERAL(26, 266, 11) // "onReadyRead"

    },
    "Client\0connected\0\0loggedIn\0signedUp\0"
    "signupError\0reason\0loginError\0"
    "disconnected\0messageReceived\0sender\0"
    "text\0error\0QAbstractSocket::SocketError\0"
    "socketError\0userJoined\0username\0"
    "userLeft\0connectToServer\0QHostAddress\0"
    "address\0port\0login\0password\0signup\0"
    "disconnectFromHost\0onReadyRead"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Client[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x06 /* Public */,
       3,    0,   90,    2, 0x06 /* Public */,
       4,    0,   91,    2, 0x06 /* Public */,
       5,    1,   92,    2, 0x06 /* Public */,
       7,    1,   95,    2, 0x06 /* Public */,
       8,    0,   98,    2, 0x06 /* Public */,
       9,    2,   99,    2, 0x06 /* Public */,
      12,    1,  104,    2, 0x06 /* Public */,
      15,    1,  107,    2, 0x06 /* Public */,
      17,    1,  110,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      18,    2,  113,    2, 0x0a /* Public */,
      22,    2,  118,    2, 0x0a /* Public */,
      24,    2,  123,    2, 0x0a /* Public */,
      25,    0,  128,    2, 0x0a /* Public */,
      26,    0,  129,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   10,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 19, QMetaType::UShort,   20,   21,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   16,   23,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   16,   23,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Client::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Client *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->loggedIn(); break;
        case 2: _t->signedUp(); break;
        case 3: _t->signupError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->loginError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->disconnected(); break;
        case 6: _t->messageReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: _t->error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 8: _t->userJoined((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->userLeft((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->connectToServer((*reinterpret_cast< const QHostAddress(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2]))); break;
        case 11: _t->login((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 12: _t->signup((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 13: _t->disconnectFromHost(); break;
        case 14: _t->onReadyRead(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::connected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::loggedIn)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::signedUp)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Client::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::signupError)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Client::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::loginError)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::disconnected)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Client::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::messageReceived)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Client::*)(QAbstractSocket::SocketError );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::error)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Client::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::userJoined)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (Client::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::userLeft)) {
                *result = 9;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Client::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Client.data,
    qt_meta_data_Client,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Client::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Client::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Client.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void Client::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Client::loggedIn()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Client::signedUp()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Client::signupError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Client::loginError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Client::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Client::messageReceived(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Client::error(QAbstractSocket::SocketError _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Client::userJoined(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Client::userLeft(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

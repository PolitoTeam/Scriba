/****************************************************************************
** Meta object code from reading C++ file 'login.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Client/login.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'login.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Login_t {
    QByteArrayData data[15];
    char stringdata0[198];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Login_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Login_t qt_meta_stringdata_Login = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Login"
QT_MOC_LITERAL(1, 6, 6), // "access"
QT_MOC_LITERAL(2, 13, 0), // ""
QT_MOC_LITERAL(3, 14, 1), // "i"
QT_MOC_LITERAL(4, 16, 26), // "on_pushButtonLogin_clicked"
QT_MOC_LITERAL(5, 43, 31), // "on_pushButtonNewAccount_clicked"
QT_MOC_LITERAL(6, 75, 8), // "loggedIn"
QT_MOC_LITERAL(7, 84, 11), // "loginFailed"
QT_MOC_LITERAL(8, 96, 6), // "reason"
QT_MOC_LITERAL(9, 103, 5), // "error"
QT_MOC_LITERAL(10, 109, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(11, 138, 11), // "socketError"
QT_MOC_LITERAL(12, 150, 31), // "on_lineEditUsername_textChanged"
QT_MOC_LITERAL(13, 182, 4), // "arg1"
QT_MOC_LITERAL(14, 187, 10) // "disconnect"

    },
    "Login\0access\0\0i\0on_pushButtonLogin_clicked\0"
    "on_pushButtonNewAccount_clicked\0"
    "loggedIn\0loginFailed\0reason\0error\0"
    "QAbstractSocket::SocketError\0socketError\0"
    "on_lineEditUsername_textChanged\0arg1\0"
    "disconnect"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Login[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   57,    2, 0x08 /* Private */,
       5,    0,   58,    2, 0x08 /* Private */,
       6,    0,   59,    2, 0x08 /* Private */,
       7,    1,   60,    2, 0x08 /* Private */,
       9,    1,   63,    2, 0x08 /* Private */,
      12,    1,   66,    2, 0x08 /* Private */,
      14,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void,

       0        // eod
};

void Login::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Login *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->access((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_pushButtonLogin_clicked(); break;
        case 2: _t->on_pushButtonNewAccount_clicked(); break;
        case 3: _t->loggedIn(); break;
        case 4: _t->loginFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 6: _t->on_lineEditUsername_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->disconnect(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
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
            using _t = void (Login::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Login::access)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Login::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_Login.data,
    qt_meta_data_Login,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Login::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Login::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Login.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Login::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Login::access(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

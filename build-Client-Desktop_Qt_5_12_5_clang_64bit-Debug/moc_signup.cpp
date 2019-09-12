/****************************************************************************
** Meta object code from reading C++ file 'signup.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Client/signup.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'signup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Signup_t {
    QByteArrayData data[17];
    char stringdata0[355];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Signup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Signup_t qt_meta_stringdata_Signup = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Signup"
QT_MOC_LITERAL(1, 7, 6), // "action"
QT_MOC_LITERAL(2, 14, 0), // ""
QT_MOC_LITERAL(3, 15, 1), // "i"
QT_MOC_LITERAL(4, 17, 26), // "on_pushButtonClear_clicked"
QT_MOC_LITERAL(5, 44, 27), // "on_pushButtonSignup_clicked"
QT_MOC_LITERAL(6, 72, 35), // "on_lineEditUsername_editingFi..."
QT_MOC_LITERAL(7, 108, 35), // "on_lineEditPassword_editingFi..."
QT_MOC_LITERAL(8, 144, 42), // "on_lineEditConfirmPassword_ed..."
QT_MOC_LITERAL(9, 187, 31), // "on_lineEditUsername_textChanged"
QT_MOC_LITERAL(10, 219, 4), // "arg1"
QT_MOC_LITERAL(11, 224, 31), // "on_lineEditPassword_textChanged"
QT_MOC_LITERAL(12, 256, 38), // "on_lineEditConfirmPassword_te..."
QT_MOC_LITERAL(13, 295, 30), // "on_pushButtonBackLogin_clicked"
QT_MOC_LITERAL(14, 326, 8), // "signedUp"
QT_MOC_LITERAL(15, 335, 12), // "signupFailed"
QT_MOC_LITERAL(16, 348, 6) // "reason"

    },
    "Signup\0action\0\0i\0on_pushButtonClear_clicked\0"
    "on_pushButtonSignup_clicked\0"
    "on_lineEditUsername_editingFinished\0"
    "on_lineEditPassword_editingFinished\0"
    "on_lineEditConfirmPassword_editingFinished\0"
    "on_lineEditUsername_textChanged\0arg1\0"
    "on_lineEditPassword_textChanged\0"
    "on_lineEditConfirmPassword_textChanged\0"
    "on_pushButtonBackLogin_clicked\0signedUp\0"
    "signupFailed\0reason"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Signup[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   77,    2, 0x08 /* Private */,
       5,    0,   78,    2, 0x08 /* Private */,
       6,    0,   79,    2, 0x08 /* Private */,
       7,    0,   80,    2, 0x08 /* Private */,
       8,    0,   81,    2, 0x08 /* Private */,
       9,    1,   82,    2, 0x08 /* Private */,
      11,    1,   85,    2, 0x08 /* Private */,
      12,    1,   88,    2, 0x08 /* Private */,
      13,    0,   91,    2, 0x08 /* Private */,
      14,    0,   92,    2, 0x08 /* Private */,
      15,    1,   93,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,

       0        // eod
};

void Signup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Signup *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->action((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_pushButtonClear_clicked(); break;
        case 2: _t->on_pushButtonSignup_clicked(); break;
        case 3: _t->on_lineEditUsername_editingFinished(); break;
        case 4: _t->on_lineEditPassword_editingFinished(); break;
        case 5: _t->on_lineEditConfirmPassword_editingFinished(); break;
        case 6: _t->on_lineEditUsername_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->on_lineEditPassword_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->on_lineEditConfirmPassword_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->on_pushButtonBackLogin_clicked(); break;
        case 10: _t->signedUp(); break;
        case 11: _t->signupFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Signup::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Signup::action)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Signup::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_Signup.data,
    qt_meta_data_Signup,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Signup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Signup::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Signup.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Signup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Signup::action(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

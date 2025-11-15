/****************************************************************************
** Meta object code from reading C++ file 'TransactionWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/TransactionWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TransactionWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TransactionWidget_t {
    QByteArrayData data[18];
    char stringdata0[235];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TransactionWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TransactionWidget_t qt_meta_stringdata_TransactionWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "TransactionWidget"
QT_MOC_LITERAL(1, 18, 13), // "statusChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 23), // "QApt::TransactionStatus"
QT_MOC_LITERAL(4, 57, 6), // "status"
QT_MOC_LITERAL(5, 64, 24), // "transactionErrorOccurred"
QT_MOC_LITERAL(6, 89, 15), // "QApt::ErrorCode"
QT_MOC_LITERAL(7, 105, 5), // "error"
QT_MOC_LITERAL(8, 111, 13), // "provideMedium"
QT_MOC_LITERAL(9, 125, 5), // "label"
QT_MOC_LITERAL(10, 131, 6), // "medium"
QT_MOC_LITERAL(11, 138, 15), // "untrustedPrompt"
QT_MOC_LITERAL(12, 154, 17), // "untrustedPackages"
QT_MOC_LITERAL(13, 172, 18), // "configFileConflict"
QT_MOC_LITERAL(14, 191, 11), // "currentPath"
QT_MOC_LITERAL(15, 203, 7), // "newPath"
QT_MOC_LITERAL(16, 211, 14), // "updateProgress"
QT_MOC_LITERAL(17, 226, 8) // "progress"

    },
    "TransactionWidget\0statusChanged\0\0"
    "QApt::TransactionStatus\0status\0"
    "transactionErrorOccurred\0QApt::ErrorCode\0"
    "error\0provideMedium\0label\0medium\0"
    "untrustedPrompt\0untrustedPackages\0"
    "configFileConflict\0currentPath\0newPath\0"
    "updateProgress\0progress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TransactionWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x08 /* Private */,
       5,    1,   47,    2, 0x08 /* Private */,
       8,    2,   50,    2, 0x08 /* Private */,
      11,    1,   55,    2, 0x08 /* Private */,
      13,    2,   58,    2, 0x08 /* Private */,
      16,    1,   63,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   10,
    QMetaType::Void, QMetaType::QStringList,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   14,   15,
    QMetaType::Void, QMetaType::Int,   17,

       0        // eod
};

void TransactionWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TransactionWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->statusChanged((*reinterpret_cast< QApt::TransactionStatus(*)>(_a[1]))); break;
        case 1: _t->transactionErrorOccurred((*reinterpret_cast< QApt::ErrorCode(*)>(_a[1]))); break;
        case 2: _t->provideMedium((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->untrustedPrompt((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 4: _t->configFileConflict((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->updateProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TransactionWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_TransactionWidget.data,
    qt_meta_data_TransactionWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TransactionWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TransactionWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TransactionWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TransactionWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

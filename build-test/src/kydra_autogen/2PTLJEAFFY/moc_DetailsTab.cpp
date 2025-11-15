/****************************************************************************
** Meta object code from reading C++ file 'DetailsTab.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/DetailsTabs/DetailsTab.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DetailsTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DetailsTab_t {
    QByteArrayData data[10];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DetailsTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DetailsTab_t qt_meta_stringdata_DetailsTab = {
    {
QT_MOC_LITERAL(0, 0, 10), // "DetailsTab"
QT_MOC_LITERAL(1, 11, 10), // "setBackend"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 14), // "QApt::Backend*"
QT_MOC_LITERAL(4, 38, 7), // "backend"
QT_MOC_LITERAL(5, 46, 10), // "setPackage"
QT_MOC_LITERAL(6, 57, 14), // "QApt::Package*"
QT_MOC_LITERAL(7, 72, 7), // "package"
QT_MOC_LITERAL(8, 80, 7), // "refresh"
QT_MOC_LITERAL(9, 88, 5) // "clear"

    },
    "DetailsTab\0setBackend\0\0QApt::Backend*\0"
    "backend\0setPackage\0QApt::Package*\0"
    "package\0refresh\0clear"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DetailsTab[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       5,    1,   37,    2, 0x0a /* Public */,
       8,    0,   40,    2, 0x0a /* Public */,
       9,    0,   41,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DetailsTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DetailsTab *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->setBackend((*reinterpret_cast< QApt::Backend*(*)>(_a[1]))); break;
        case 1: _t->setPackage((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 2: _t->refresh(); break;
        case 3: _t->clear(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DetailsTab::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_DetailsTab.data,
    qt_meta_data_DetailsTab,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DetailsTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DetailsTab::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DetailsTab.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DetailsTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

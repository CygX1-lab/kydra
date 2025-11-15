/****************************************************************************
** Meta object code from reading C++ file 'ManagerWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/ManagerWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ManagerWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ManagerWidget_t {
    QByteArrayData data[12];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ManagerWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ManagerWidget_t qt_meta_stringdata_ManagerWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "ManagerWidget"
QT_MOC_LITERAL(1, 14, 6), // "reload"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 13), // "filterByGroup"
QT_MOC_LITERAL(4, 36, 9), // "groupName"
QT_MOC_LITERAL(5, 46, 14), // "filterByStatus"
QT_MOC_LITERAL(6, 61, 20), // "QApt::Package::State"
QT_MOC_LITERAL(7, 82, 5), // "state"
QT_MOC_LITERAL(8, 88, 14), // "filterByOrigin"
QT_MOC_LITERAL(9, 103, 10), // "originName"
QT_MOC_LITERAL(10, 114, 20), // "filterByArchitecture"
QT_MOC_LITERAL(11, 135, 4) // "arch"

    },
    "ManagerWidget\0reload\0\0filterByGroup\0"
    "groupName\0filterByStatus\0QApt::Package::State\0"
    "state\0filterByOrigin\0originName\0"
    "filterByArchitecture\0arch"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ManagerWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x0a /* Public */,
       3,    1,   40,    2, 0x0a /* Public */,
       5,    1,   43,    2, 0x0a /* Public */,
       8,    1,   46,    2, 0x0a /* Public */,
      10,    1,   49,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,   11,

       0        // eod
};

void ManagerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ManagerWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->reload(); break;
        case 1: _t->filterByGroup((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->filterByStatus((*reinterpret_cast< const QApt::Package::State(*)>(_a[1]))); break;
        case 3: _t->filterByOrigin((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->filterByArchitecture((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ManagerWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<PackageWidget::staticMetaObject>(),
    qt_meta_stringdata_ManagerWidget.data,
    qt_meta_data_ManagerWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ManagerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ManagerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ManagerWidget.stringdata0))
        return static_cast<void*>(this);
    return PackageWidget::qt_metacast(_clname);
}

int ManagerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PackageWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

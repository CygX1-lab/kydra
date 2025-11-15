/****************************************************************************
** Meta object code from reading C++ file 'FilterWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/FilterWidget/FilterWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FilterWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FilterWidget_t {
    QByteArrayData data[22];
    char stringdata0[287];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FilterWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FilterWidget_t qt_meta_stringdata_FilterWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "FilterWidget"
QT_MOC_LITERAL(1, 13, 13), // "filterByGroup"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 9), // "groupName"
QT_MOC_LITERAL(4, 38, 14), // "filterByStatus"
QT_MOC_LITERAL(5, 53, 20), // "QApt::Package::State"
QT_MOC_LITERAL(6, 74, 5), // "state"
QT_MOC_LITERAL(7, 80, 14), // "filterByOrigin"
QT_MOC_LITERAL(8, 95, 10), // "originName"
QT_MOC_LITERAL(9, 106, 20), // "filterByArchitecture"
QT_MOC_LITERAL(10, 127, 12), // "architecture"
QT_MOC_LITERAL(11, 140, 10), // "setBackend"
QT_MOC_LITERAL(12, 151, 14), // "QApt::Backend*"
QT_MOC_LITERAL(13, 166, 7), // "backend"
QT_MOC_LITERAL(14, 174, 6), // "reload"
QT_MOC_LITERAL(15, 181, 15), // "populateFilters"
QT_MOC_LITERAL(16, 197, 17), // "categoryActivated"
QT_MOC_LITERAL(17, 215, 11), // "QModelIndex"
QT_MOC_LITERAL(18, 227, 5), // "index"
QT_MOC_LITERAL(19, 233, 15), // "statusActivated"
QT_MOC_LITERAL(20, 249, 15), // "originActivated"
QT_MOC_LITERAL(21, 265, 21) // "architectureActivated"

    },
    "FilterWidget\0filterByGroup\0\0groupName\0"
    "filterByStatus\0QApt::Package::State\0"
    "state\0filterByOrigin\0originName\0"
    "filterByArchitecture\0architecture\0"
    "setBackend\0QApt::Backend*\0backend\0"
    "reload\0populateFilters\0categoryActivated\0"
    "QModelIndex\0index\0statusActivated\0"
    "originActivated\0architectureActivated"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FilterWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       4,    1,   72,    2, 0x06 /* Public */,
       7,    1,   75,    2, 0x06 /* Public */,
       9,    1,   78,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   81,    2, 0x0a /* Public */,
      14,    0,   84,    2, 0x0a /* Public */,
      15,    0,   85,    2, 0x08 /* Private */,
      16,    1,   86,    2, 0x08 /* Private */,
      19,    1,   89,    2, 0x08 /* Private */,
      20,    1,   92,    2, 0x08 /* Private */,
      21,    1,   95,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,   10,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 17,   18,

       0        // eod
};

void FilterWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FilterWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->filterByGroup((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->filterByStatus((*reinterpret_cast< const QApt::Package::State(*)>(_a[1]))); break;
        case 2: _t->filterByOrigin((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->filterByArchitecture((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->setBackend((*reinterpret_cast< QApt::Backend*(*)>(_a[1]))); break;
        case 5: _t->reload(); break;
        case 6: _t->populateFilters(); break;
        case 7: _t->categoryActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: _t->statusActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 9: _t->originActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 10: _t->architectureActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FilterWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FilterWidget::filterByGroup)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FilterWidget::*)(const QApt::Package::State );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FilterWidget::filterByStatus)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FilterWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FilterWidget::filterByOrigin)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FilterWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FilterWidget::filterByArchitecture)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FilterWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_FilterWidget.data,
    qt_meta_data_FilterWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FilterWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FilterWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FilterWidget.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int FilterWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void FilterWidget::filterByGroup(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FilterWidget::filterByStatus(const QApt::Package::State _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FilterWidget::filterByOrigin(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FilterWidget::filterByArchitecture(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

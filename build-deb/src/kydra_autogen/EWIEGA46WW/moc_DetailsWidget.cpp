/****************************************************************************
** Meta object code from reading C++ file 'DetailsWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/DetailsWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DetailsWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DetailsWidget_t {
    QByteArrayData data[18];
    char stringdata0[207];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DetailsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DetailsWidget_t qt_meta_stringdata_DetailsWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DetailsWidget"
QT_MOC_LITERAL(1, 14, 10), // "setInstall"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 14), // "QApt::Package*"
QT_MOC_LITERAL(4, 41, 7), // "package"
QT_MOC_LITERAL(5, 49, 9), // "setRemove"
QT_MOC_LITERAL(6, 59, 10), // "setUpgrade"
QT_MOC_LITERAL(7, 70, 12), // "setReInstall"
QT_MOC_LITERAL(8, 83, 7), // "setKeep"
QT_MOC_LITERAL(9, 91, 8), // "setPurge"
QT_MOC_LITERAL(10, 100, 21), // "emitHideButtonsSignal"
QT_MOC_LITERAL(11, 122, 10), // "setBackend"
QT_MOC_LITERAL(12, 133, 14), // "QApt::Backend*"
QT_MOC_LITERAL(13, 148, 7), // "backend"
QT_MOC_LITERAL(14, 156, 10), // "setPackage"
QT_MOC_LITERAL(15, 167, 17), // "refreshCurrentTab"
QT_MOC_LITERAL(16, 185, 5), // "clear"
QT_MOC_LITERAL(17, 191, 15) // "emitHideButtons"

    },
    "DetailsWidget\0setInstall\0\0QApt::Package*\0"
    "package\0setRemove\0setUpgrade\0setReInstall\0"
    "setKeep\0setPurge\0emitHideButtonsSignal\0"
    "setBackend\0QApt::Backend*\0backend\0"
    "setPackage\0refreshCurrentTab\0clear\0"
    "emitHideButtons"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DetailsWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       5,    1,   77,    2, 0x06 /* Public */,
       6,    1,   80,    2, 0x06 /* Public */,
       7,    1,   83,    2, 0x06 /* Public */,
       8,    1,   86,    2, 0x06 /* Public */,
       9,    1,   89,    2, 0x06 /* Public */,
      10,    0,   92,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   93,    2, 0x0a /* Public */,
      14,    1,   96,    2, 0x0a /* Public */,
      15,    0,   99,    2, 0x0a /* Public */,
      16,    0,  100,    2, 0x0a /* Public */,
      17,    0,  101,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DetailsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DetailsWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->setInstall((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 1: _t->setRemove((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 2: _t->setUpgrade((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 3: _t->setReInstall((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 4: _t->setKeep((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 5: _t->setPurge((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 6: _t->emitHideButtonsSignal(); break;
        case 7: _t->setBackend((*reinterpret_cast< QApt::Backend*(*)>(_a[1]))); break;
        case 8: _t->setPackage((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 9: _t->refreshCurrentTab(); break;
        case 10: _t->clear(); break;
        case 11: _t->emitHideButtons(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DetailsWidget::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DetailsWidget::setInstall)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DetailsWidget::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DetailsWidget::setRemove)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DetailsWidget::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DetailsWidget::setUpgrade)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DetailsWidget::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DetailsWidget::setReInstall)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DetailsWidget::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DetailsWidget::setKeep)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DetailsWidget::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DetailsWidget::setPurge)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (DetailsWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DetailsWidget::emitHideButtonsSignal)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DetailsWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QTabWidget::staticMetaObject>(),
    qt_meta_stringdata_DetailsWidget.data,
    qt_meta_data_DetailsWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DetailsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DetailsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DetailsWidget.stringdata0))
        return static_cast<void*>(this);
    return QTabWidget::qt_metacast(_clname);
}

int DetailsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
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
void DetailsWidget::setInstall(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DetailsWidget::setRemove(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DetailsWidget::setUpgrade(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DetailsWidget::setReInstall(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DetailsWidget::setKeep(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DetailsWidget::setPurge(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DetailsWidget::emitHideButtonsSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

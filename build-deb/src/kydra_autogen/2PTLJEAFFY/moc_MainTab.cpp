/****************************************************************************
** Meta object code from reading C++ file 'MainTab.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/DetailsTabs/MainTab.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainTab_t {
    QByteArrayData data[18];
    char stringdata0[200];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainTab_t qt_meta_stringdata_MainTab = {
    {
QT_MOC_LITERAL(0, 0, 7), // "MainTab"
QT_MOC_LITERAL(1, 8, 10), // "setInstall"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 14), // "QApt::Package*"
QT_MOC_LITERAL(4, 35, 7), // "package"
QT_MOC_LITERAL(5, 43, 9), // "setRemove"
QT_MOC_LITERAL(6, 53, 10), // "setUpgrade"
QT_MOC_LITERAL(7, 64, 12), // "setReInstall"
QT_MOC_LITERAL(8, 77, 7), // "setKeep"
QT_MOC_LITERAL(9, 85, 8), // "setPurge"
QT_MOC_LITERAL(10, 94, 7), // "refresh"
QT_MOC_LITERAL(11, 102, 14), // "emitSetInstall"
QT_MOC_LITERAL(12, 117, 13), // "emitSetRemove"
QT_MOC_LITERAL(13, 131, 14), // "emitSetUpgrade"
QT_MOC_LITERAL(14, 146, 16), // "emitSetReInstall"
QT_MOC_LITERAL(15, 163, 12), // "emitSetPurge"
QT_MOC_LITERAL(16, 176, 11), // "emitSetKeep"
QT_MOC_LITERAL(17, 188, 11) // "hideButtons"

    },
    "MainTab\0setInstall\0\0QApt::Package*\0"
    "package\0setRemove\0setUpgrade\0setReInstall\0"
    "setKeep\0setPurge\0refresh\0emitSetInstall\0"
    "emitSetRemove\0emitSetUpgrade\0"
    "emitSetReInstall\0emitSetPurge\0emitSetKeep\0"
    "hideButtons"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainTab[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,
       5,    1,   87,    2, 0x06 /* Public */,
       6,    1,   90,    2, 0x06 /* Public */,
       7,    1,   93,    2, 0x06 /* Public */,
       8,    1,   96,    2, 0x06 /* Public */,
       9,    1,   99,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,  102,    2, 0x0a /* Public */,
      11,    0,  103,    2, 0x08 /* Private */,
      12,    0,  104,    2, 0x08 /* Private */,
      13,    0,  105,    2, 0x08 /* Private */,
      14,    0,  106,    2, 0x08 /* Private */,
      15,    0,  107,    2, 0x08 /* Private */,
      16,    0,  108,    2, 0x08 /* Private */,
      17,    0,  109,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainTab *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->setInstall((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 1: _t->setRemove((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 2: _t->setUpgrade((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 3: _t->setReInstall((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 4: _t->setKeep((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 5: _t->setPurge((*reinterpret_cast< QApt::Package*(*)>(_a[1]))); break;
        case 6: _t->refresh(); break;
        case 7: _t->emitSetInstall(); break;
        case 8: _t->emitSetRemove(); break;
        case 9: _t->emitSetUpgrade(); break;
        case 10: _t->emitSetReInstall(); break;
        case 11: _t->emitSetPurge(); break;
        case 12: _t->emitSetKeep(); break;
        case 13: _t->hideButtons(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainTab::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainTab::setInstall)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainTab::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainTab::setRemove)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MainTab::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainTab::setUpgrade)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MainTab::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainTab::setReInstall)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MainTab::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainTab::setKeep)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MainTab::*)(QApt::Package * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainTab::setPurge)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainTab::staticMetaObject = { {
    QMetaObject::SuperData::link<DetailsTab::staticMetaObject>(),
    qt_meta_stringdata_MainTab.data,
    qt_meta_data_MainTab,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainTab::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainTab.stringdata0))
        return static_cast<void*>(this);
    return DetailsTab::qt_metacast(_clname);
}

int MainTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DetailsTab::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void MainTab::setInstall(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainTab::setRemove(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MainTab::setUpgrade(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MainTab::setReInstall(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MainTab::setKeep(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MainTab::setPurge(QApt::Package * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

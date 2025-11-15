/****************************************************************************
** Meta object code from reading C++ file 'ManagerSettingsDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/config/ManagerSettingsDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ManagerSettingsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ManagerSettingsDialog_t {
    QByteArrayData data[8];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ManagerSettingsDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ManagerSettingsDialog_t qt_meta_stringdata_ManagerSettingsDialog = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ManagerSettingsDialog"
QT_MOC_LITERAL(1, 22, 15), // "settingsChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 17), // "slotButtonClicked"
QT_MOC_LITERAL(4, 57, 16), // "QAbstractButton*"
QT_MOC_LITERAL(5, 74, 6), // "button"
QT_MOC_LITERAL(6, 81, 7), // "changed"
QT_MOC_LITERAL(7, 89, 11) // "authChanged"

    },
    "ManagerSettingsDialog\0settingsChanged\0"
    "\0slotButtonClicked\0QAbstractButton*\0"
    "button\0changed\0authChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ManagerSettingsDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   35,    2, 0x08 /* Private */,
       6,    0,   38,    2, 0x08 /* Private */,
       7,    0,   39,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ManagerSettingsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ManagerSettingsDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->settingsChanged(); break;
        case 1: _t->slotButtonClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 2: _t->changed(); break;
        case 3: _t->authChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ManagerSettingsDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ManagerSettingsDialog::settingsChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ManagerSettingsDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<KPageDialog::staticMetaObject>(),
    qt_meta_stringdata_ManagerSettingsDialog.data,
    qt_meta_data_ManagerSettingsDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ManagerSettingsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ManagerSettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ManagerSettingsDialog.stringdata0))
        return static_cast<void*>(this);
    return KPageDialog::qt_metacast(_clname);
}

int ManagerSettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KPageDialog::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void ManagerSettingsDialog::settingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

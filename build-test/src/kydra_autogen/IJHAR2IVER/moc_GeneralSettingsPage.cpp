/****************************************************************************
** Meta object code from reading C++ file 'GeneralSettingsPage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/config/GeneralSettingsPage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GeneralSettingsPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GeneralSettingsPage_t {
    QByteArrayData data[8];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GeneralSettingsPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GeneralSettingsPage_t qt_meta_stringdata_GeneralSettingsPage = {
    {
QT_MOC_LITERAL(0, 0, 19), // "GeneralSettingsPage"
QT_MOC_LITERAL(1, 20, 31), // "versionColumnsVisibilityChanged"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 7), // "visible"
QT_MOC_LITERAL(4, 61, 15), // "emitAuthChanged"
QT_MOC_LITERAL(5, 77, 28), // "updateAutoCleanSpinboxSuffix"
QT_MOC_LITERAL(6, 106, 16), // "editStatusColors"
QT_MOC_LITERAL(7, 123, 26) // "applyVersionColumnsSetting"

    },
    "GeneralSettingsPage\0versionColumnsVisibilityChanged\0"
    "\0visible\0emitAuthChanged\0"
    "updateAutoCleanSpinboxSuffix\0"
    "editStatusColors\0applyVersionColumnsSetting"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GeneralSettingsPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   42,    2, 0x08 /* Private */,
       5,    0,   43,    2, 0x08 /* Private */,
       6,    0,   44,    2, 0x08 /* Private */,
       7,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GeneralSettingsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GeneralSettingsPage *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->versionColumnsVisibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->emitAuthChanged(); break;
        case 2: _t->updateAutoCleanSpinboxSuffix(); break;
        case 3: _t->editStatusColors(); break;
        case 4: _t->applyVersionColumnsSetting(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GeneralSettingsPage::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GeneralSettingsPage::versionColumnsVisibilityChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GeneralSettingsPage::staticMetaObject = { {
    QMetaObject::SuperData::link<SettingsPageBase::staticMetaObject>(),
    qt_meta_stringdata_GeneralSettingsPage.data,
    qt_meta_data_GeneralSettingsPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GeneralSettingsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GeneralSettingsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GeneralSettingsPage.stringdata0))
        return static_cast<void*>(this);
    return SettingsPageBase::qt_metacast(_clname);
}

int GeneralSettingsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SettingsPageBase::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void GeneralSettingsPage::versionColumnsVisibilityChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

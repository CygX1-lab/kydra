/****************************************************************************
** Meta object code from reading C++ file 'RepositorySettingsPage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/config/RepositorySettingsPage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RepositorySettingsPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RepositorySettingsPage_t {
    QByteArrayData data[7];
    char stringdata0[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RepositorySettingsPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RepositorySettingsPage_t qt_meta_stringdata_RepositorySettingsPage = {
    {
QT_MOC_LITERAL(0, 0, 22), // "RepositorySettingsPage"
QT_MOC_LITERAL(1, 23, 13), // "addRepository"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 16), // "removeRepository"
QT_MOC_LITERAL(4, 55, 14), // "editRepository"
QT_MOC_LITERAL(5, 70, 20), // "updateLocalDebFolder"
QT_MOC_LITERAL(6, 91, 28) // "onRepositorySelectionChanged"

    },
    "RepositorySettingsPage\0addRepository\0"
    "\0removeRepository\0editRepository\0"
    "updateLocalDebFolder\0onRepositorySelectionChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RepositorySettingsPage[] = {

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
       1,    0,   39,    2, 0x08 /* Private */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    0,   41,    2, 0x08 /* Private */,
       5,    0,   42,    2, 0x08 /* Private */,
       6,    0,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RepositorySettingsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RepositorySettingsPage *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->addRepository(); break;
        case 1: _t->removeRepository(); break;
        case 2: _t->editRepository(); break;
        case 3: _t->updateLocalDebFolder(); break;
        case 4: _t->onRepositorySelectionChanged(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject RepositorySettingsPage::staticMetaObject = { {
    QMetaObject::SuperData::link<SettingsPageBase::staticMetaObject>(),
    qt_meta_stringdata_RepositorySettingsPage.data,
    qt_meta_data_RepositorySettingsPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RepositorySettingsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RepositorySettingsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RepositorySettingsPage.stringdata0))
        return static_cast<void*>(this);
    return SettingsPageBase::qt_metacast(_clname);
}

int RepositorySettingsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE

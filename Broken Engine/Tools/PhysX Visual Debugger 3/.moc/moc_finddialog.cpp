/****************************************************************************
** Meta object code from reading C++ file 'finddialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../PVDNext/finddialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'finddialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FindDialog_t {
    QByteArrayData data[11];
    char stringdata0[214];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FindDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FindDialog_t qt_meta_stringdata_FindDialog = {
    {
QT_MOC_LITERAL(0, 0, 10), // "FindDialog"
QT_MOC_LITERAL(1, 11, 20), // "on_selection_changed"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 14), // "QItemSelection"
QT_MOC_LITERAL(4, 48, 21), // "on_findButton_clicked"
QT_MOC_LITERAL(5, 70, 23), // "on_selectButton_clicked"
QT_MOC_LITERAL(6, 94, 23), // "on_cancelButton_clicked"
QT_MOC_LITERAL(7, 118, 37), // "on_findByComboBox_currentInde..."
QT_MOC_LITERAL(8, 156, 5), // "index"
QT_MOC_LITERAL(9, 162, 25), // "on_findEdit_returnPressed"
QT_MOC_LITERAL(10, 188, 25) // "on_findTreeView_activated"

    },
    "FindDialog\0on_selection_changed\0\0"
    "QItemSelection\0on_findButton_clicked\0"
    "on_selectButton_clicked\0on_cancelButton_clicked\0"
    "on_findByComboBox_currentIndexChanged\0"
    "index\0on_findEdit_returnPressed\0"
    "on_findTreeView_activated"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FindDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x08 /* Private */,
       4,    0,   52,    2, 0x08 /* Private */,
       5,    0,   53,    2, 0x08 /* Private */,
       6,    0,   54,    2, 0x08 /* Private */,
       7,    1,   55,    2, 0x08 /* Private */,
       9,    0,   58,    2, 0x08 /* Private */,
      10,    1,   59,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    8,

       0        // eod
};

void FindDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FindDialog *_t = static_cast<FindDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_selection_changed((*reinterpret_cast< const QItemSelection(*)>(_a[1]))); break;
        case 1: _t->on_findButton_clicked(); break;
        case 2: _t->on_selectButton_clicked(); break;
        case 3: _t->on_cancelButton_clicked(); break;
        case 4: _t->on_findByComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_findEdit_returnPressed(); break;
        case 6: _t->on_findTreeView_activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject FindDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FindDialog.data,
      qt_meta_data_FindDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FindDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FindDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FindDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FindDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

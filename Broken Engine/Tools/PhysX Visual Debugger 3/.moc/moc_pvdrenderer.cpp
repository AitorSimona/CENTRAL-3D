/****************************************************************************
** Meta object code from reading C++ file 'pvdrenderer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../PVDNext/pvdrenderer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pvdrenderer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PVDRenderer_t {
    QByteArrayData data[22];
    char stringdata0[311];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PVDRenderer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PVDRenderer_t qt_meta_stringdata_PVDRenderer = {
    {
QT_MOC_LITERAL(0, 0, 11), // "PVDRenderer"
QT_MOC_LITERAL(1, 12, 13), // "trackSelected"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 10), // "pickObject"
QT_MOC_LITERAL(4, 38, 8), // "objectId"
QT_MOC_LITERAL(5, 47, 16), // "isAddToSelection"
QT_MOC_LITERAL(6, 64, 7), // "painted"
QT_MOC_LITERAL(7, 72, 17), // "onSettingsUpdated"
QT_MOC_LITERAL(8, 90, 15), // "onTrackSelected"
QT_MOC_LITERAL(9, 106, 13), // "onResetCamera"
QT_MOC_LITERAL(10, 120, 13), // "onZoomToScene"
QT_MOC_LITERAL(11, 134, 18), // "onCenterAtSelected"
QT_MOC_LITERAL(12, 153, 16), // "onZoomToSelected"
QT_MOC_LITERAL(13, 170, 13), // "onLookAtPoint"
QT_MOC_LITERAL(14, 184, 21), // "onInstanceViewUpdated"
QT_MOC_LITERAL(15, 206, 19), // "shouldTrackSelected"
QT_MOC_LITERAL(16, 226, 14), // "onObjectPicked"
QT_MOC_LITERAL(17, 241, 17), // "shouldSetupCamera"
QT_MOC_LITERAL(18, 259, 15), // "onRenderCtxMenu"
QT_MOC_LITERAL(19, 275, 12), // "QMouseEvent*"
QT_MOC_LITERAL(20, 288, 5), // "event"
QT_MOC_LITERAL(21, 294, 16) // "initCameraOffset"

    },
    "PVDRenderer\0trackSelected\0\0pickObject\0"
    "objectId\0isAddToSelection\0painted\0"
    "onSettingsUpdated\0onTrackSelected\0"
    "onResetCamera\0onZoomToScene\0"
    "onCenterAtSelected\0onZoomToSelected\0"
    "onLookAtPoint\0onInstanceViewUpdated\0"
    "shouldTrackSelected\0onObjectPicked\0"
    "shouldSetupCamera\0onRenderCtxMenu\0"
    "QMouseEvent*\0event\0initCameraOffset"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PVDRenderer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,
       3,    2,   87,    2, 0x06 /* Public */,
       6,    0,   92,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   93,    2, 0x0a /* Public */,
       8,    1,   94,    2, 0x0a /* Public */,
       9,    0,   97,    2, 0x0a /* Public */,
      10,    0,   98,    2, 0x0a /* Public */,
      11,    0,   99,    2, 0x0a /* Public */,
      12,    0,  100,    2, 0x0a /* Public */,
      13,    0,  101,    2, 0x0a /* Public */,
      14,    1,  102,    2, 0x0a /* Public */,
      16,    1,  105,    2, 0x0a /* Public */,
      18,    1,  108,    2, 0x0a /* Public */,
      21,    0,  111,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    4,    5,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   17,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void,

       0        // eod
};

void PVDRenderer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PVDRenderer *_t = static_cast<PVDRenderer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->trackSelected((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->pickObject((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->painted(); break;
        case 3: _t->onSettingsUpdated(); break;
        case 4: _t->onTrackSelected((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->onResetCamera(); break;
        case 6: _t->onZoomToScene(); break;
        case 7: _t->onCenterAtSelected(); break;
        case 8: _t->onZoomToSelected(); break;
        case 9: _t->onLookAtPoint(); break;
        case 10: _t->onInstanceViewUpdated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->onObjectPicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->onRenderCtxMenu((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 13: _t->initCameraOffset(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (PVDRenderer::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PVDRenderer::trackSelected)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (PVDRenderer::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PVDRenderer::pickObject)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (PVDRenderer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PVDRenderer::painted)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject PVDRenderer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PVDRenderer.data,
      qt_meta_data_PVDRenderer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *PVDRenderer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PVDRenderer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PVDRenderer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PVDRenderer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void PVDRenderer::trackSelected(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PVDRenderer::pickObject(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PVDRenderer::painted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

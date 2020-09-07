/****************************************************************************
** Meta object code from reading C++ file 'geometry.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../jerko/geometry.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'geometry.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Geometry_t {
    QByteArrayData data[24];
    char stringdata0[206];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Geometry_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Geometry_t qt_meta_stringdata_Geometry = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Geometry"
QT_MOC_LITERAL(1, 9, 15), // "positionChanged"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 15), // "rotationChanged"
QT_MOC_LITERAL(4, 42, 4), // "sync"
QT_MOC_LITERAL(5, 47, 7), // "cleanup"
QT_MOC_LITERAL(6, 55, 10), // "newProject"
QT_MOC_LITERAL(7, 66, 4), // "open"
QT_MOC_LITERAL(8, 71, 4), // "save"
QT_MOC_LITERAL(9, 76, 11), // "importModel"
QT_MOC_LITERAL(10, 88, 8), // "filename"
QT_MOC_LITERAL(11, 97, 11), // "exportModel"
QT_MOC_LITERAL(12, 109, 4), // "undo"
QT_MOC_LITERAL(13, 114, 4), // "redo"
QT_MOC_LITERAL(14, 119, 3), // "cut"
QT_MOC_LITERAL(15, 123, 4), // "copy"
QT_MOC_LITERAL(16, 128, 5), // "paste"
QT_MOC_LITERAL(17, 134, 19), // "handleWindowChanged"
QT_MOC_LITERAL(18, 154, 13), // "QQuickWindow*"
QT_MOC_LITERAL(19, 168, 3), // "win"
QT_MOC_LITERAL(20, 172, 7), // "unsaved"
QT_MOC_LITERAL(21, 180, 7), // "canSave"
QT_MOC_LITERAL(22, 188, 8), // "position"
QT_MOC_LITERAL(23, 197, 8) // "rotation"

    },
    "Geometry\0positionChanged\0\0rotationChanged\0"
    "sync\0cleanup\0newProject\0open\0save\0"
    "importModel\0filename\0exportModel\0undo\0"
    "redo\0cut\0copy\0paste\0handleWindowChanged\0"
    "QQuickWindow*\0win\0unsaved\0canSave\0"
    "position\0rotation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Geometry[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       2,  126, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x06 /* Public */,
       3,    0,  100,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,  101,    2, 0x0a /* Public */,
       5,    0,  102,    2, 0x0a /* Public */,
       6,    0,  103,    2, 0x0a /* Public */,
       7,    1,  104,    2, 0x0a /* Public */,
       8,    1,  107,    2, 0x0a /* Public */,
       9,    1,  110,    2, 0x0a /* Public */,
      11,    1,  113,    2, 0x0a /* Public */,
      12,    0,  116,    2, 0x0a /* Public */,
      13,    0,  117,    2, 0x0a /* Public */,
      14,    0,  118,    2, 0x0a /* Public */,
      15,    0,  119,    2, 0x0a /* Public */,
      16,    0,  120,    2, 0x0a /* Public */,
      17,    1,  121,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      20,    0,  124,    2, 0x02 /* Public */,
      21,    0,  125,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18,   19,

 // methods: parameters
    QMetaType::Bool,
    QMetaType::Bool,

 // properties: name, type, flags
      22, QMetaType::QVector3D, 0x00495103,
      23, QMetaType::QQuaternion, 0x00495103,

 // properties: notify_signal_id
       0,
       1,

       0        // eod
};

void Geometry::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Geometry *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->positionChanged(); break;
        case 1: _t->rotationChanged(); break;
        case 2: _t->sync(); break;
        case 3: _t->cleanup(); break;
        case 4: _t->newProject(); break;
        case 5: _t->open((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->save((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->importModel((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->exportModel((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->undo(); break;
        case 10: _t->redo(); break;
        case 11: _t->cut(); break;
        case 12: _t->copy(); break;
        case 13: _t->paste(); break;
        case 14: _t->handleWindowChanged((*reinterpret_cast< QQuickWindow*(*)>(_a[1]))); break;
        case 15: { bool _r = _t->unsaved();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 16: { bool _r = _t->canSave();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QQuickWindow* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Geometry::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Geometry::positionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Geometry::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Geometry::rotationChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Geometry *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVector3D*>(_v) = _t->position(); break;
        case 1: *reinterpret_cast< QQuaternion*>(_v) = _t->rotation(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<Geometry *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setPosition(*reinterpret_cast< QVector3D*>(_v)); break;
        case 1: _t->setRotation(*reinterpret_cast< QQuaternion*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Geometry::staticMetaObject = { {
    QMetaObject::SuperData::link<QQuickItem::staticMetaObject>(),
    qt_meta_stringdata_Geometry.data,
    qt_meta_data_Geometry,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Geometry::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Geometry::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Geometry.stringdata0))
        return static_cast<void*>(this);
    return QQuickItem::qt_metacast(_clname);
}

int Geometry::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QQuickItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Geometry::positionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Geometry::rotationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

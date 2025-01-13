/****************************************************************************
** Meta object code from reading C++ file 'FeatureTracker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../analysis/FeatureTracker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FeatureTracker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FeatureTracker_t {
    QByteArrayData data[9];
    char stringdata0[124];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FeatureTracker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FeatureTracker_t qt_meta_stringdata_FeatureTracker = {
    {
QT_MOC_LITERAL(0, 0, 14), // "FeatureTracker"
QT_MOC_LITERAL(1, 15, 17), // "framesDirSelected"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 4), // "file"
QT_MOC_LITERAL(4, 39, 15), // "posFileSelected"
QT_MOC_LITERAL(5, 55, 17), // "showPosFileDialog"
QT_MOC_LITERAL(6, 73, 19), // "showFramesDirDialog"
QT_MOC_LITERAL(7, 93, 19), // "startTrackingFrames"
QT_MOC_LITERAL(8, 113, 10) // "endSession"

    },
    "FeatureTracker\0framesDirSelected\0\0"
    "file\0posFileSelected\0showPosFileDialog\0"
    "showFramesDirDialog\0startTrackingFrames\0"
    "endSession"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FeatureTracker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x08 /* Private */,
       4,    1,   47,    2, 0x08 /* Private */,
       5,    0,   50,    2, 0x08 /* Private */,
       6,    0,   51,    2, 0x08 /* Private */,
       7,    0,   52,    2, 0x08 /* Private */,
       8,    0,   53,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FeatureTracker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FeatureTracker *_t = static_cast<FeatureTracker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->framesDirSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->posFileSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->showPosFileDialog(); break;
        case 3: _t->showFramesDirDialog(); break;
        case 4: _t->startTrackingFrames(); break;
        case 5: _t->endSession(); break;
        default: ;
        }
    }
}

const QMetaObject FeatureTracker::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FeatureTracker.data,
      qt_meta_data_FeatureTracker,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FeatureTracker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FeatureTracker::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FeatureTracker.stringdata0))
        return static_cast<void*>(const_cast< FeatureTracker*>(this));
    return QWidget::qt_metacast(_clname);
}

int FeatureTracker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

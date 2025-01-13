/****************************************************************************
** Meta object code from reading C++ file 'FrequencySelector.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../analysis/FrequencySelector.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FrequencySelector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FrequencySelector_t {
    QByteArrayData data[10];
    char stringdata0[125];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FrequencySelector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FrequencySelector_t qt_meta_stringdata_FrequencySelector = {
    {
QT_MOC_LITERAL(0, 0, 17), // "FrequencySelector"
QT_MOC_LITERAL(1, 18, 12), // "selectXPoint"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 5), // "point"
QT_MOC_LITERAL(4, 38, 12), // "selectYPoint"
QT_MOC_LITERAL(5, 51, 14), // "clearSelection"
QT_MOC_LITERAL(6, 66, 15), // "deleteSelection"
QT_MOC_LITERAL(7, 82, 13), // "saveSelection"
QT_MOC_LITERAL(8, 96, 12), // "addSelection"
QT_MOC_LITERAL(9, 109, 15) // "startSimulation"

    },
    "FrequencySelector\0selectXPoint\0\0point\0"
    "selectYPoint\0clearSelection\0deleteSelection\0"
    "saveSelection\0addSelection\0startSimulation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FrequencySelector[] = {

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
       4,    1,   52,    2, 0x08 /* Private */,
       5,    0,   55,    2, 0x08 /* Private */,
       6,    0,   56,    2, 0x08 /* Private */,
       7,    0,   57,    2, 0x08 /* Private */,
       8,    0,   58,    2, 0x08 /* Private */,
       9,    0,   59,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QPointF,    3,
    QMetaType::Void, QMetaType::QPointF,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FrequencySelector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FrequencySelector *_t = static_cast<FrequencySelector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectXPoint((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 1: _t->selectYPoint((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 2: _t->clearSelection(); break;
        case 3: _t->deleteSelection(); break;
        case 4: _t->saveSelection(); break;
        case 5: _t->addSelection(); break;
        case 6: _t->startSimulation(); break;
        default: ;
        }
    }
}

const QMetaObject FrequencySelector::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FrequencySelector.data,
      qt_meta_data_FrequencySelector,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FrequencySelector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FrequencySelector::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FrequencySelector.stringdata0))
        return static_cast<void*>(const_cast< FrequencySelector*>(this));
    return QWidget::qt_metacast(_clname);
}

int FrequencySelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

/****************************************************************************
** Meta object code from reading C++ file 'RunDebugView.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "debug/RunDebugView.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RunDebugView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12RunDebugViewE_t {};
} // unnamed namespace

template <> constexpr inline auto RunDebugView::qt_create_metaobjectdata<qt_meta_tag_ZN12RunDebugViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RunDebugView",
        "activeConfigurationChanged",
        "",
        "index",
        "buildTriggered",
        "runTriggered",
        "stopTriggered",
        "editConfigurationRequested",
        "stepOverTriggered",
        "stepIntoTriggered",
        "stepOutTriggered",
        "onComboChanged"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'activeConfigurationChanged'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'buildTriggered'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'runTriggered'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stopTriggered'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'editConfigurationRequested'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stepOverTriggered'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stepIntoTriggered'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stepOutTriggered'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onComboChanged'
        QtMocHelpers::SlotData<void(int)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RunDebugView, qt_meta_tag_ZN12RunDebugViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RunDebugView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12RunDebugViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12RunDebugViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12RunDebugViewE_t>.metaTypes,
    nullptr
} };

void RunDebugView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RunDebugView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->activeConfigurationChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->buildTriggered(); break;
        case 2: _t->runTriggered(); break;
        case 3: _t->stopTriggered(); break;
        case 4: _t->editConfigurationRequested(); break;
        case 5: _t->stepOverTriggered(); break;
        case 6: _t->stepIntoTriggered(); break;
        case 7: _t->stepOutTriggered(); break;
        case 8: _t->onComboChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RunDebugView::*)(int )>(_a, &RunDebugView::activeConfigurationChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RunDebugView::*)()>(_a, &RunDebugView::buildTriggered, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RunDebugView::*)()>(_a, &RunDebugView::runTriggered, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RunDebugView::*)()>(_a, &RunDebugView::stopTriggered, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RunDebugView::*)()>(_a, &RunDebugView::editConfigurationRequested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (RunDebugView::*)()>(_a, &RunDebugView::stepOverTriggered, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (RunDebugView::*)()>(_a, &RunDebugView::stepIntoTriggered, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (RunDebugView::*)()>(_a, &RunDebugView::stepOutTriggered, 7))
            return;
    }
}

const QMetaObject *RunDebugView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RunDebugView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12RunDebugViewE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RunDebugView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void RunDebugView::activeConfigurationChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void RunDebugView::buildTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RunDebugView::runTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RunDebugView::stopTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void RunDebugView::editConfigurationRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void RunDebugView::stepOverTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void RunDebugView::stepIntoTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void RunDebugView::stepOutTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP

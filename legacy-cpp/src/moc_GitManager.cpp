/****************************************************************************
** Meta object code from reading C++ file 'GitManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "scm/GitManager.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GitManager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10GitManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto GitManager::qt_create_metaobjectdata<qt_meta_tag_ZN10GitManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "GitManager",
        "statusChanged",
        "",
        "QList<GitFileStatus>",
        "status",
        "logChanged",
        "QList<GitCommit>",
        "log",
        "branchesChanged",
        "branches",
        "currentBranch",
        "commitCompleted",
        "success",
        "error",
        "pullCompleted",
        "output",
        "pushCompleted",
        "operationFailed",
        "op",
        "stderrText",
        "isGitRepo"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'statusChanged'
        QtMocHelpers::SignalData<void(const QVector<GitFileStatus> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'logChanged'
        QtMocHelpers::SignalData<void(const QVector<GitCommit> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'branchesChanged'
        QtMocHelpers::SignalData<void(const QStringList &, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 9 }, { QMetaType::QString, 10 },
        }}),
        // Signal 'commitCompleted'
        QtMocHelpers::SignalData<void(bool, const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 12 }, { QMetaType::QString, 13 },
        }}),
        // Signal 'pullCompleted'
        QtMocHelpers::SignalData<void(bool, const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 12 }, { QMetaType::QString, 15 },
        }}),
        // Signal 'pushCompleted'
        QtMocHelpers::SignalData<void(bool, const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 12 }, { QMetaType::QString, 15 },
        }}),
        // Signal 'operationFailed'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { QMetaType::QString, 19 },
        }}),
        // Signal 'isGitRepo'
        QtMocHelpers::SignalData<void(bool)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<GitManager, qt_meta_tag_ZN10GitManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject GitManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10GitManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10GitManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10GitManagerE_t>.metaTypes,
    nullptr
} };

void GitManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<GitManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->statusChanged((*reinterpret_cast<std::add_pointer_t<QList<GitFileStatus>>>(_a[1]))); break;
        case 1: _t->logChanged((*reinterpret_cast<std::add_pointer_t<QList<GitCommit>>>(_a[1]))); break;
        case 2: _t->branchesChanged((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->commitCompleted((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->pullCompleted((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->pushCompleted((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 6: _t->operationFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->isGitRepo((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (GitManager::*)(const QVector<GitFileStatus> & )>(_a, &GitManager::statusChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (GitManager::*)(const QVector<GitCommit> & )>(_a, &GitManager::logChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (GitManager::*)(const QStringList & , const QString & )>(_a, &GitManager::branchesChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (GitManager::*)(bool , const QString & )>(_a, &GitManager::commitCompleted, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (GitManager::*)(bool , const QString & )>(_a, &GitManager::pullCompleted, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (GitManager::*)(bool , const QString & )>(_a, &GitManager::pushCompleted, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (GitManager::*)(const QString & , const QString & )>(_a, &GitManager::operationFailed, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (GitManager::*)(bool )>(_a, &GitManager::isGitRepo, 7))
            return;
    }
}

const QMetaObject *GitManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GitManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10GitManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GitManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void GitManager::statusChanged(const QVector<GitFileStatus> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void GitManager::logChanged(const QVector<GitCommit> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void GitManager::branchesChanged(const QStringList & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void GitManager::commitCompleted(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void GitManager::pullCompleted(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void GitManager::pushCompleted(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void GitManager::operationFailed(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void GitManager::isGitRepo(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}
QT_WARNING_POP

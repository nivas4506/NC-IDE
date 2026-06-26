/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainWindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onNewFile",
        "",
        "onOpenFile",
        "onOpenProject",
        "onSave",
        "onSaveAs",
        "onSaveAll",
        "onCloseTab",
        "onCloseAll",
        "onRecentFileTriggered",
        "onUndo",
        "onRedo",
        "onGoToLine",
        "onShowPreferences",
        "onToggleFindPanel",
        "replaceMode",
        "onProjectSearch",
        "updateStatusBar",
        "filePath",
        "onActiveFileChanged",
        "onCursorPositionChanged",
        "label",
        "onBuildTriggered",
        "onRunTriggered",
        "onStopTriggered",
        "onBuildStarted",
        "onBuildOutput",
        "text",
        "isError",
        "onBuildFinished",
        "success",
        "exitCode",
        "onRunStarted",
        "onRunOutput",
        "onRunFinished",
        "onDebugStarted",
        "onDebugEnded",
        "onStepOver",
        "onStepInto",
        "onStepOut",
        "onExpressionEvaluated",
        "expr",
        "onNewProjectFromTemplate",
        "onToggleCommandPalette",
        "onActiveRunConfigChanged",
        "index",
        "handleGitFileSelected",
        "relPath",
        "handleProblemSelected",
        "path",
        "line",
        "col",
        "onBranchIndicatorClicked",
        "onLanguageIndicatorClicked",
        "handleActivityChanged",
        "handleToggleSidebar",
        "handleEditorChanged",
        "CodeEditor*",
        "editor",
        "handleEditorTextChanged",
        "handleLspDiagnostics",
        "uri",
        "QJsonArray",
        "diagnostics"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onNewFile'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenFile'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenProject'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSave'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveAs'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveAll'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCloseTab'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCloseAll'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRecentFileTriggered'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUndo'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRedo'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGoToLine'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onShowPreferences'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleFindPanel'
        QtMocHelpers::SlotData<void(bool)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 16 },
        }}),
        // Slot 'onProjectSearch'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateStatusBar'
        QtMocHelpers::SlotData<void(const QString &)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 19 },
        }}),
        // Slot 'onActiveFileChanged'
        QtMocHelpers::SlotData<void(const QString &)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 19 },
        }}),
        // Slot 'onCursorPositionChanged'
        QtMocHelpers::SlotData<void(const QString &)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 22 },
        }}),
        // Slot 'onBuildTriggered'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRunTriggered'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStopTriggered'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBuildStarted'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBuildOutput'
        QtMocHelpers::SlotData<void(const QString &, bool)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 28 }, { QMetaType::Bool, 29 },
        }}),
        // Slot 'onBuildFinished'
        QtMocHelpers::SlotData<void(bool, int)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 31 }, { QMetaType::Int, 32 },
        }}),
        // Slot 'onRunStarted'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRunOutput'
        QtMocHelpers::SlotData<void(const QString &, bool)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 28 }, { QMetaType::Bool, 29 },
        }}),
        // Slot 'onRunFinished'
        QtMocHelpers::SlotData<void(int)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 32 },
        }}),
        // Slot 'onDebugStarted'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDebugEnded'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStepOver'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStepInto'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStepOut'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExpressionEvaluated'
        QtMocHelpers::SlotData<void(const QString &)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 42 },
        }}),
        // Slot 'onNewProjectFromTemplate'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleCommandPalette'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onActiveRunConfigChanged'
        QtMocHelpers::SlotData<void(int)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 46 },
        }}),
        // Slot 'handleGitFileSelected'
        QtMocHelpers::SlotData<void(const QString &)>(47, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 48 },
        }}),
        // Slot 'handleProblemSelected'
        QtMocHelpers::SlotData<void(const QString &, int, int)>(49, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 50 }, { QMetaType::Int, 51 }, { QMetaType::Int, 52 },
        }}),
        // Slot 'onBranchIndicatorClicked'
        QtMocHelpers::SlotData<void()>(53, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLanguageIndicatorClicked'
        QtMocHelpers::SlotData<void()>(54, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleActivityChanged'
        QtMocHelpers::SlotData<void(int)>(55, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 46 },
        }}),
        // Slot 'handleToggleSidebar'
        QtMocHelpers::SlotData<void()>(56, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleEditorChanged'
        QtMocHelpers::SlotData<void(CodeEditor *)>(57, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 58, 59 },
        }}),
        // Slot 'handleEditorTextChanged'
        QtMocHelpers::SlotData<void()>(60, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleLspDiagnostics'
        QtMocHelpers::SlotData<void(const QString &, const QJsonArray &)>(61, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 62 }, { 0x80000000 | 63, 64 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onNewFile(); break;
        case 1: _t->onOpenFile(); break;
        case 2: _t->onOpenProject(); break;
        case 3: _t->onSave(); break;
        case 4: _t->onSaveAs(); break;
        case 5: _t->onSaveAll(); break;
        case 6: _t->onCloseTab(); break;
        case 7: _t->onCloseAll(); break;
        case 8: _t->onRecentFileTriggered(); break;
        case 9: _t->onUndo(); break;
        case 10: _t->onRedo(); break;
        case 11: _t->onGoToLine(); break;
        case 12: _t->onShowPreferences(); break;
        case 13: _t->onToggleFindPanel((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 14: _t->onProjectSearch(); break;
        case 15: _t->updateStatusBar((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->onActiveFileChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 17: _t->onCursorPositionChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->onBuildTriggered(); break;
        case 19: _t->onRunTriggered(); break;
        case 20: _t->onStopTriggered(); break;
        case 21: _t->onBuildStarted(); break;
        case 22: _t->onBuildOutput((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 23: _t->onBuildFinished((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 24: _t->onRunStarted(); break;
        case 25: _t->onRunOutput((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 26: _t->onRunFinished((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 27: _t->onDebugStarted(); break;
        case 28: _t->onDebugEnded(); break;
        case 29: _t->onStepOver(); break;
        case 30: _t->onStepInto(); break;
        case 31: _t->onStepOut(); break;
        case 32: _t->onExpressionEvaluated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 33: _t->onNewProjectFromTemplate(); break;
        case 34: _t->onToggleCommandPalette(); break;
        case 35: _t->onActiveRunConfigChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 36: _t->handleGitFileSelected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 37: _t->handleProblemSelected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 38: _t->onBranchIndicatorClicked(); break;
        case 39: _t->onLanguageIndicatorClicked(); break;
        case 40: _t->handleActivityChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 41: _t->handleToggleSidebar(); break;
        case 42: _t->handleEditorChanged((*reinterpret_cast<std::add_pointer_t<CodeEditor*>>(_a[1]))); break;
        case 43: _t->handleEditorTextChanged(); break;
        case 44: _t->handleLspDiagnostics((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 42:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< CodeEditor* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 45)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 45;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 45)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 45;
    }
    return _id;
}
QT_WARNING_POP

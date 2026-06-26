// src/debug/RunDebugView.h
#pragma once
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class RunDebugView : public QWidget {
    Q_OBJECT
public:
    explicit RunDebugView(QWidget* parent = nullptr);

    void setConfigurations(const QStringList& names, const QString& activeName);
    QString activeConfiguration() const;

signals:
    void activeConfigurationChanged(int index);
    void buildTriggered();
    void runTriggered();
    void stopTriggered();
    void editConfigurationRequested();
    void stepOverTriggered();
    void stepIntoTriggered();
    void stepOutTriggered();

private slots:
    void onComboChanged(int index);

private:
    QComboBox* m_configCombo;
    QPushButton* m_buildBtn;
    QPushButton* m_runBtn;
    QPushButton* m_stopBtn;
    QPushButton* m_editConfigBtn;
    QPushButton* m_stepOverBtn;
    QPushButton* m_stepIntoBtn;
    QPushButton* m_stepOutBtn;
};

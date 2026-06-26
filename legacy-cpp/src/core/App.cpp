// src/core/App.cpp
#include "App.h"
#include "services/ServiceLocator.h"
#include "settings/SettingsManager.h"
#include "commands/CommandRegistry.h"
#include "workspace/WorkspaceManager.h"

App::App(QObject* parent) : QObject(parent) {
}

App::~App() {
}

void App::initialize() {
    // Register global settings and command registry
    ServiceLocator::registerSettingsManager(&SettingsManager::instance());
    ServiceLocator::registerCommandRegistry(&CommandRegistry::instance());
}

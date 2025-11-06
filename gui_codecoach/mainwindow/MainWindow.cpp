//
// Created by andres on 4/11/25.
//

#include "MainWindow.h"

// --- Constructor ---
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupLayout();
    buildMenusAndActions();
    connectSignals();
    bindViewModels();
    loadInitialData();
}

// --- Layout ---
void MainWindow::setupLayout() {
    // TODO: crear splitter, tabs y widgets
}

// --- Menús y acciones ---
void MainWindow::buildMenusAndActions() {
    // TODO: crear menús, toolbars y acciones
}

// --- Conexiones ---
void MainWindow::connectSignals() {
    // TODO: conectar señales y slots
}

// --- Enlaces UI ↔ ViewModels ---
void MainWindow::bindViewModels() {
    // TODO: enlazar signals/slots entre VM y widgets
}

// --- Datos iniciales ---
void MainWindow::loadInitialData() {
    // TODO: cargar problemas iniciales (dummy)
}

// --- Cambiar de pestañas ---
void MainWindow::switchToResultsTab() {
    // TODO: rightTabs_->setCurrentIndex(TabResults);
}

void MainWindow::switchToCoachTab() {
    // TODO: rightTabs_->setCurrentIndex(TabCoach);
}

// Created by andres on 4/11/25.
//

#include "MainWindow.h"

#include <QApplication>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <QWidget>
#include <QString>

// --- Widgets ---
#include "../components/ProblemListWidget.h"
#include "../components/ProblemDetailWidget.h"
#include "../components/CodeEditorWidget.h"
#include "../components/RunResultsWidget.h"
#include "../components/CoachFeedbackWidget.h"

// --- ViewModels ---
#include "../viewmodels/ProblemViewModel.h"
#include "../viewmodels/EditorViewModel.h"
#include "../viewmodels/RunViewModel.h"
#include "../viewmodels/CoachViewModel.h"

// --- DTOs ---
#include "../viewmodels/dto/ProblemSummary.h"
#include "../viewmodels/dto/ProblemDetail.h"
#include "../viewmodels/dto/RunResults.h"
#include "../viewmodels/dto/CoachFeedback.h"

using cc::vm::ProblemViewModel;
using cc::vm::EditorViewModel;
using cc::vm::RunViewModel;
using cc::vm::CoachViewModel;

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
    auto* central = new QWidget(this);
    auto* v = new QVBoxLayout(central);
    v->setContentsMargins(0,0,0,0);

    // Splitter raíz: izquierda lista, derecha pestañas
    rootSplit_   = new QSplitter(Qt::Horizontal, central);
    problemList_ = new ProblemListWidget(rootSplit_);

    rightTabs_     = new QTabWidget(rootSplit_);
    problemDetail_ = new ProblemDetailWidget(rightTabs_);
    codeEditor_    = new CodeEditorWidget(rightTabs_);
    runResults_    = new RunResultsWidget(rightTabs_);
    coachFeedback_ = new CoachFeedbackWidget(rightTabs_);

    rightTabs_->addTab(problemDetail_, "Enunciado");
    rightTabs_->addTab(codeEditor_,    "Editor");
    rightTabs_->addTab(runResults_,    "Resultados");
    rightTabs_->addTab(coachFeedback_, "Coach");

    rootSplit_->setStretchFactor(0, 1);
    rootSplit_->setStretchFactor(1, 3);

    v->addWidget(rootSplit_, 1);
    setCentralWidget(central);

    resize(1280, 780);
    setWindowTitle("CodeCoach");
}

// --- Menús y acciones ---
void MainWindow::buildMenusAndActions() {
    // Menús
    menuFile_ = menuBar()->addMenu("&Archivo");
    menuRun_  = menuBar()->addMenu("&Ejecutar");
    menuView_ = menuBar()->addMenu("&Vista");
    menuHelp_ = menuBar()->addMenu("&Ayuda");

    // Archivo
    actNew_  = new QAction("Nuevo", this);
    actOpen_ = new QAction("Abrir…", this);
    actSave_ = new QAction("Guardar", this);
    actExit_ = new QAction("Salir", this);
    menuFile_->addActions({actNew_, actOpen_, actSave_});
    menuFile_->addSeparator();
    menuFile_->addAction(actExit_);

    // Ejecutar
    actRun_    = new QAction("Run", this);
    actCancel_ = new QAction("Cancelar", this);
    actRun_->setShortcut(Qt::CTRL | Qt::Key_Return);   // Ctrl+Enter
    actCancel_->setShortcut(Qt::Key_Escape);
    menuRun_->addActions({actRun_, actCancel_});

    // Vista
    actToggleLeftPane_ = new QAction("Mostrar/ocultar lista", this);
    actToggleLeftPane_->setCheckable(true);
    actToggleLeftPane_->setChecked(true);
    actToggleTheme_ = new QAction("Tema claro/oscuro", this);
    menuView_->addActions({actToggleLeftPane_, actToggleTheme_});

    // Ayuda
    actAbout_ = new QAction("Acerca de", this);
    menuHelp_->addAction(actAbout_);

    // Toolbar principal
    mainToolbar_ = addToolBar("Main");
    mainToolbar_->addAction(actRun_);
    mainToolbar_->addAction(actCancel_);
    mainToolbar_->addSeparator();
    mainToolbar_->addAction(actSave_);
}

// --- Conexiones (UI ↔ UI básicas) ---
void MainWindow::connectSignals() {
    // Mostrar/ocultar panel izquierdo
    connect(actToggleLeftPane_, &QAction::toggled, this, [this](bool on){
        if (!rootSplit_) return;
        if (auto* left = rootSplit_->widget(0)) left->setVisible(on);
    });

    // Tema (stub)
    connect(actToggleTheme_, &QAction::triggered, this, []{
        // TODO: aplicar palette claro/oscuro o estilo fusion.
    });

    // Salir
    connect(actExit_, &QAction::triggered, qApp, &QApplication::quit);

    // Ejecutar desde editor (Ctrl+Enter interno)
    connect(codeEditor_, &CodeEditorWidget::runRequested, actRun_, &QAction::trigger);
}

// --- Enlaces UI ↔ ViewModels ---
void MainWindow::bindViewModels() {
    // Crear VMs (ownership de la ventana)
    problemVM_ = new ProblemViewModel(this);
    editorVM_  = new EditorViewModel(this);
    runVM_     = new RunViewModel(this);
    coachVM_   = new CoachViewModel(this);

    // 1) ProblemVM → poblar lista
    connect(problemVM_, &ProblemViewModel::problemsReady,
            problemList_, &ProblemListWidget::setProblems);

    // 2) Selección en lista → pedir detalle al VM
    connect(problemList_, &ProblemListWidget::problemChosen,
            problemVM_, &ProblemViewModel::setCurrentById);

    // 3) Detalle listo → render en Enunciado y crear starter en Editor
    connect(problemVM_, &ProblemViewModel::detailReady,
            this, [this](const cc::dto::ProblemDetail& d){
                // guardar id actual para RUN
                currentProblemId_ = d.id;
                // pintar enunciado
                problemDetail_->render(d);
                // pedir código starter al EditorVM
                editorVM_->loadStarterFor(d.id);
            });

    // 4) EditorVM → inyectar código starter al editor
    connect(editorVM_, &EditorViewModel::codeReady,
            codeEditor_, &CodeEditorWidget::loadStarterCode);

    // 5) Acción Run → RunVM + CoachVM
    connect(actRun_, &QAction::triggered, this, [this]{
        switchToResultsTab();
        if (runResults_) runResults_->showRunning();

        const QString pid  = currentProblemId_.isEmpty() ? QStringLiteral("two-sum")
                                                         : currentProblemId_;
        const QString code = codeEditor_ ? codeEditor_->currentCode() : QString();

        runVM_->run(pid, code);
        coachVM_->analyze(pid, code);
    });

    // 6) RunVM → Resultados
    connect(runVM_, &RunViewModel::resultsReady,
            runResults_, &RunResultsWidget::render);
    connect(runVM_, &RunViewModel::stdOut,
            runResults_, &RunResultsWidget::appendStdOut);
    connect(runVM_, &RunViewModel::stdErr,
            runResults_, &RunResultsWidget::appendStdErr);

    // 7) CoachVM → CoachFeedback
    connect(coachVM_, &CoachViewModel::feedbackReady,
            coachFeedback_, &CoachFeedbackWidget::render);

    // 8) Cancelar (stub visible en resultados)
    connect(actCancel_, &QAction::triggered, this, [this]{
        if (runResults_) runResults_->appendStdOut("Ejecución cancelada por el usuario.");
    });
}

// --- Datos iniciales ---
void MainWindow::loadInitialData() {
    // Pedir al VM que cargue lista inicial (mock o real)
    problemVM_->loadMock();
}

// --- Cambiar de pestañas ---
void MainWindow::switchToResultsTab() {
    if (rightTabs_) rightTabs_->setCurrentIndex(TabResults);
}

void MainWindow::switchToCoachTab() {
    if (rightTabs_) rightTabs_->setCurrentIndex(TabCoach);
}

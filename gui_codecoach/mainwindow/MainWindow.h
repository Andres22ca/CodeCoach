//
// Created by andres on 4/11/25.
//

#ifndef CODECOACH_MAINWINDOW_H
#define CODECOACH_MAINWINDOW_H

#include <QMainWindow>

class QTabWidget;
class QSplitter;
class QAction;
class QMenu;
class QToolBar;

class ProblemListWidget;
class ProblemDetailWidget;
class CodeEditorWidget;
class RunResultsWidget;
class CoachFeedbackWidget;

namespace cc::vm {
class ProblemViewModel;
class EditorViewModel;
class RunViewModel;
class CoachViewModel;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    // --- Ciclo de construcción (sin implementación aún) ---
    void setupLayout();           // crea splitters, tabs, widgets
    void buildMenusAndActions();  // crea menús, acciones y toolbars
    void connectSignals();        // conecta señales/slots entre widgets
    void bindViewModels();        // suscribe UI <-> ViewModels
    void loadInitialData();       // carga datos iniciales (mock más adelante)

    // --- Widgets raíz ---
    QSplitter* rootSplit_ = nullptr;   // Izq: problemas  | Der: tabs
    QTabWidget* rightTabs_ = nullptr;  // Editor | Resultados | Coach

    // --- Widgets de contenido ---
    ProblemListWidget*    problemList_    = nullptr;
    ProblemDetailWidget*  problemDetail_  = nullptr; // va arriba del editor o en splitter vertical
    CodeEditorWidget*     codeEditor_     = nullptr;
    RunResultsWidget*     runResults_     = nullptr;
    CoachFeedbackWidget*  coachFeedback_  = nullptr;

    // --- ViewModels ---
    cc::vm::ProblemViewModel* problemVM_ = nullptr;
    cc::vm::EditorViewModel*  editorVM_  = nullptr;
    cc::vm::RunViewModel*     runVM_     = nullptr;
    cc::vm::CoachViewModel*   coachVM_   = nullptr;

    // --- Menús, acciones y barras ---
    QMenu* menuFile_ = nullptr;
    QMenu* menuRun_  = nullptr;
    QMenu* menuView_ = nullptr;
    QMenu* menuHelp_ = nullptr;

    QToolBar* mainToolbar_ = nullptr;

    QAction* actNew_      = nullptr;
    QAction* actOpen_     = nullptr;
    QAction* actSave_     = nullptr;
    QAction* actExit_     = nullptr;

    QAction* actRun_      = nullptr;   // Ctrl+Enter
    QAction* actCancel_   = nullptr;

    QAction* actToggleLeftPane_ = nullptr; // mostrar/ocultar lista de problemas
    QAction* actToggleTheme_    = nullptr; // claro/oscuro (stub)

    QAction* actAbout_    = nullptr;

    // --- Conveniencias ---
    enum TabIndex { TabEditor = 0, TabResults = 1, TabCoach = 2 };
    void switchToResultsTab();  // helper privado (lo implementarás luego)
    void switchToCoachTab();
};

#endif // CODECOACH_MAINWINDOW_H
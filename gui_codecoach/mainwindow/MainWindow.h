#pragma once
#include <QMainWindow>
#include <QString>

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
    // ciclo
    void setupLayout();
    void buildMenusAndActions();
    void connectSignals();
    void bindViewModels();
    void loadInitialData();
    void switchToResultsTab();
    void switchToCoachTab();

    enum TabIndex { TabDetail = 0, TabEditor = 1, TabResults = 2, TabCoach = 3 };

    // layout
    QSplitter*  rootSplit_  = nullptr;
    QTabWidget* rightTabs_  = nullptr;

    // widgets
    ProblemListWidget*    problemList_    = nullptr;
    ProblemDetailWidget*  problemDetail_  = nullptr;
    CodeEditorWidget*     codeEditor_     = nullptr;
    RunResultsWidget*     runResults_     = nullptr;
    CoachFeedbackWidget*  coachFeedback_  = nullptr;

    // viewmodels
    cc::vm::ProblemViewModel* problemVM_ = nullptr;
    cc::vm::EditorViewModel*  editorVM_  = nullptr;
    cc::vm::RunViewModel*     runVM_     = nullptr;
    cc::vm::CoachViewModel*   coachVM_   = nullptr;

    // menús / acciones
    QMenu*    menuFile_ = nullptr;
    QMenu*    menuRun_  = nullptr;
    QMenu*    menuView_ = nullptr;
    QMenu*    menuHelp_ = nullptr;

    QToolBar* mainToolbar_ = nullptr;

    QAction* actNew_    = nullptr;
    QAction* actOpen_   = nullptr;
    QAction* actSave_   = nullptr;
    QAction* actExit_   = nullptr;

    QAction* actRun_    = nullptr;
    QAction* actCancel_ = nullptr;

    QAction* actToggleLeftPane_ = nullptr;
    QAction* actToggleTheme_    = nullptr;
    QAction* actAbout_          = nullptr;

    // estado
    QString currentProblemId_;
};

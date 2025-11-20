#ifndef CODECOACH_EDITORVIEWMODEL_H
#define CODECOACH_EDITORVIEWMODEL_H

#include <QObject>
#include <QString>
#include "dto/ProblemDetail.h"

namespace cc::vm {

    class EditorViewModel : public QObject {
        Q_OBJECT
    public:
        explicit EditorViewModel(QObject* parent = nullptr);

        // ----------------------
        // ★ API PRINCIPAL
        // ----------------------

        // ***** NECESARIO PARA MainWindow.cpp *****
        void loadStarterFor(const QString& id);
        // ******************************************

        // ***** NECESARIO PARA MainWindow.cpp *****
        // Cuando generamos el starter enviamos código a CodeEditorWidget
        signals:
            void codeReady(const QString& code);
        // ******************************************

        // Otros métodos existentes
        void setProblem(const cc::dto::ProblemDetail& problem);
        void setCode(const QString& code);
        QString code() const;

        void setLanguage(const QString& lang);
        QString language() const;

        void reset();
        void format();
        void insertSnippet(const QString& snippet);

        bool isDirty() const;
        void markSaved();

        signals:
            void problemChanged(const cc::dto::ProblemDetail& problem);
        void codeChanged(const QString& code);
        void languageChanged(const QString& lang);
        void dirtyChanged(bool dirty);

    private:
        cc::dto::ProblemDetail currentProblem_;
        QString code_;
        QString language_ = QStringLiteral("C++17");
        bool dirty_ = false;
        bool updating_ = false;
    };

} // namespace cc::vm

#endif // CODECOACH_EDITORVIEWMODEL_H

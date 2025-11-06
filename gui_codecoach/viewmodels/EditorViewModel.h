//
// Created by andres on 6/11/25.
//

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

        //  API principal
        void setProblem(const cc::dto::ProblemDetail& problem); // Cambia el problema activo
        void setCode(const QString& code);                       // Establece el código actual
        QString code() const;                                    // Devuelve el código actual

        void setLanguage(const QString& lang);                   // para progra
        QString language() const;

        void reset();                                            // Limpia/recarga código inicial del problema
        void format();                                           // Formateo (stub por ahora)
        void insertSnippet(const QString& snippet);              // Inserta texto/snippet

        // --- Estado de guardado ---
        bool isDirty() const;            // Hay cambios sin guardar?
        void markSaved();                // Marca como guardado (dirty=false)

        signals:
            // Notificaciones para la UI / otros VMs
            void problemChanged(const cc::dto::ProblemDetail& problem);
        void codeChanged(const QString& code);
        void languageChanged(const QString& lang);
        void dirtyChanged(bool dirty);

    private:
        // --- Estado interno mínimo (sin lógica en el header) ---
        cc::dto::ProblemDetail currentProblem_;
        QString code_;
        QString language_ = QStringLiteral("C++17");
        bool dirty_ = false;

        // Bandera para evitar bucles al sincronizar con el widget (UI <-> VM)
        bool updating_ = false;
    };

} // namespace cc::vm

#endif // CODECOACH_EDITORVIEWMODEL_H

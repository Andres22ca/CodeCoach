//
// Created by andres on 5/11/25.
//

#ifndef CODECOACH_PROBLEMVIEWMODEL_H
#define CODECOACH_PROBLEMVIEWMODEL_H

#include <QObject>
#include <QList>
#include "dto/ProblemSummary.h"
#include "dto/ProblemDetail.h"

namespace cc::vm {

    class ProblemViewModel : public QObject {
        Q_OBJECT
    public:
        explicit ProblemViewModel(QObject* parent = nullptr);

        //  API pública
        void loadAll();                     // Carga la lista de problemas (dummy o real)
        void select(const QString& id);     // Selecciona un problema por ID
        void clearSelection();              // Limpia el problema actual

        //  Accesores
        QList<cc::dto::ProblemSummary> problems() const;
        cc::dto::ProblemDetail currentProblem() const;

        signals:
            //  Señales para la GUI
            void problemsChanged(const QList<cc::dto::ProblemSummary>& problems);
        void detailChanged(const cc::dto::ProblemDetail& detail);
        void selectionCleared();

    private:
        //  Estado interno
        QList<cc::dto::ProblemSummary> problems_; // Lista de problemas cargados
        cc::dto::ProblemDetail current_;          // Problema actual seleccionado
    };

} // namespace cc::vm

#endif // CODECOACH_PROBLEMVIEWMODEL_H

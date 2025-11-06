//
// Created by andres on 5/11/25.
//

#ifndef CODECOACH_RUNRESULTS_H
#define CODECOACH_RUNRESULTS_H

#include <QString>
#include <QList>

namespace cc::dto {

    struct RunResults {
        enum class Status {
            Pending,   // Aún no ejecutado
            Running,
            Passed,
            Failed,
            Error
        };

        Status status = Status::Pending;

        int testsPassed = 0;
        int testsTotal = 0;

        double timeMs = 0.0;   // Tiempo total de ejecución
        size_t memoryBytes = 0; // Memoria usada

        QString stdOut;   // Salida estándar
        QString stdErr;   // Errores o logs
        QList<int> failedCases; // Índices de casos fallidos
    };

} // namespace cc::dto

#endif // CODECOACH_RUNRESULTS_H

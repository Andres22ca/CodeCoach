

#ifndef CODECOACH_PROBLEMSUMMARY_H
#define CODECOACH_PROBLEMSUMMARY_H

#include <QString>
#include <QStringList>

namespace cc::dto {

    struct ProblemSummary {
        QString id;          // Identificador único del problema
        QString title;       // Nombre o título
        QString difficulty;  // "Easy", "Medium", "Hard"
        QStringList tags;    // Etiquetas de categorías (math, dp, strings...)
    };

} // namespace cc::dto

#endif // CODECOACH_PROBLEMSUMMARY_H

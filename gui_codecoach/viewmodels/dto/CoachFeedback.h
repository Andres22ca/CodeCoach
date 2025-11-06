//
// Created by andres on 5/11/25.
//

#ifndef CODECOACH_COACHFEEDBACK_H
#define CODECOACH_COACHFEEDBACK_H

#include <QString>
#include <QStringList>

namespace cc::dto {

    struct CoachFeedback {
        QString headline;        // Resumen corto del feedback
        QStringList hints;       // Lista de sugerencias concretas
        QString reasoning;       // Explicación más detallada
    };

} // namespace cc::dto

#endif // CODECOACH_COACHFEEDBACK_H

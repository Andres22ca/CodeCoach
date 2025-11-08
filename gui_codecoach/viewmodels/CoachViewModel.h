//
// Created by andres on 6/10/25.
//

#ifndef GUI_CODECOACH_COACHVIEWMODEL_H
#define GUI_CODECOACH_COACHVIEWMODEL_H



#pragma once
#include <QObject>
#include "dto/CoachFeedback.h"

namespace cc::vm {
    class CoachViewModel : public QObject {
        Q_OBJECT
    public:
        explicit CoachViewModel(QObject* parent=nullptr);
    public slots:
        void analyze(const QString& problemId, const QString& code);
        signals:
            void feedbackReady(cc::dto::CoachFeedback fb);
    };
}


//Envía código y resultados al solution_analyzer, muestra hints y complejidad.
#endif //GUI_CODECOACH_COACHVIEWMODEL_H
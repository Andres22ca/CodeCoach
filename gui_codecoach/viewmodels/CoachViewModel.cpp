
#include "CoachViewModel.h"
#include <QTimer>
using namespace cc::vm;
using cc::dto::CoachFeedback;

CoachViewModel::CoachViewModel(QObject* parent): QObject(parent) {}

void CoachViewModel::analyze(const QString& /*problemId*/, const QString& /*code*/) {
    QTimer::singleShot(350, this, [this]{
        CoachFeedback fb;
        fb.headline = "Podrías optimizar el uso del mapa.";
        fb.hints = {"Reserva capacidad con reserve()", "Evita recrear el mapa en el bucle"};
        fb.reasoning = "Las re-asignaciones frecuentes y el rehashing elevan el costo amortizado.";
        emit feedbackReady(fb);
    });
}

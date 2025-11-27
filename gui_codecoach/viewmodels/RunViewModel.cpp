#include "RunViewModel.h"
#include <QTimer>

using namespace cc::vm;

RunViewModel::RunViewModel(QObject* parent)
    : QObject(parent)
{
}

void RunViewModel::run(const QString& problemId, const QString& code)
{
    Q_UNUSED(problemId);
    Q_UNUSED(code);

    // Estado inicial
    clear();
    setRunning(true);
    emit stdOut("Iniciando ejecución...\n");

    // Simulamos progreso en 3 pasos
    setProgress(25);
    emit stdOut("Compilando...\n");

    QTimer::singleShot(200, this, [this]{
        if (!running_)
            return;

        setProgress(70);
        emit stdOut("Ejecutando casos de prueba...\n");

        QTimer::singleShot(300, this, [this]{
            if (!running_)
                return;

            setProgress(100);
            setRunning(false);

            // lastResults_ ya es un cc::dto::RunResults default
            emit stdOut("Ejecución finalizada.\n");
            emit resultsReady(lastResults_);
        });
    });
}

void RunViewModel::cancel()
{
    if (!running_)
        return;

    setRunning(false);
    emit stdOut("Ejecución cancelada por el usuario.\n");
    emit errorOccurred("La ejecución fue cancelada.");
}

void RunViewModel::clear()
{
    running_ = false;
    progress_ = 0;
    lastResults_ = cc::dto::RunResults{};
    emit runningChanged(running_);
    emit progressChanged(progress_);
}

cc::dto::RunResults RunViewModel::results() const
{
    return lastResults_;
}

bool RunViewModel::isRunning() const
{
    return running_;
}

int RunViewModel::progress() const
{
    return progress_;
}

void RunViewModel::setRunning(bool running)
{
    if (running_ == running)
        return;
    running_ = running;
    emit runningChanged(running_);
}

void RunViewModel::setProgress(int p)
{
    if (progress_ == p)
        return;
    progress_ = p;
    emit progressChanged(progress_);
}

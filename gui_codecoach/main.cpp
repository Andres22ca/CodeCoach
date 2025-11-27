#include <QApplication>
#include <QList>
#include <QMetaType>

#include "mainwindow/MainWindow.h"
#include "viewmodels/dto/ProblemSummary.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Registrar tipos para el sistema de metatipos de Qt
    qRegisterMetaType<cc::dto::ProblemSummary>("cc::dto::ProblemSummary");
    qRegisterMetaType<QList<cc::dto::ProblemSummary>>("QList<cc::dto::ProblemSummary>");

    MainWindow w;
    w.show();

    return app.exec();
}


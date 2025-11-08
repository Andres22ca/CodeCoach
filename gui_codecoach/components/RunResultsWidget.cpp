#include "RunResultsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
using cc::dto::RunResults;

static QString statusToStr(RunResults::Status s){
    switch(s){
        case RunResults::Status::Pending: return "Pending";
        case RunResults::Status::Running: return "Running";
        case RunResults::Status::Passed:  return "Passed";
        case RunResults::Status::Failed:  return "Failed";
        case RunResults::Status::Error:   return "Error";
    }
    return "?";
}

RunResultsWidget::RunResultsWidget(QWidget* parent) : QWidget(parent) {
    auto* v = new QVBoxLayout(this);
    statusLbl_ = new QLabel("Status: Pending", this);

    auto* h = new QHBoxLayout();
    out_ = new QPlainTextEdit(this); out_->setReadOnly(true); out_->setPlaceholderText("stdout…");
    err_ = new QPlainTextEdit(this); err_->setReadOnly(true); err_->setPlaceholderText("stderr…");

    h->addWidget(out_, 1);
    h->addWidget(err_, 1);

    v->addWidget(statusLbl_);
    v->addLayout(h);
}

void RunResultsWidget::showRunning() {
    statusLbl_->setText("Status: Running");
    out_->clear(); err_->clear();
}

void RunResultsWidget::render(const RunResults& r) {
    statusLbl_->setText(QString("Status: %1  |  %2/%3 tests  |  %4 ms  |  %5 bytes")
        .arg(statusToStr(r.status))
        .arg(r.testsPassed).arg(r.testsTotal)
        .arg(r.timeMs).arg(r.memoryBytes));

    out_->setPlainText(r.stdOut);
    err_->setPlainText(r.stdErr);
}

void RunResultsWidget::appendStdOut(const QString& line){ out_->appendPlainText(line); }
void RunResultsWidget::appendStdErr(const QString& line){ err_->appendPlainText(line); }


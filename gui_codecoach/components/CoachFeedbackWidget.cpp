#include "CoachFeedbackWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
using cc::dto::CoachFeedback;

CoachFeedbackWidget::CoachFeedbackWidget(QWidget* parent) : QWidget(parent) {
    auto* v = new QVBoxLayout(this);
    headline_ = new QLabel("<b>Coach:</b> (sin datos)", this);
    hints_ = new QListWidget(this);
    reasoning_ = new QPlainTextEdit(this); reasoning_->setReadOnly(true);

    v->addWidget(headline_);
    v->addWidget(hints_, 2);
    v->addWidget(reasoning_, 1);
}

void CoachFeedbackWidget::render(const CoachFeedback& fb) {
    headline_->setText("<b>Coach:</b> " + fb.headline);
    hints_->clear();
    for (const auto& h : fb.hints) hints_->addItem("• " + h);
    reasoning_->setPlainText(fb.reasoning);
}

void CoachFeedbackWidget::addHint(const QString& h) { hints_->addItem("• " + h); }


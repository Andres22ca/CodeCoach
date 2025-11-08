//
// Created by andres on 6/10/25.
//

#ifndef GUI_CODECOACH_CODEEDITORWIDGET_H
#define GUI_CODECOACH_CODEEDITORWIDGET_H


#pragma once
#include <QPlainTextEdit>

class CodeEditorWidget : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit CodeEditorWidget(QWidget* parent=nullptr);
    QString currentCode() const;

    signals:
        void runRequested();

public slots:
    void loadStarterCode(QString problemId);

protected:
    void keyPressEvent(QKeyEvent* e) override; // Ctrl+Enter => runRequested
};



#endif //GUI_CODECOACH_CODEEDITORWIDGET_H // editor de texto
#include "CodeEditorWidget.h"
#include <QKeyEvent>
#include <QTextOption>

CodeEditorWidget::CodeEditorWidget(QWidget* parent) : QPlainTextEdit(parent) {
    setWordWrapMode(QTextOption::NoWrap);
    setPlaceholderText("// Escribe tu solución en C++ aquí… (Ctrl+Enter para ejecutar)");
}

QString CodeEditorWidget::currentCode() const {
    return toPlainText();
}

void CodeEditorWidget::loadStarterCode(QString problemId) {
    setPlainText(QString(
        "// %1 — plantilla mínima\n"
        "#include <bits/stdc++.h>\n"
        "using namespace std;\n"
        "int main(){\n"
        "  ios::sync_with_stdio(false); cin.tie(nullptr);\n"
        "  // TODO: implement\n"
        "  return 0;\n"
        "}\n").arg(problemId));
}

void CodeEditorWidget::keyPressEvent(QKeyEvent* e) {
    const bool ctrlEnter = (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
                           && (e->modifiers() & Qt::ControlModifier);
    if (ctrlEnter) { emit runRequested(); return; }
    QPlainTextEdit::keyPressEvent(e);
}

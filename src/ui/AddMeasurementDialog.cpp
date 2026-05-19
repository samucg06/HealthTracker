
/**
 * @file AddMeasurementDialog.cpp
 * @brief Implementación del diálogo para agregar mediciones.
 */

#include "AddMeasurementDialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QGroupBox>

AddMeasurementDialog::AddMeasurementDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Agregar Medición");
    setFixedWidth(360);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setupUi();
}

void AddMeasurementDialog::setupUi() {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(12);
    root->setContentsMargins(15, 15, 15, 12);

    auto* form = new QFormLayout();
    form->setSpacing(8);

    // ── Tipo ──────────────────────────────────
    m_typeCombo = new QComboBox();
    m_typeCombo->addItems({"⚖️  Peso", "❤️  Presión Arterial", "🩸 Glucosa"});
    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddMeasurementDialog::onTypeChanged);
    form->addRow("Tipo de medición:", m_typeCombo);
    root->addLayout(form);

    // ── Stack de inputs ───────────────────────
    m_stack = new QStackedWidget();

    // Página 0 — Peso
    {
        auto* page = new QWidget();
        auto* fl   = new QFormLayout(page);
        fl->setContentsMargins(0,0,0,0);
        m_weightSpin = new QDoubleSpinBox();
        m_weightSpin->setRange(20.0, 300.0);
        m_weightSpin->setValue(70.0);
        m_weightSpin->setDecimals(1);
        m_weightSpin->setSuffix(" kg");
        fl->addRow("Peso:", m_weightSpin);
        m_stack->addWidget(page);
    }

    // Página 1 — Presión arterial
    {
        auto* page = new QWidget();
        auto* fl   = new QFormLayout(page);
        fl->setContentsMargins(0,0,0,0);
        m_bpSys = new QDoubleSpinBox();
        m_bpSys->setRange(60.0, 250.0);
        m_bpSys->setValue(120.0);
        m_bpSys->setDecimals(0);
        m_bpSys->setSuffix(" mmHg");
        m_bpDia = new QDoubleSpinBox();
        m_bpDia->setRange(40.0, 150.0);
        m_bpDia->setValue(80.0);
        m_bpDia->setDecimals(0);
        m_bpDia->setSuffix(" mmHg");
        fl->addRow("Sistólica:",  m_bpSys);
        fl->addRow("Diastólica:", m_bpDia);
        m_stack->addWidget(page);
    }

    // Página 2 — Glucosa
    {
        auto* page = new QWidget();
        auto* fl   = new QFormLayout(page);
        fl->setContentsMargins(0,0,0,0);
        m_glucSpin = new QDoubleSpinBox();
        m_glucSpin->setRange(20.0, 600.0);
        m_glucSpin->setValue(100.0);
        m_glucSpin->setDecimals(1);
        m_glucSpin->setSuffix(" mg/dL");
        fl->addRow("Glucosa en sangre:", m_glucSpin);
        m_stack->addWidget(page);
    }

    root->addWidget(m_stack);

    // ── Fecha/hora y notas ────────────────────
    auto* bottomForm = new QFormLayout();
    bottomForm->setSpacing(8);

    m_dateTime = new QDateTimeEdit(QDateTime::currentDateTime());
    m_dateTime->setDisplayFormat("yyyy-MM-dd  HH:mm");
    m_dateTime->setCalendarPopup(true);
    bottomForm->addRow("Fecha / Hora:", m_dateTime);

    m_notes = new QLineEdit();
    m_notes->setPlaceholderText("Observaciones opcionales...");
    bottomForm->addRow("Notas:", m_notes);

    root->addLayout(bottomForm);

    // ── Botones ───────────────────────────────
    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Guardar");
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);
}

void AddMeasurementDialog::onTypeChanged(int index) {
    m_stack->setCurrentIndex(index);
}

// ─────────────────────────────────────────────
//  Getters
// ─────────────────────────────────────────────

QString AddMeasurementDialog::getType() const {
    const QStringList types = {"weight", "blood_pressure", "glucose"};
    return types[m_typeCombo->currentIndex()];
}

double AddMeasurementDialog::getValue1() const {
    int idx = m_typeCombo->currentIndex();
    if (idx == 0) return m_weightSpin->value();
    if (idx == 1) return m_bpSys->value();
    return m_glucSpin->value();
}

double AddMeasurementDialog::getValue2() const {
    return (m_typeCombo->currentIndex() == 1) ? m_bpDia->value() : 0.0;
}

QString AddMeasurementDialog::getRecordedAt() const {
    return m_dateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss");
}

QString AddMeasurementDialog::getNotes() const {
    return m_notes->text().trimmed();
}

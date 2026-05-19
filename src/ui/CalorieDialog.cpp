/**
 * @file CalorieDialog.cpp
 * @brief Implementación de la calculadora de calorías diarias.
 */

#include "CalorieDialog.h"
#include "utils/StatsCalculator.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDate>
#include <QFrame>

CalorieDialog::CalorieDialog(const UserRecord& user, QWidget* parent)
    : QDialog(parent), m_user(user) {
    setWindowTitle("Calculadora de Calorías Diarias");
    setFixedWidth(420);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setupUi();
}

void CalorieDialog::setupUi() {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(12);
    root->setContentsMargins(18, 16, 18, 14);

    root->addWidget(new QLabel(
        "<h3 style='margin:0'>🍎 Calculadora de Kilocalorías</h3>"
        "<p style='color:gray; font-size:9pt; margin:0'>"
        "Fórmula: Mifflin-St Jeor</p>"));

    auto* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    root->addWidget(sep);

    auto* form = new QFormLayout();
    form->setSpacing(10);

    // Peso actual (puede haber cambiado desde el registro)
    m_weightSpin = new QDoubleSpinBox();
    m_weightSpin->setRange(30.0, 300.0);
    m_weightSpin->setValue(70.0);
    m_weightSpin->setDecimals(1);
    m_weightSpin->setSuffix(" kg");
    form->addRow("Peso actual:", m_weightSpin);

    m_activityCombo = new QComboBox();
    m_activityCombo->addItems({
        "Sedentario  (sin ejercicio)",
        "Ligero      (1–3 días/sem)",
        "Moderado    (3–5 días/sem)",
        "Activo      (6–7 días/sem)",
        "Muy activo  (2x al día)"
    });
    m_activityCombo->setCurrentIndex(1);
    form->addRow("Nivel de actividad:", m_activityCombo);

    m_goalCombo = new QComboBox();
    m_goalCombo->addItems({
        "Déficit (bajar peso)   −500 kcal",
        "Mantenimiento",
        "Superávit (ganar masa) +300 kcal"
    });
    m_goalCombo->setCurrentIndex(1);
    form->addRow("Objetivo:", m_goalCombo);

    root->addLayout(form);

    auto* calcBtn = new QPushButton("🔢  Calcular");
    calcBtn->setMinimumHeight(34);
    connect(calcBtn, &QPushButton::clicked, this, &CalorieDialog::onCalculate);
    root->addWidget(calcBtn);

    // Resultado
    m_resultLabel = new QLabel("Ajusta los parámetros y presiona Calcular.");
    m_resultLabel->setWordWrap(true);
    m_resultLabel->setAlignment(Qt::AlignCenter);
    m_resultLabel->setMinimumHeight(90);
    m_resultLabel->setStyleSheet(
        "background:#f0faf0; border:1px solid #b0d8b0; "
        "border-radius:6px; padding:10px; font-size:10pt;");
    root->addWidget(m_resultLabel);

    auto* closeBtn = new QPushButton("Cerrar");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    root->addWidget(closeBtn);
}

void CalorieDialog::onCalculate() {
    int age = QDate::currentDate().year() - m_user.birthYear;
    if (age < 5 || age > 120) age = 25;  // fallback si el año no se ingresó

    // Mapear texto del combo a clave interna
    const QStringList actKeys = {
        "Sedentario", "Ligero", "Moderado", "Activo", "Muy activo"
    };
    const QStringList goalKeys = {
        "Déficit (bajar peso)", "Mantenimiento", "Superávit (ganar masa)"
    };

    QString activity = actKeys[m_activityCombo->currentIndex()];
    QString goal     = goalKeys[m_goalCombo->currentIndex()];

    double tdee   = StatsCalculator::calculateTDEE(
        m_weightSpin->value(), m_user.heightCm, age, m_user.gender, activity);
    double target = StatsCalculator::adjustForGoal(tdee, goal);

    // IMC con el peso ingresado
    double bmi  = StatsCalculator::calculateBMI(
        m_weightSpin->value(), m_user.heightCm / 100.0);
    QString cat = StatsCalculator::bmiCategory(bmi);

    m_resultLabel->setText(
        QString(
            "<b>TDEE (gasto total):</b> %.0f kcal/día<br>"
            "<b>Meta (%1):</b> "
            "<span style='font-size:15pt; color:#27ae60;'><b>%.0f kcal/día</b></span><br><br>"
            "<span style='color:gray;'>IMC: %.1f — %2 | Edad: %3 años</span>"
        )
        .arg(goal)
        .arg(tdee)
        .arg(target)
        .arg(bmi)
        .arg(cat)
        .arg(age)
    );
}

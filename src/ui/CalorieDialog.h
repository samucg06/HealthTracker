#ifndef CALORIEDLOG_H
#define CALORIEDLOG_H

/**
 * @file CalorieDialog.h
 * @brief Calculadora de kilocalorías diarias con ecuación Mifflin-St Jeor.
 */

#include <QDialog>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include "database/DatabaseManager.h"

/**
 * @class CalorieDialog
 * @brief Ventana que calcula el TDEE y el objetivo calórico del usuario.
 *
 * El diálogo toma como entrada el nivel de actividad y el objetivo
 * (déficit / mantenimiento / superávit) y muestra las kcal diarias
 * usando la ecuación de Mifflin-St Jeor con los datos del perfil.
 */
class CalorieDialog : public QDialog {
    Q_OBJECT

public:
    explicit CalorieDialog(const UserRecord& user, QWidget* parent = nullptr);

private slots:
    void onCalculate();

private:
    void setupUi();

    UserRecord      m_user;
    QDoubleSpinBox* m_weightSpin;
    QComboBox*      m_activityCombo;
    QComboBox*      m_goalCombo;
    QLabel*         m_resultLabel;
};

#endif // CALORIEDLOG_H

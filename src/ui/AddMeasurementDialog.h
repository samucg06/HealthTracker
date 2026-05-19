#ifndef ADDMEASUREMENTDIALOG_H
#define ADDMEASUREMENTDIALOG_H

/**
 * @file AddMeasurementDialog.h
 * @brief Diálogo para registrar una nueva medición biométrica.
 */

#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QStackedWidget>

/**
 * @class AddMeasurementDialog
 * @brief Permite al usuario seleccionar el tipo de medición e ingresar los valores.
 *
 * Usa un QStackedWidget para mostrar campos distintos según el tipo:
 * - Peso: un spinbox (kg)
 * - Presión arterial: dos spinboxes (sistólica / diastólica)
 * - Glucosa: un spinbox (mg/dL)
 */
class AddMeasurementDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddMeasurementDialog(QWidget* parent = nullptr);

    QString getType()       const;
    double  getValue1()     const;
    double  getValue2()     const;
    QString getRecordedAt() const;
    QString getNotes()      const;

private slots:
    void onTypeChanged(int index);

private:
    void setupUi();

    QComboBox*      m_typeCombo;
    QStackedWidget* m_stack;

    // Peso
    QDoubleSpinBox* m_weightSpin;

    // Presión arterial
    QDoubleSpinBox* m_bpSys;
    QDoubleSpinBox* m_bpDia;

    // Glucosa
    QDoubleSpinBox* m_glucSpin;

    QDateTimeEdit*  m_dateTime;
    QLineEdit*      m_notes;
};

#endif // ADDMEASUREMENTDIALOG_H

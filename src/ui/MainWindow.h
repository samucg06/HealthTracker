#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**
 * @file MainWindow.h
 * @brief Ventana principal del rastreador de salud.
 */

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QtCharts/QChartView>
#include "database/DatabaseManager.h"

QT_CHARTS_USE_NAMESPACE

/**
 * @class MainWindow
 * @brief Ventana central de la aplicación: tabla de mediciones,
 *        resumen estadístico y gráfica de tendencia.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const UserRecord& user, QWidget* parent = nullptr);

private slots:
    void onAddMeasurement();
    void onDeleteMeasurement();
    void onFilterChanged(int index);
    void onExportCSV();
    void onCalorieCalculator();

private:
    void setupUi();
    void setupMenuBar();
    void refreshTable();
    void refreshChart();
    void updateStatsBar();

    UserRecord m_user;

    QComboBox*    m_filterCombo;
    QTableWidget* m_table;
    QLabel*       m_statsLabel;
    QChartView*   m_chartView;

    QVector<MeasurementRecord> m_currentData;
};

#endif // MAINWINDOW_H

/**
 * @file MainWindow.cpp
 * @brief Implementación de la ventana principal de HealthTracker.
 */

#include "MainWindow.h"
#include "AddMeasurementDialog.h"
#include "CalorieDialog.h"
#include "database/DatabaseManager.h"
#include "utils/StatsCalculator.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QGroupBox>
#include <QHeaderView>
#include <QSplitter>
#include <QDateTime>
#include <QStatusBar>

// QtCharts
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QSplineSeries>

QT_CHARTS_USE_NAMESPACE

// ─────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────

MainWindow::MainWindow(const UserRecord& user, QWidget* parent)
    : QMainWindow(parent), m_user(user) {
    setWindowTitle(QString("HealthTracker — %1").arg(user.fullName));
    setMinimumSize(900, 620);
    setupUi();
    setupMenuBar();
    refreshTable();
    refreshChart();
    updateStatsBar();
    statusBar()->showMessage("Listo");
}

// ─────────────────────────────────────────────
//  Construcción de la UI
// ─────────────────────────────────────────────

void MainWindow::setupUi() {
    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(10, 8, 10, 8);
    root->setSpacing(8);

    // ── Barra de estadísticas ─────────────────
    auto* statsGroup = new QGroupBox("📊 Resumen");
    auto* statsLayout = new QHBoxLayout(statsGroup);
    m_statsLabel = new QLabel("Sin datos registrados.");
    m_statsLabel->setWordWrap(true);
    statsLayout->addWidget(m_statsLabel);
    root->addWidget(statsGroup);

    // ── Splitter: tabla | gráfica ─────────────
    auto* splitter = new QSplitter(Qt::Horizontal);

    // Lado izquierdo: filtros + tabla
    auto* leftWidget = new QWidget();
    auto* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(6);

    // Fila de filtro + botones
    auto* filterRow = new QHBoxLayout();
    filterRow->addWidget(new QLabel("Filtro:"));
    m_filterCombo = new QComboBox();
    m_filterCombo->addItems({"Todos", "⚖️  Peso", "❤️  Presión arterial", "🩸 Glucosa"});
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);
    filterRow->addWidget(m_filterCombo);
    filterRow->addStretch();

    auto* addBtn = new QPushButton("➕ Agregar");
    addBtn->setMinimumWidth(110);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddMeasurement);
    filterRow->addWidget(addBtn);

    auto* delBtn = new QPushButton("🗑  Eliminar");
    delBtn->setMinimumWidth(110);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::onDeleteMeasurement);
    filterRow->addWidget(delBtn);

    leftLayout->addLayout(filterRow);

    // Tabla
    m_table = new QTableWidget(0, 5, leftWidget);
    m_table->setHorizontalHeaderLabels({"Tipo", "Valor 1", "Valor 2", "Fecha / Hora", "Notas"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    leftLayout->addWidget(m_table);

    splitter->addWidget(leftWidget);

    // Lado derecho: gráfica
    auto* chartGroup = new QGroupBox("📈 Tendencia");
    auto* chartLayout = new QVBoxLayout(chartGroup);
    m_chartView = new QChartView();
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumWidth(280);
    chartLayout->addWidget(m_chartView);
    splitter->addWidget(chartGroup);

    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);
    root->addWidget(splitter);
}

void MainWindow::setupMenuBar() {
    // ── Archivo ───────────────────────────────
    auto* fileMenu = menuBar()->addMenu("&Archivo");

    auto* exportAct = new QAction("Exportar CSV...", this);
    exportAct->setShortcut(QKeySequence("Ctrl+E"));
    connect(exportAct, &QAction::triggered, this, &MainWindow::onExportCSV);
    fileMenu->addAction(exportAct);

    fileMenu->addSeparator();

    auto* exitAct = new QAction("Salir", this);
    exitAct->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAct, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAct);

    // ── Herramientas ──────────────────────────
    auto* toolsMenu = menuBar()->addMenu("&Herramientas");
    auto* calAct = new QAction("Calculadora de calorías", this);
    calAct->setShortcut(QKeySequence("Ctrl+K"));
    connect(calAct, &QAction::triggered, this, &MainWindow::onCalorieCalculator);
    toolsMenu->addAction(calAct);

    // ── Acerca de ─────────────────────────────
    auto* helpMenu = menuBar()->addMenu("A&yuda");
    auto* aboutAct = new QAction("Acerca de HealthTracker", this);
    connect(aboutAct, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "HealthTracker",
            "<b>HealthTracker v1.0</b><br>"
            "Rastreador Personal de Salud<br><br>"
            "Universidad ECI — Algoritmos en Sistemas Electrónicos<br>"
            "C++17 · Qt5 · SQLite3 · Doxygen");
    });
    helpMenu->addAction(aboutAct);
}

// ─────────────────────────────────────────────
//  Actualización de datos
// ─────────────────────────────────────────────

void MainWindow::refreshTable() {
    // Determinar filtro
    const QStringList typeKeys = {"", "weight", "blood_pressure", "glucose"};
    QString filterType = typeKeys[m_filterCombo->currentIndex()];

    m_currentData = DatabaseManager::instance().getMeasurements(m_user.id, filterType);

    m_table->setRowCount(0);
    for (const auto& r : m_currentData) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        QString typeStr, v1Str, v2Str;
        if (r.type == "weight") {
            typeStr = "⚖️  Peso";
            v1Str   = QString("%1 kg").arg(r.value1, 0, 'f', 1);
            v2Str   = "—";
        } else if (r.type == "blood_pressure") {
            typeStr = "❤️  Presión";
            v1Str   = QString("%1 mmHg").arg(r.value1, 0, 'f', 0);
            v2Str   = QString("%1 mmHg").arg(r.value2, 0, 'f', 0);
        } else if (r.type == "glucose") {
            typeStr = "🩸 Glucosa";
            v1Str   = QString("%1 mg/dL").arg(r.value1, 0, 'f', 1);
            v2Str   = "—";
        }

        m_table->setItem(row, 0, new QTableWidgetItem(typeStr));
        m_table->setItem(row, 1, new QTableWidgetItem(v1Str));
        m_table->setItem(row, 2, new QTableWidgetItem(v2Str));
        m_table->setItem(row, 3, new QTableWidgetItem(r.recordedAt));
        m_table->setItem(row, 4, new QTableWidgetItem(r.notes));

        // Alinear números al centro
        m_table->item(row, 1)->setTextAlignment(Qt::AlignCenter);
        m_table->item(row, 2)->setTextAlignment(Qt::AlignCenter);
    }
}

void MainWindow::updateStatsBar() {
    auto weights  = DatabaseManager::instance().getMeasurements(m_user.id, "weight");
    auto bps      = DatabaseManager::instance().getMeasurements(m_user.id, "blood_pressure");
    auto glucoses = DatabaseManager::instance().getMeasurements(m_user.id, "glucose");

    QStringList parts;

    if (!weights.isEmpty()) {
        double last = weights.first().value1;
        double avg  = StatsCalculator::average(weights, 1);
        double bmi  = StatsCalculator::calculateBMI(last, m_user.heightCm / 100.0);
        double slp  = StatsCalculator::trend(weights);
        QString trend = (slp > 0.01) ? "↑" : (slp < -0.01) ? "↓" : "→";
        parts << QString("⚖️  Peso: %1 kg (prom. %2) IMC %3 [%4] %5")
                    .arg(last, 0,'f',1).arg(avg, 0,'f',1)
                    .arg(bmi,  0,'f',1)
                    .arg(StatsCalculator::bmiCategory(bmi))
                    .arg(trend);
    }
    if (!bps.isEmpty()) {
        double sys = StatsCalculator::average(bps, 1);
        double dia = StatsCalculator::average(bps, 2);
        parts << QString("❤️  Presión: %1/%2 mmHg")
                    .arg(sys, 0,'f',0).arg(dia, 0,'f',0);
    }
    if (!glucoses.isEmpty()) {
        double avg = StatsCalculator::average(glucoses, 1);
        parts << QString("🩸 Glucosa: %1 mg/dL").arg(avg, 0,'f',1);
    }

    m_statsLabel->setText(parts.isEmpty()
        ? "Sin datos. Agrega tu primera medición con ➕."
        : parts.join("     |     "));
}

void MainWindow::refreshChart() {
    const QStringList typeKeys = {"", "weight", "blood_pressure", "glucose"};
    QString filterType = typeKeys[m_filterCombo->currentIndex()];
    if (filterType.isEmpty()) filterType = "weight"; // por defecto grafica peso

    auto data = DatabaseManager::instance().getMeasurements(m_user.id, filterType);

    auto* series = new QSplineSeries();
    series->setName(filterType == "weight"          ? "Peso (kg)"       :
                    filterType == "blood_pressure"   ? "Sistólica (mmHg)" :
                                                       "Glucosa (mg/dL)");

    // QtCharts necesita los puntos ordenados ASC
    for (int i = data.size() - 1; i >= 0; --i) {
        QDateTime dt = QDateTime::fromString(data[i].recordedAt, "yyyy-MM-dd HH:mm:ss");
        if (dt.isValid())
            series->append(dt.toMSecsSinceEpoch(), data[i].value1);
    }

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(filterType == "weight"         ? "Evolución del Peso" :
                    filterType == "blood_pressure"  ? "Presión Sistólica"  :
                                                      "Glucosa en Sangre");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->hide();

    if (series->count() >= 2) {
        auto* axisX = new QDateTimeAxis();
        axisX->setFormat("dd/MM");
        axisX->setTitleText("Fecha");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        auto* axisY = new QValueAxis();
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
    } else {
        chart->createDefaultAxes();
    }

    m_chartView->setChart(chart);
}

// ─────────────────────────────────────────────
//  Slots
// ─────────────────────────────────────────────

void MainWindow::onAddMeasurement() {
    AddMeasurementDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    bool ok = DatabaseManager::instance().addMeasurement(
        m_user.id,
        dlg.getType(),
        dlg.getValue1(),
        dlg.getValue2(),
        dlg.getRecordedAt(),
        dlg.getNotes()
    );

    if (ok) {
        refreshTable();
        refreshChart();
        updateStatsBar();
        statusBar()->showMessage("Medición guardada.", 3000);
    } else {
        QMessageBox::warning(this, "Error", "No se pudo guardar la medición.");
    }
}

void MainWindow::onDeleteMeasurement() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_currentData.size()) {
        QMessageBox::information(this, "Selección",
            "Selecciona una medición de la tabla para eliminarla.");
        return;
    }

    auto reply = QMessageBox::question(this, "Confirmar eliminación",
        "¿Deseas eliminar esta medición? Esta acción no se puede deshacer.",
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    if (DatabaseManager::instance().deleteMeasurement(m_currentData[row].id)) {
        refreshTable();
        refreshChart();
        updateStatsBar();
        statusBar()->showMessage("Medición eliminada.", 3000);
    }
}

void MainWindow::onFilterChanged(int /*index*/) {
    refreshTable();
    refreshChart();
}

void MainWindow::onExportCSV() {
    if (m_currentData.isEmpty()) {
        QMessageBox::information(this, "Sin datos", "No hay mediciones para exportar.");
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this, "Exportar a CSV",
        QString("salud_%1.csv").arg(QDate::currentDate().toString("yyyyMMdd")),
        "Archivos CSV (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No se pudo crear el archivo.");
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "Tipo,Valor1,Unidad1,Valor2,Unidad2,FechaHora,Notas\n";
    for (const auto& r : m_currentData) {
        QString u1 = (r.type == "weight") ? "kg"
                   : (r.type == "blood_pressure") ? "mmHg" : "mg/dL";
        QString u2 = (r.type == "blood_pressure") ? "mmHg" : "";
        out << r.type    << ","
            << r.value1  << ","
            << u1        << ","
            << r.value2  << ","
            << u2        << ","
            << r.recordedAt << ","
            << r.notes   << "\n";
    }
    file.close();

    QMessageBox::information(this, "Exportación exitosa",
        QString("Datos exportados a:\n%1").arg(path));
    statusBar()->showMessage("CSV exportado.", 3000);
}

void MainWindow::onCalorieCalculator() {
    CalorieDialog dlg(m_user, this);
    dlg.exec();
}

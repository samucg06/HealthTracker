/**
 * @file StatsCalculator.cpp
 * @brief Implementación de cálculos estadísticos y de salud.
 */

#include "StatsCalculator.h"
#include <cmath>

// ─────────────────────────────────────────────
//  Promedio
// ─────────────────────────────────────────────

double StatsCalculator::average(const QVector<MeasurementRecord>& data, int valueIdx) {
    if (data.isEmpty()) return 0.0;
    double sum = 0.0;
    for (const auto& r : data)
        sum += (valueIdx == 1) ? r.value1 : r.value2;
    return sum / static_cast<double>(data.size());
}

// ─────────────────────────────────────────────
//  IMC
// ─────────────────────────────────────────────

double StatsCalculator::calculateBMI(double weightKg, double heightM) {
    if (heightM <= 0.0) return 0.0;
    return weightKg / (heightM * heightM);
}

QString StatsCalculator::bmiCategory(double bmi) {
    if (bmi <= 0)    return "N/A";
    if (bmi < 18.5)  return "Bajo peso";
    if (bmi < 25.0)  return "Normal";
    if (bmi < 30.0)  return "Sobrepeso";
    if (bmi < 35.0)  return "Obesidad I";
    return "Obesidad II+";
}

// ─────────────────────────────────────────────
//  Calorías (Mifflin-St Jeor)
// ─────────────────────────────────────────────

double StatsCalculator::calculateTDEE(double weightKg, double heightCm,
                                       int age, const QString& gender,
                                       const QString& activityLevel) {
    // TMB (Tasa Metabólica Basal) - Mifflin-St Jeor
    double bmr = 10.0 * weightKg
               + 6.25 * heightCm
               - 5.0  * static_cast<double>(age);

    if (gender == "Masculino") bmr += 5.0;
    else                       bmr -= 161.0;

    // Factor de actividad
    double factor = 1.2;
    if      (activityLevel == "Sedentario")  factor = 1.200;
    else if (activityLevel == "Ligero")      factor = 1.375;
    else if (activityLevel == "Moderado")    factor = 1.550;
    else if (activityLevel == "Activo")      factor = 1.725;
    else if (activityLevel == "Muy activo")  factor = 1.900;

    return bmr * factor;
}

double StatsCalculator::adjustForGoal(double tdee, const QString& goal) {
    if (goal.contains("Déficit"))   return tdee - 500.0;
    if (goal.contains("Superávit")) return tdee + 300.0;
    return tdee;  // Mantenimiento
}

// ─────────────────────────────────────────────
//  Tendencia lineal simple (pendiente)
// ─────────────────────────────────────────────

double StatsCalculator::trend(const QVector<MeasurementRecord>& data) {
    int n = data.size();
    if (n < 2) return 0.0;

    // Los datos vienen de más reciente a más antiguo → invertir el índice x
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    for (int i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = data[n - 1 - i].value1;   // de antiguo a reciente
        sumX  += x;
        sumY  += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    double denom = n * sumX2 - sumX * sumX;
    if (std::abs(denom) < 1e-9) return 0.0;
    return (n * sumXY - sumX * sumY) / denom;
}

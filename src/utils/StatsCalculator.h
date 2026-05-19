#ifndef STATSCALCULATOR_H
#define STATSCALCULATOR_H

/**
 * @file StatsCalculator.h
 * @brief Clase utilitaria para cálculos estadísticos y de salud.
 *
 * Incluye promedio, IMC y cálculo de kilocalorías diarias
 * usando la ecuación de Mifflin-St Jeor.
 */

#include <QVector>
#include <QString>
#include "database/DatabaseManager.h"

/**
 * @class StatsCalculator
 * @brief Contiene métodos estáticos para análisis biométrico.
 *
 * No requiere instanciación; todos los métodos son estáticos.
 */
class StatsCalculator {
public:
    /**
     * @brief Calcula el promedio de value1 o value2 de un conjunto de mediciones.
     * @param data     Vector de mediciones.
     * @param valueIdx 1 = value1 (ej. peso), 2 = value2 (ej. diastólica).
     * @return Promedio aritmético, o 0.0 si el vector está vacío.
     */
    static double average(const QVector<MeasurementRecord>& data, int valueIdx);

    /**
     * @brief Calcula el Índice de Masa Corporal (IMC / BMI).
     * @param weightKg Peso en kilogramos.
     * @param heightM  Estatura en metros.
     * @return IMC = peso / estatura²
     */
    static double calculateBMI(double weightKg, double heightM);

    /**
     * @brief Retorna la categoría OMS del IMC dado.
     */
    static QString bmiCategory(double bmi);

    /**
     * @brief Calcula el Gasto Energético Total Diario (TDEE) con Mifflin-St Jeor.
     * @param weightKg      Peso actual (kg).
     * @param heightCm      Estatura (cm).
     * @param age           Edad en años.
     * @param gender        "Masculino" | "Femenino" | "Otro"
     * @param activityLevel "Sedentario" | "Ligero" | "Moderado" | "Activo" | "Muy activo"
     * @return kcal/día estimadas.
     */
    static double calculateTDEE(double weightKg, double heightCm, int age,
                                  const QString& gender,
                                  const QString& activityLevel);

    /**
     * @brief Ajusta el TDEE según el objetivo del usuario.
     * @param tdee TDEE base calculado.
     * @param goal "Déficit (bajar peso)" | "Mantenimiento" | "Superávit (ganar masa)"
     * @return kcal objetivo por día.
     */
    static double adjustForGoal(double tdee, const QString& goal);

    /**
     * @brief Calcula la tendencia lineal simple (pendiente) de value1.
     * @return > 0 tendencia al alza, < 0 a la baja, 0 sin datos.
     */
    static double trend(const QVector<MeasurementRecord>& data);

private:
    StatsCalculator() = delete;  ///< Clase no instanciable
};

#endif // STATSCALCULATOR_H

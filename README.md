# 🏥 HealthTracker
**Rastreador Personal de Salud**  
Universidad Escuela Colombiana de Ingeniería Julio Garavito — Algoritmos en Sistemas Electrónicos  
C++17 · Qt5 · SQLite3 · Doxygen

---

## ⚙️ Instalación de dependencias (Linux)

```bash
chmod +x install_deps.sh
./install_deps.sh
```

O manualmente en Ubuntu/Debian:
```bash
sudo apt update
sudo apt install -y build-essential cmake git \
    qtbase5-dev qt5-qmake libqt5charts5-dev \
    libsqlite3-dev doxygen graphviz
```

---

## 🔨 Compilación

```bash
# Entrar al directorio del proyecto
cd HealthTracker

# Generar Makefile con qmake
qmake HealthTracker.pro

# Compilar (usa todos los núcleos disponibles)
make -j$(nproc)

# Ejecutar
./HealthTracker
```

---

## 🗂️ Estructura del proyecto

```
HealthTracker/
├── HealthTracker.pro          # Archivo de proyecto Qt
├── Doxyfile                   # Configuración de documentación
├── install_deps.sh            # Script de instalación
├── README.md
└── src/
    ├── main.cpp               # Punto de entrada
    ├── database/
    │   ├── DatabaseManager.h  # Interfaz SQLite3 (Singleton)
    │   └── DatabaseManager.cpp
    ├── ui/
    │   ├── LoginDialog.h/cpp          # Autenticación
    │   ├── MainWindow.h/cpp           # Ventana principal
    │   ├── AddMeasurementDialog.h/cpp # Agregar medición
    │   └── CalorieDialog.h/cpp        # Calculadora kcal
    └── utils/
        ├── StatsCalculator.h  # Cálculos estadísticos
        └── StatsCalculator.cpp
```

---

## 📋 Funcionalidades

| Funcionalidad | Estado |
|---|---|
| Registro e inicio de sesión | ✅ |
| Registro de peso | ✅ |
| Registro de presión arterial | ✅ |
| Registro de glucosa | ✅ |
| Filtrado por tipo | ✅ |
| Promedios y tendencias | ✅ |
| IMC con categoría OMS | ✅ |
| Calculadora de kcal (Mifflin-St Jeor) | ✅ |
| Exportación CSV | ✅ |
| Gráfica de tendencia (QtCharts) | ✅ |
| Contraseñas con SHA-256 | ✅ |

---

## 📄 Documentación Doxygen

```bash
doxygen Doxyfile
# Abre docs/html/index.html en tu navegador
xdg-open docs/html/index.html
```

---

## 🗄️ Modelo Entidad-Relación

```
users                         measurements
─────────────────             ──────────────────────────
id          PK INT            id          PK INT
username    TEXT UNIQUE       user_id     FK → users.id
password    TEXT (SHA-256)    type        TEXT
full_name   TEXT              value1      REAL
birth_year  INTEGER           value2      REAL
height_cm   REAL              recorded_at TEXT (ISO 8601)
gender      TEXT              notes       TEXT
```

---

## 🧪 Bonus implementado

- **IMC + categoría OMS** en el panel de estadísticas
- **Gráfica dinámica** con QtCharts (tendencia por tipo)
- **Tendencia lineal** (↑ ↓ →) calculada por regresión simple

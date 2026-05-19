#!/bin/bash
# ============================================================
#  Script de instalación - HealthTracker
#  Universidad ECI - Algoritmos en Sistemas Electrónicos
# ============================================================

set -e

echo ""
echo "╔══════════════════════════════════════════════╗"
echo "║   Instalando dependencias para HealthTracker  ║"
echo "╚══════════════════════════════════════════════╝"
echo ""

# Detectar distribución
if [ -f /etc/debian_version ]; then
    DISTRO="debian"
elif [ -f /etc/arch-release ]; then
    DISTRO="arch"
elif [ -f /etc/fedora-release ]; then
    DISTRO="fedora"
else
    DISTRO="unknown"
fi

echo "► Distribución detectada: $DISTRO"
echo ""

if [ "$DISTRO" = "debian" ]; then
    echo "► Actualizando repositorios..."
    sudo apt update

    echo "► Instalando herramientas de compilación..."
    sudo apt install -y build-essential cmake git

    echo "► Instalando Qt5..."
    sudo apt install -y \
        qtbase5-dev \
        qt5-qmake \
        qttools5-dev \
        qttools5-dev-tools \
        libqt5widgets5 \
        libqt5charts5-dev \
        qt5-default 2>/dev/null || true

    # En Ubuntu 22.04+ qt5-default no existe, usamos qtchooser
    if ! command -v qmake &> /dev/null; then
        sudo apt install -y qtchooser
        sudo qtchooser -install qt5 $(which qmake5) 2>/dev/null || true
        export QT_SELECT=qt5
    fi

    echo "► Instalando SQLite3..."
    sudo apt install -y libsqlite3-dev sqlite3

    echo "► Instalando Doxygen..."
    sudo apt install -y doxygen graphviz

elif [ "$DISTRO" = "arch" ]; then
    sudo pacman -Syu --noconfirm
    sudo pacman -S --noconfirm base-devel cmake git qt5-base qt5-charts sqlite doxygen graphviz

elif [ "$DISTRO" = "fedora" ]; then
    sudo dnf update -y
    sudo dnf install -y gcc-c++ cmake git qt5-qtbase-devel qt5-qtcharts-devel sqlite-devel doxygen graphviz

else
    echo "⚠ Distribución no reconocida. Instala manualmente:"
    echo "  - build-essential / gcc-c++"
    echo "  - cmake, git"
    echo "  - qtbase5-dev, libqt5charts5-dev"
    echo "  - libsqlite3-dev"
    echo "  - doxygen, graphviz"
    exit 1
fi

echo ""
echo "╔══════════════════════════════════════════════╗"
echo "║        ✔  Instalación completada             ║"
echo "╚══════════════════════════════════════════════╝"
echo ""
echo "Para compilar el proyecto:"
echo "  cd HealthTracker"
echo "  qmake HealthTracker.pro"
echo "  make -j$(nproc)"
echo "  ./HealthTracker"
echo ""

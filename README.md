# Monitor de Procesos en C++ con Análisis de Riesgo y Registro Estructurado

## Descripción General
Monitor de procesos en tiempo real que detecta actividades sospechosas basándose en:
- Consumo anómalo de memoria
- Nombres de proceso inusuales
- Lista negra personalizable
- Modos de análisis (básico/estricto)

## Integrantes del Equipo
- Ángel Josué Arcos Guerrero - josuee43 - Módulo Procesos
- Andrea Aylin Abundiz Hérnandez - Jinblush22 - Módulo Análisis  
- Johan Emmanuel Garay Garza - zagreus10 - Módulo Registro

## Instrucciones de Compilación

### Requisitos
- **Windows**: MinGW o Visual Studio, Windows SDK
- **Linux**: g++ (4.8+), make
- **macOS**: Clang, Xcode Command Line Tools

### Compilación

# Windows (MinGW)
g++ -std=c++11 main.cpp Procesos.cpp Analisis.cpp Registro.cpp -o monitor.exe -lpsapi
monitor.exe

## Entrada esperada
Modo de análisis:
1 = Básico
2 = Estricto

## Enfoque técnico
Este proyecto utiliza APIs nativas del sistema operativo con compilación condicional:
Windows: CreateToolhelp32Snapshot + GetProcessMemoryInfo

## Criterio de procesos sospechosos
ALTO		Patrón malicioso o consumo excesivo
MEDIO		Comportamiento anómalo
BAJO		Desconocido pero no peligroso

## Salida generada
========================================
MONITOR DE PROCESOS - REPORTE DE ANÁLISIS
========================================

Fecha y hora: 2026-02-12 15:30:45
Total de procesos detectados: 127

--- LISTA COMPLETA DE PROCESOS ---
PID        NOMBRE                         MEMORIA (MB)
-------------------------------------------------------
4          System                         0.08
123        svchost.exe                   45.23
456        chrome.exe                   812.45
789        explorer.exe                 120.34
...

--- PROCESOS SOSPECHOSOS ---

PID: 456
Nombre: chrome.exe
Nivel de riesgo: MEDIO
Razón: Consumo de memoria excesivo: 812.45 MB

##Referencias
https://www.ibm.com/docs/es/ibm-mq/9.3.x?topic=programs-building-c-windows
https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-process32first

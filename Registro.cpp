#include "Registro.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <ctime>

void GestorRegistro::guardarReporte(const std::string& nombre_archivo,
                                   const std::vector<Proceso*>& procesos,
                                   const std::vector<ProcesoSospechoso>& sospechosos) {
    
    std::ofstream archivo(nombre_archivo);
    
    if (!archivo.is_open()) {
        throw std::ios_base::failure("No se pudo abrir el archivo: " + nombre_archivo);
    }
    
    try {
        archivo << "========================================\n";
        archivo << "MONITOR DE PROCESOS - REPORTE DE ANÁLISIS\n";
        archivo << "========================================\n\n";

        archivo << "Fecha y hora: " << obtenerFechaHora() << "\n";
        archivo << "Total de procesos detectados: " << procesos.size() << "\n\n";

        archivo << "--- LISTA COMPLETA DE PROCESOS ---\n";
        archivo << std::left << std::setw(10) << "PID" 
                << std::setw(30) << "NOMBRE" 
                << std::setw(15) << "MEMORIA (MB)" << "\n";
        archivo << std::string(55, '-') << "\n";
        
        for (const auto& p : procesos) {
            archivo << std::left << std::setw(10) << p->pid
                    << std::setw(30) << p->nombre
                    << std::setw(15) << std::fixed << std::setprecision(2) 
                    << p->uso_memoria << "\n";
        }

        archivo << "\n--- PROCESOS SOSPECHOSOS ---\n\n";
        
        if (sospechosos.empty()) {
            archivo << "No se encontraron procesos sospechosos.\n";
        } else {
            for (const auto& s : sospechosos) {
                archivo << "PID: " << s.proceso->pid << "\n";
                archivo << "Nombre: " << s.proceso->nombre << "\n";
                archivo << "Nivel de riesgo: " << nivelRiesgoToString(s.riesgo) << "\n";
                archivo << "Razón: " << s.razon << "\n";
                archivo << std::string(30, '-') << "\n";
            }
        }
        
        archivo.close();
        std::cout << "✅ Reporte guardado exitosamente en: " << nombre_archivo << std::endl;
        
    } catch (const std::exception& e) {
        archivo.close();
        throw std::ios_base::failure("Error escribiendo en el archivo: " + std::string(e.what()));
    }
}

std::string GestorRegistro::obtenerFechaHora() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string GestorRegistro::nivelRiesgoToString(NivelRiesgo riesgo) {
    switch (riesgo) {
        case NivelRiesgo::BAJO: return "BAJO";
        case NivelRiesgo::MEDIO: return "MEDIO";
        case NivelRiesgo::ALTO: return "ALTO";
        case NivelRiesgo::CRITICO: return "CRÍTICO";
        default: return "DESCONOCIDO";
    }
}
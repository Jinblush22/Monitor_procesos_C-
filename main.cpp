#include "Procesos.h"
#include "Analisis.h"
#include "Registro.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <memory>

void limpiarBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::cout << "=== MONITOR DE PROCESOS ===\n\n";
    
    try {
        int modo;
        std::cout << "Modo de análisis:\n";
        std::cout << "1 = Básico\n";
        std::cout << "2 = Estricto\n";
        std::cout << "Seleccione: ";
        
        if (!(std::cin >> modo) || (modo != 1 && modo != 2)) {
            throw std::invalid_argument("Modo de análisis inválido");
        }
        limpiarBuffer();

        std::string archivo;
        std::cout << "Nombre del archivo de salida: ";
        std::getline(std::cin, archivo);
        
        if (archivo.empty()) {
            throw std::invalid_argument("El nombre del archivo no puede estar vacío");
        }
        
        std::vector<std::string> procesos_sospechosos;

        std::cout << "\nObteniendo procesos del sistema...\n";
        std::vector<Proceso*> procesos = GestorProcesos::obtenerProcesosActivos();
        std::cout << " " << procesos.size() << " procesos detectados\n\n";

        std::cout << "--- PROCESOS ACTIVOS ---\n";
        for (const auto& p : procesos) {
            std::cout << "PID: " << p->pid 
                      << " | Nombre: " << p->nombre 
                      << " | Memoria: " << p->uso_memoria << " MB\n";
        }

        std::cout << "\nAnalizando procesos...\n";
        AnalizadorRiesgo analizador(modo, procesos_sospechosos);
        std::vector<ProcesoSospechoso> sospechosos = analizador.analizarProcesos(procesos);
        
        std::cout << "\nPROCESOS SOSPECHOSOS:\n";
        if (sospechosos.empty()) {
            std::cout << "No se encontraron procesos sospechosos.\n";
        } else {
            for (const auto& s : sospechosos) {
                std::cout << "PID: " << s.proceso->pid << "\n";
                std::cout << "Nombre: " << s.proceso->nombre << "\n";
                std::cout << "Riesgo: " << GestorRegistro::nivelRiesgoToString(s.riesgo) << "\n";
                std::cout << "Razón: " << s.razon << "\n\n";
            }
        }

        std::cout << "Guardando reporte...\n";
        GestorRegistro::guardarReporte(archivo, procesos, sospechosos);

        for (auto& p : procesos) {
            delete p;
        }
        procesos.clear();
        
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error de entrada: " << e.what() << std::endl;
        return 1;
    } catch (const std::ios_base::failure& e) {
        std::cerr << "Error de archivo: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error inesperado: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nPrograma finalizado exitosamente.\n";
    return 0;
}
#include "Analisis.h"
#include <algorithm>
#include <regex>
#include <set>

const std::set<std::string> PROCESOS_CONOCIDOS = {
    "System", "svchost.exe", "explorer.exe", "winlogon.exe",
    "bash", "init", "systemd", "kernel_task", "launchd",
    "chrome", "firefox", "Code", "Teams", "Spotify"
};

const std::vector<std::regex> PATRONES_SOSPECHOSOS = {
    std::regex(".*(crypt|miner|coin).*", std::regex_constants::icase),
    std::regex(".*(keylog|spy|rat).*", std::regex_constants::icase),
    std::regex("[a-z]{1,4}\\.exe", std::regex_constants::icase),
    std::regex(".*(backdoor|trojan).*", std::regex_constants::icase)
};

AnalizadorRiesgo::AnalizadorRiesgo(int modo_analisis, 
                                   const std::vector<std::string>& procesos_usuario)
    : modo(modo_analisis), lista_negra_usuario(procesos_usuario) {}

std::vector<ProcesoSospechoso> AnalizadorRiesgo::analizarProcesos(
    const std::vector<Proceso*>& procesos) {
    
    std::vector<ProcesoSospechoso> sospechosos;
    
    for (const auto& p : procesos) {
        NivelRiesgo riesgo = evaluarProceso(p);
        
        if (riesgo != NivelRiesgo::BAJO || modo == 2) {
            ProcesoSospechoso ps;
            ps.proceso = p;
            ps.riesgo = riesgo;

            if (esProcesoConocido(p->nombre) && modo == 2) {
                ps.razon = "Proceso legítimo pero en modo análisis estricto";
            } else if (tieneNombreSospechoso(p->nombre)) {
                ps.razon = "Nombre de proceso sospechoso";
            } else if (memoriaExcesiva(p->uso_memoria)) {
                ps.razon = "Consumo de memoria excesivo: " + 
                          std::to_string(p->uso_memoria) + " MB";
            } else if (std::find(lista_negra_usuario.begin(), 
                                lista_negra_usuario.end(), 
                                p->nombre) != lista_negra_usuario.end()) {
                ps.razon = "Marcado por el usuario como sospechoso";
            }
            
            sospechosos.push_back(ps);
        }
    }
    
    return sospechosos;
}

NivelRiesgo AnalizadorRiesgo::evaluarProceso(const Proceso* p) {
    auto it = std::find(lista_negra_usuario.begin(), 
                        lista_negra_usuario.end(), p->nombre);
    if (it != lista_negra_usuario.end()) {
        return NivelRiesgo::CRITICO;
    }

    if (tieneNombreSospechoso(p->nombre)) {
        return NivelRiesgo::ALTO;
    }

    if (memoriaExcesiva(p->uso_memoria)) {
        return modo == 1 ? NivelRiesgo::MEDIO : NivelRiesgo::ALTO;
    }

    if (!esProcesoConocido(p->nombre)) {
        return modo == 1 ? NivelRiesgo::BAJO : NivelRiesgo::MEDIO;
    }
    
    return NivelRiesgo::BAJO;
}

bool AnalizadorRiesgo::esProcesoConocido(const std::string& nombre) {
    return PROCESOS_CONOCIDOS.find(nombre) != PROCESOS_CONOCIDOS.end();
}

bool AnalizadorRiesgo::tieneNombreSospechoso(const std::string& nombre) {
    for (const auto& patron : PATRONES_SOSPECHOSOS) {
        if (std::regex_match(nombre, patron)) {
            return true;
        }
    }
    return false;
}

bool AnalizadorRiesgo::memoriaExcesiva(double uso_memoria) {
    return uso_memoria > (modo == 1 ? 500.0 : 200.0);
}
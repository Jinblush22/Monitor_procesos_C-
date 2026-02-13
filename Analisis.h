#ifndef ANALISIS_H
#define ANALISIS_H

#include "Procesos.h"
#include <string>
#include <vector>

enum class NivelRiesgo {
    BAJO,
    MEDIO,
    ALTO,
    CRITICO
};

struct ProcesoSospechoso {
    Proceso* proceso;
    NivelRiesgo riesgo;
    std::string razon;
};

class AnalizadorRiesgo {
public:
    AnalizadorRiesgo(int modo_analisis, 
                     const std::vector<std::string>& procesos_usuario = {});
    
    std::vector<ProcesoSospechoso> analizarProcesos(
        const std::vector<Proceso*>& procesos);
    
private:
    int modo;
    std::vector<std::string> lista_negra_usuario;

    NivelRiesgo evaluarProceso(const Proceso* p);
    bool esProcesoConocido(const std::string& nombre);
    bool tieneNombreSospechoso(const std::string& nombre);
    bool memoriaExcesiva(double uso_memoria);
};

#endif
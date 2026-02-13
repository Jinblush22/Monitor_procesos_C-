#ifndef REGISTRO_H
#define REGISTRO_H

#include "Procesos.h"
#include "Analisis.h"
#include <string>
#include <vector>
#include <fstream>

class GestorRegistro {
public:

    static void guardarReporte(const std::string& nombre_archivo,
                              const std::vector<Proceso*>& procesos,
                              const std::vector<ProcesoSospechoso>& sospechosos);
    
    static std::string nivelRiesgoToString(NivelRiesgo riesgo);
    
private:
    static std::string obtenerFechaHora();
};

#endif
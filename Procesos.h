#ifndef PROCESOS_H
#define PROCESOS_H

#include <string>
#include <vector>
#include <stdexcept>

struct Proceso {
    int pid;
    std::string nombre;
    double uso_memoria;
};

class GestorProcesos {
public:

    static std::vector<Proceso*> obtenerProcesosActivos();
    
private:
    #ifdef _WIN32
    static std::vector<Proceso*> obtenerProcesosWindows();
    #elif __linux__
    static std::vector<Proceso*> obtenerProcesosLinux();
    #elif __APPLE__
    static std::vector<Proceso*> obtenerProcesosMac();
    #endif
};

#endif
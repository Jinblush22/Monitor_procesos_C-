#include "Procesos.h"
#include <iostream>
#include <cstdlib>
#include <memory>
#include <regex>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#elif __linux__
#include <dirent.h>
#include <fstream>
#endif

std::vector<Proceso*> GestorProcesos::obtenerProcesosActivos() {
    std::vector<Proceso*> procesos;
    
    try {
        #ifdef _WIN32
        procesos = obtenerProcesosWindows();
        #elif __linux__
        procesos = obtenerProcesosLinux();
        #elif __APPLE__
        procesos = obtenerProcesosMac();
        #else
        throw std::runtime_error("Sistema operativo no soportado");
        #endif
    }
    catch (const std::exception& e) {
        std::cerr << "Error obteniendo procesos: " << e.what() << std::endl;
        throw; 
    }
    
    return procesos;
}

#ifdef _WIN32
std::vector<Proceso*> GestorProcesos::obtenerProcesosWindows() {
    std::vector<Proceso*> procesos;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("No se pudo crear snapshot de procesos");
    }
    
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(snapshot, &pe)) {
        do {
            Proceso* p = new Proceso();
            p->pid = pe.th32ProcessID;
            p->nombre = pe.szExeFile;
            
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
                                         FALSE, pe.th32ProcessID);
            if (hProcess) {
                PROCESS_MEMORY_COUNTERS pmc;
                if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                    p->uso_memoria = pmc.WorkingSetSize / (1024.0 * 1024.0); // MB
                }
                CloseHandle(hProcess);
            }
            
            procesos.push_back(p);
        } while (Process32Next(snapshot, &pe));
    }
    
    CloseHandle(snapshot);
    return procesos;
}
#endif

#ifdef __linux__
std::vector<Proceso*> GestorProcesos::obtenerProcesosLinux() {
    std::vector<Proceso*> procesos;
    DIR* proc_dir = opendir("/proc");
    
    if (!proc_dir) {
        throw std::runtime_error("No se pudo abrir /proc");
    }
    
    struct dirent* entry;
    while ((entry = readdir(proc_dir)) != nullptr) {
        if (entry->d_type == DT_DIR && std::isdigit(entry->d_name[0])) {
            Proceso* p = new Proceso();
            p->pid = std::stoi(entry->d_name);

            std::string stat_path = "/proc/" + std::string(entry->d_name) + "/comm";
            std::ifstream comm_file(stat_path);
            if (comm_file) {
                std::getline(comm_file, p->nombre);
                comm_file.close();
            }

            std::string status_path = "/proc/" + std::string(entry->d_name) + "/status";
            std::ifstream status_file(status_path);
            std::string line;
            while (std::getline(status_file, line)) {
                if (line.find("VmRSS:") == 0) {
                    std::regex mem_regex("\\d+");
                    std::smatch match;
                    if (std::regex_search(line, match, mem_regex)) {
                        p->uso_memoria = std::stoi(match.str()) / 1024.0; // KB a MB
                    }
                    break;
                }
            }
            status_file.close();
            
            procesos.push_back(p);
        }
    }
    
    closedir(proc_dir);
    return procesos;
}
#endif

#ifdef __APPLE__
std::vector<Proceso*> GestorProcesos::obtenerProcesosMac() {
    std::vector<Proceso*> procesos;
    FILE* pipe = popen("ps -axo pid,rss,comm | tail -n +2", "r");
    
    if (!pipe) {
        throw std::runtime_error("No se pudo ejecutar ps");
    }
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        int pid;
        long rss;
        char nombre[128];
        
        if (sscanf(buffer, "%d %ld %127s", &pid, &rss, nombre) == 3) {
            Proceso* p = new Proceso();
            p->pid = pid;
            p->nombre = nombre;
            p->uso_memoria = rss * 4096 / (1024.0 * 1024.0); // Páginas a MB
            procesos.push_back(p);
        }
    }
    
    pclose(pipe);
    return procesos;
}
#endif
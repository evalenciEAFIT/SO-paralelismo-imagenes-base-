#include <iostream>
#include <string>
#include <fstream>
#include "jpeg_manager.h"
#include "procesador_imagen.h"
#include <omp.h>

// Muestra las instrucciones de uso del programa
void mostrar_uso(const char* nombre_programa) {
    std::cout << "Uso: " << nombre_programa << " <archivo_entrada> [archivo_salida]" << std::endl;
    std::cout << "Si no se especifica archivo_salida, se usará 'imagen_procesada.jpg'" << std::endl;
}

// Verifica si un archivo existe
bool archivo_existe(const std::string& nombre_archivo) {
    std::ifstream f(nombre_archivo.c_str());
    return f.good();
}

// Función principal del programa
// Pseudocódigo:
// 1. Verificar argumentos de línea de comandos
// 2. Obtener nombres de archivos de entrada y salida
// 3. Verificar existencia del archivo de entrada
// 4. Leer archivo JPEG de entrada
// 5. Procesar la imagen (invertir colores)
// 6. Crear nuevo archivo JPEG con la imagen procesada
int main(int argc, char* argv[]) {
    // 1. Verificar argumentos de línea de comandos
    if (argc < 2 || argc > 3) {
        mostrar_uso(argv[0]);
        return 1;
    }

    // 2. Obtener nombres de archivos de entrada y salida
    std::string archivo_entrada = argv[1];
    std::string archivo_salida = (argc == 3) ? argv[2] : "imagen_procesada.jpg";

    // 3. Verificar existencia del archivo de entrada
    if (!archivo_existe(archivo_entrada)) {
        std::cerr << "Error: El archivo de entrada '" << archivo_entrada << "' no existe." << std::endl;
        return 1;
    }

    jpeg_manager gestor;

    // 4. Leer archivo JPEG de entrada
    if (!gestor.leer_archivo(archivo_entrada)) {
        std::cerr << "Error al leer el archivo: " << archivo_entrada << std::endl;
        return 1;
    }

    std::cout << "Archivo " << archivo_entrada << " leído exitosamente." << std::endl;

    auto encabezado = gestor.ver_encabezado();
    std::cout << "Dimensiones de la imagen: " << encabezado.ancho << "x" << encabezado.alto << std::endl;

    auto matriz = gestor.obtener_matriz_pixeles();

    if (matriz.empty() || matriz[0].empty() || matriz[0][0].empty()) {
        std::cerr << "Error: La matriz de píxeles está vacía" << std::endl;
        return 1;
    }

    try {
        // 5. Procesar la imagen (invertir colores)
        ProcesadorImagen::invertir_colores(matriz);
    } catch (const std::exception& e) {
        std::cerr << "Error al procesar la imagen: " << e.what() << std::endl;
        return 1;
    }

    // 6. Crear nuevo archivo JPEG con la imagen procesada
    if (!gestor.crear_archivo(archivo_salida, matriz)) {
        std::cerr << "Error al crear el nuevo archivo: " << archivo_salida << std::endl;
        return 1;
    }

    std::cout << "Nuevo archivo creado exitosamente: " << archivo_salida << std::endl;
    return 0;
}
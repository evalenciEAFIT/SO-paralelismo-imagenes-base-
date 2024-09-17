#ifndef JPEG_MANAGER_H
#define JPEG_MANAGER_H

#include <string>
#include <vector>

// Clase para gestionar operaciones de lectura y escritura de archivos JPEG
class jpeg_manager {
private:
    // Estructura para almacenar información básica del encabezado de un archivo JPEG
    struct Encabezado {
        int ancho;    // Ancho de la imagen en píxeles
        int alto;     // Alto de la imagen en píxeles
        int canales;  // Número de canales de color (normalmente 3 para RGB)
    };

    Encabezado encabezado;  // Almacena la información del encabezado de la imagen actual
    std::vector<std::vector<std::vector<unsigned char>>> matriz_pixeles;  // Matriz 3D para almacenar los datos de píxeles

public:
    jpeg_manager();
    ~jpeg_manager();

    // Lee un archivo JPEG y almacena su contenido
    bool leer_archivo(const std::string& nombre_archivo);

    // Obtiene la información del encabezado de la imagen actual
    Encabezado ver_encabezado() const;

    // Obtiene la matriz de píxeles de la imagen actual
    std::vector<std::vector<std::vector<unsigned char>>> obtener_matriz_pixeles() const;

    // Crea un nuevo archivo JPEG a partir de una matriz de píxeles
    bool crear_archivo(const std::string& nombre_archivo, const std::vector<std::vector<std::vector<unsigned char>>>& nueva_matriz);
};

#endif // JPEG_MANAGER_H
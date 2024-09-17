#ifndef PROCESADOR_IMAGEN_H
#define PROCESADOR_IMAGEN_H

#include <vector>

class ProcesadorImagen {
public:
    // Invierte los colores de la imagen
    // Pseudocódigo:
    // 1. Iterar sobre todos los píxeles de la imagen
    // 2. Para cada canal de color, invertir su valor (255 - valor)
    static void invertir_colores(std::vector<std::vector<std::vector<unsigned char>>>& matriz);

    // Aquí puedes añadir más funciones de procesamiento de imágenes en el futuro
    // Por ejemplo:
    // static void ajustar_brillo(std::vector<std::vector<std::vector<unsigned char>>>& matriz, int factor);
    // static void aplicar_filtro_sepia(std::vector<std::vector<std::vector<unsigned char>>>& matriz);
};

#endif // PROCESADOR_IMAGEN_H
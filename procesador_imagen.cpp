#include "procesador_imagen.h"
#include <omp.h>

void ProcesadorImagen::invertir_colores(std::vector<std::vector<std::vector<unsigned char>>>& matriz) {
    // Invertir los colores de la imagen
    #pragma omp parallel for collapse(3)
    for (size_t y = 0; y < matriz.size(); ++y) {
        for (size_t x = 0; x < matriz[0].size(); ++x) {
            for (size_t c = 0; c < matriz[0][0].size(); ++c) {
                matriz[y][x][c] = 255 - matriz[y][x][c];
            }
        }
    }
}

// Aquí puedes implementar más funciones de procesamiento de imágenes en el futuro
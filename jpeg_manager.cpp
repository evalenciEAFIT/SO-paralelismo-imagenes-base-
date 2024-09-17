#include "jpeg_manager.h"
#include <iostream>
#include <fstream>
#include <jpeglib.h>
#include <cstring>
#include <cerrno>
#include <memory>
#include <omp.h>

jpeg_manager::jpeg_manager() : encabezado{0, 0, 0} {}

jpeg_manager::~jpeg_manager() {}

// Lee un archivo JPEG y almacena su contenido
// Pseudocódigo:
// 1. Abrir el archivo JPEG
// 2. Inicializar el decodificador JPEG
// 3. Leer el encabezado del archivo
// 4. Iniciar la descompresión
// 5. Leer los datos de la imagen línea por línea
// 6. Almacenar los datos en la matriz_pixeles
// 7. Finalizar la descompresión y cerrar el archivo
bool jpeg_manager::leer_archivo(const std::string& nombre_archivo) {
    // 1. Abrir el archivo JPEG
    FILE* infile = fopen(nombre_archivo.c_str(), "rb");
    if (!infile) {
        std::cerr << "Error al abrir el archivo " << nombre_archivo << ": " << strerror(errno) << std::endl;
        return false;
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // 2. Inicializar el decodificador JPEG
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);

    try {
        // 3. Leer el encabezado del archivo
        if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
            throw std::runtime_error("Error al leer el encabezado JPEG");
        }

        // 4. Iniciar la descompresión
        jpeg_start_decompress(&cinfo);

        encabezado.ancho = cinfo.output_width;
        encabezado.alto = cinfo.output_height;
        encabezado.canales = cinfo.output_components;

        if (encabezado.canales != 3) {
            throw std::runtime_error("Solo se soportan imágenes RGB (3 canales)");
        }

        int row_stride = cinfo.output_width * cinfo.output_components;
        auto buffer = std::make_unique<JSAMPLE[]>(row_stride);
        JSAMPROW row_pointer[1];
        row_pointer[0] = buffer.get();

        matriz_pixeles.resize(encabezado.alto, std::vector<std::vector<unsigned char>>(encabezado.ancho, std::vector<unsigned char>(encabezado.canales)));

        // 5. Leer los datos de la imagen línea por línea
        while (cinfo.output_scanline < cinfo.output_height) {
            jpeg_read_scanlines(&cinfo, row_pointer, 1);
            int y = cinfo.output_scanline - 1;
            
            // 6. Almacenar los datos en la matriz_pixeles
            #pragma omp parallel for
            for (JDIMENSION x = 0; x < cinfo.output_width; ++x) {
                for (int c = 0; c < encabezado.canales; ++c) {
                    matriz_pixeles[y][x][c] = buffer[x * encabezado.canales + c];
                }
            }
        }

        // 7. Finalizar la descompresión y cerrar el archivo
        jpeg_finish_decompress(&cinfo);
    } catch (const std::exception& e) {
        std::cerr << "Error al procesar el archivo JPEG: " << e.what() << std::endl;
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return false;
    }

    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return true;
}

// Obtiene la información del encabezado de la imagen actual
jpeg_manager::Encabezado jpeg_manager::ver_encabezado() const {
    return encabezado;
}

// Obtiene la matriz de píxeles de la imagen actual
std::vector<std::vector<std::vector<unsigned char>>> jpeg_manager::obtener_matriz_pixeles() const {
    return matriz_pixeles;
}

// Crea un nuevo archivo JPEG a partir de una matriz de píxeles
// Pseudocódigo:
// 1. Verificar la validez de la matriz de entrada
// 2. Abrir un nuevo archivo para escritura
// 3. Inicializar el codificador JPEG
// 4. Configurar los parámetros de compresión
// 5. Iniciar la compresión
// 6. Escribir los datos de la imagen línea por línea
// 7. Finalizar la compresión y cerrar el archivo
bool jpeg_manager::crear_archivo(const std::string& nombre_archivo, const std::vector<std::vector<std::vector<unsigned char>>>& nueva_matriz) {
    // 1. Verificar la validez de la matriz de entrada
    if (nueva_matriz.empty() || nueva_matriz[0].empty() || nueva_matriz[0][0].empty()) {
        std::cerr << "Error: La matriz de píxeles está vacía" << std::endl;
        return false;
    }

    // 2. Abrir un nuevo archivo para escritura
    FILE* outfile = fopen(nombre_archivo.c_str(), "wb");
    if (!outfile) {
        std::cerr << "Error al crear el archivo " << nombre_archivo << ": " << strerror(errno) << std::endl;
        return false;
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // 3. Inicializar el codificador JPEG
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    // 4. Configurar los parámetros de compresión
    cinfo.image_width = nueva_matriz[0].size();
    cinfo.image_height = nueva_matriz.size();
    cinfo.input_components = nueva_matriz[0][0].size();
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);

    // 5. Iniciar la compresión
    jpeg_start_compress(&cinfo, TRUE);

    std::vector<unsigned char> buffer(cinfo.image_width * cinfo.input_components);
    JSAMPROW row_pointer[1];
    row_pointer[0] = buffer.data();

    // 6. Escribir los datos de la imagen línea por línea
    while (cinfo.next_scanline < cinfo.image_height) {
        JDIMENSION y = cinfo.next_scanline;
        #pragma omp parallel for
        for (JDIMENSION x = 0; x < cinfo.image_width; ++x) {
            for (int c = 0; c < cinfo.input_components; ++c) {
                buffer[x * cinfo.input_components + c] = nueva_matriz[y][x][c];
            }
        }
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // 7. Finalizar la compresión y cerrar el archivo
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    return true;
}
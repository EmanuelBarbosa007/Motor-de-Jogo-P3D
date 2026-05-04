#pragma once


// ==============================================================
// Ficheiro:    Texture.h
// Descrição:   Declaração da classe Texture
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: GLEW, OpenGL, stb_image.h (para carregamento de texturas)
// Observações:
// Esta classe representa uma textura que pode ser aplicada a objetos 3D.
// Ela encapsula o carregamento de texturas a partir de ficheiros e a sua vinculação ao contexto OpenGL.
// ==============================================================


#include <string>


namespace game_engine_p3d {

    class Texture {
    public:
        // ------------------------------------------------------------
        // Construtores e destrutores
        // ------------------------------------------------------------
        Texture(const std::string& path = "");
        ~Texture();
        // ------------------------------------------------------------
        // Accessors e mutators
        // ------------------------------------------------------------
        std::string path() const { return path_; } // Retorna o caminho da textura
        // ------------------------------------------------------------
        void set_texture_unit(unsigned int unit) { texture_unit_ = unit; }
        // ------------------------------------------------------------
        void Use() const;

    private:
        std::string path_{};                    // Caminho para o ficheiro de textura
		unsigned int texture_unit_ = 0;         // Unidade de textura (0, 1, 2, ...)
        unsigned int texture_name_{};           // ID (nome) da textura OpenGL
        int width_{}, height_{}, channels_{};   // Dimensões e canais da textura
        bool loaded_ = false;                   // Indica se a textura foi carregada com sucesso
    };
}
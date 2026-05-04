#define GLEW_STATIC // Definição necessária, antes de incluir 'GL\glew.h', sempre que se usa GLEW como uma biblioteca estática
#include <GL/glew.h>

#include "Common.h" // Para a macro LOG
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Biblioteca para carregamento de imagens


namespace game_engine_p3d {

	Texture::Texture(const std::string& path)
		: path_{ path } {
        
        if(path.empty()) {
            LOG("Error: Texture path is empty. Cannot create Texture object without a path.");
            exit(EXIT_FAILURE); // Encerra o programa com erro
		}

        // Adiciona o caminho base dos assets ao nome do ficheiro
        std::string image_filename_full = kAssetPath + path_;

		LOG("Creating Texture object with path: '" + image_filename_full << "'");

        glGenTextures(1, &texture_name_);

        glActiveTexture(GL_TEXTURE0 + texture_unit_);

        glBindTexture(GL_TEXTURE_2D, texture_name_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int width, height, nChannels;
        stbi_set_flip_vertically_on_load(true);
        // FIXME: Use the correct path to the texture file
        unsigned char* imageData = stbi_load(image_filename_full.c_str(), &width, &height, &nChannels, 0);
        if (imageData) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);

            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(imageData);
        }
        else {
            LOG("Error loading texture!");
        }
	}

    Texture::~Texture() {
        // FIXME: Implementar a lógica de destruição da textura
        if (texture_name_ != 0) {
            glDeleteTextures(1, &texture_name_);
            texture_name_ = 0;
        }
        LOG("Texture destroyed.");
    }

    void Texture::Use() const {
        // Indica a unidade de textura a usar
        glActiveTexture(GL_TEXTURE0 + texture_unit_);

        // Liga a textura a um ponto de ligação do tipo textura 2D da unidade de textura ativa
        glBindTexture(GL_TEXTURE_2D, texture_name_);

        // FIXME: Corrigir isto. Está hardcoded e não é correto. Poderão estar aqui.
        // OBTÉM O SHADER ATIVO ATUALMENTE:
        GLint current_program = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

        if (current_program != 0) {
            // Usa a constante kTextureSamplerName definida no Common.h (que é "texture1")
            GLint location_texture2D = glGetProgramResourceLocation(current_program, GL_PROGRAM_INPUT, kTextureSamplerName);

            // Se a função acima não funcionar (no caso de ser uniform e não resource), usar:
            location_texture2D = glGetUniformLocation(current_program, kTextureSamplerName);

            glProgramUniform1i(current_program, location_texture2D, texture_unit_ /* Unidade de Textura */);

            LOG("Texture bound to OpenGL context:" << " [Texture Unit: " << texture_unit_ << "]  [Texture 2D location: " << location_texture2D << "]");
        }
        else {
            LOG("Error: No active shader program found when trying to bind texture.");
        }
    }
}
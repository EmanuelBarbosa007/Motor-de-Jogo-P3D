#pragma once

#include <string>
#include <vector>
#include <memory> // <-- ADICIONADO PARA OS SMART POINTERS
#include <glm/glm.hpp>
#include "Texture.h"

namespace game_engine_p3d {
    // Declarações antecipadas
    class Shader;

    class Material {
    public:
        // ------------------------------------------------------------
        // Construtores e destrutores
        // ------------------------------------------------------------
        Material(const Shader* shader, const std::string obj_filename);
        Material(
            glm::vec3 ka = glm::vec3{ 0.2f, 0.2f, 0.2f },
            glm::vec3 kd = glm::vec3{ 0.8f, 0.8f, 0.8f },
            glm::vec3 ks = glm::vec3{ 1.0f, 1.0f, 1.0f },
            glm::vec3 ke = glm::vec3{ 0.0f, 0.0f, 0.0f },
            float Ns = 32.0f,
            float Ni = 1.0f,
            float d = 1.0f,
            int illum = 2,
            Shader* shader = nullptr,
            std::vector<std::shared_ptr<Texture>> textures_ = {} // <-- ALTERADO PARA SMART POINTERS
        );
        ~Material() = default;

        // ------------------------------------------------------------
        // Accessors e mutators
        // ------------------------------------------------------------
        Shader* shader() const { return shader_; }

        // ------------------------------------------------------------
        // Outras funções-membro
        // ------------------------------------------------------------
        void Use() const; // Ativa o shader e a textura do material

    private:
        // ------------------------------------------------------------
        // Funções-membro privadas
        // ------------------------------------------------------------
        bool findMaterialFile(const std::string& obj_filename);
        void LoadMaterialFromFile(void);

        // ------------------------------------------------------------
        // Dados-membro privados
        // ------------------------------------------------------------
        std::string mtl_filename_{};
        Shader* shader_ = nullptr;

        std::vector<std::shared_ptr<Texture>> texture_{}; // <-- ALTERADO PARA SMART POINTERS

        glm::vec3 ka_{};
        glm::vec3 kd_{};
        glm::vec3 ks_{};
        glm::vec3 ke_{};
        float Ns_{};
        float Ni_{};
        float d_{};
        int illum_{};
    };
}
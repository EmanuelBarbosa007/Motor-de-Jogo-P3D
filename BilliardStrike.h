#pragma once

#include "Behaviour.h"
#include "Object.h"
#include "Game.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstdlib> // Para o std::rand()
#include <string>

namespace game_engine_p3d {

    class BilliardStrike : public Behaviour {
    private:
        bool is_moving_ = false;
        glm::vec3 velocity_{ 0.0f };
        bool has_been_hit_ = false;

    public:
        BilliardStrike() = default;
        ~BilliardStrike() override = default;

        void Update(Object& obj) override {
            GLFWwindow* window = glfwGetCurrentContext();

            // Inicia o movimento de TODAS as bolas ao clicar no ESPAÇO
            if (window && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !has_been_hit_) {
                is_moving_ = true;
                has_been_hit_ = true; // Impede múltiplas tacadas

                // Gera uma direção e força aleatórias
                float vx = ((std::rand() % 200) / 100.0f) - 1.0f;
                float vz = ((std::rand() % 200) / 100.0f) - 1.0f;

                velocity_ = glm::vec3(vx * 15.0f, 0.0f, vz * 15.0f);
            }

            if (is_moving_) {
                // Posição atual desta bola
                glm::vec3 my_pos = glm::vec3(obj.model().matrix_[3]);

                // 1. LIMITES DA MESA: Parar se chegar à borda do pano verde
                if (my_pos.x < -18.0f || my_pos.x > 18.0f || my_pos.z < -28.0f || my_pos.z > 20.0f) {
                    is_moving_ = false;
                    velocity_ = glm::vec3(0.0f);
                }

                // 2. COLISÃO ENTRE BOLAS
                if (obj.game() != nullptr && is_moving_) {
                    for (int i = 1; i <= 15; i++) {
                        Object* other = obj.game()->FindObjectByName("Bola " + std::to_string(i));

                        // Garante que a bola não tenta colidir consigo própria
                        if (other && other->name() != obj.name()) {
                            glm::vec3 other_pos = glm::vec3(other->model().matrix_[3]);

                            // A CORREÇÃO: Usar < 2.0f em vez de <= 2.1f!
                            // As bolas começam com 2.1 de distância. Se usarmos 2.0, elas 
                            // só batem se o choque for real depois de se começarem a mexer.
                            if (glm::distance(my_pos, other_pos) < 2.0f) {
                                is_moving_ = false;
                                velocity_ = glm::vec3(0.0f);
                                break;
                            }
                        }
                    }
                }

                // 3. MOVIMENTO E ATRITO
                if (is_moving_) {
                    obj.model().Translate(velocity_.x * 0.016f, velocity_.y * 0.016f, velocity_.z * 0.016f);
                    obj.model().Rotate(velocity_.z * 3.0f, 0.0f, -velocity_.x * 3.0f);

                    velocity_ *= 0.985f; // Atrito
                    if (glm::length(velocity_) < 0.1f) {
                        is_moving_ = false;
                        velocity_ = glm::vec3(0.0f);
                    }
                }
            }
        }
    };
}
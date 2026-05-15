#pragma once

#include "Behaviour.h"
#include "Light.h"
#include "Object.h"
#include <GLFW/glfw3.h>

namespace game_engine_p3d {

    class LightController : public Behaviour {
    private:
        Light* l_ambient_;
        Light* l_dir_;
        Light* l_point_;
        Light* l_spot_;

        // Guardam o estado (Ligado = true / Desligado = false)
        bool state1 = true, state2 = true, state3 = true, state4 = true;
        // Evitam que a luz pisque freneticamente se deixares o dedo na tecla
        bool press1 = false, press2 = false, press3 = false, press4 = false;

        // Guardam a cor original da luz para a podermos repor
        glm::vec3 orig_ambient, orig_dir, orig_point, orig_spot;

    public:
        LightController(Light* a, Light* d, Light* p, Light* s)
            : l_ambient_(a), l_dir_(d), l_point_(p), l_spot_(s) {

            if (a) orig_ambient = a->ambient();
            if (d) orig_dir = d->diffuse();
            if (p) orig_point = p->diffuse();
            if (s) orig_spot = s->diffuse();
        }

        void Update(Object& obj) override {
            GLFWwindow* window = glfwGetCurrentContext();
            if (!window) return;

            // Tecla 1 - Alternar Luz Ambiente
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                if (!press1) {
                    state1 = !state1;
                    l_ambient_->set_ambient(state1 ? orig_ambient : glm::vec3(0.0f));
                    press1 = true;
                }
            }
            else press1 = false;

            // Tecla 2 - Alternar Luz Direcional
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                if (!press2) {
                    state2 = !state2;
                    l_dir_->set_diffuse(state2 ? orig_dir : glm::vec3(0.0f));
                    l_dir_->set_specular(state2 ? orig_dir : glm::vec3(0.0f));
                    press2 = true;
                }
            }
            else press2 = false;

            // Tecla 3 - Alternar Luz Pontual
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
                if (!press3) {
                    state3 = !state3;
                    l_point_->set_diffuse(state3 ? orig_point : glm::vec3(0.0f));
                    l_point_->set_specular(state3 ? orig_point : glm::vec3(0.0f));
                    press3 = true;
                }
            }
            else press3 = false;

            // Tecla 4 - Alternar Luz Cónica (Spotlight)
            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
                if (!press4) {
                    state4 = !state4;
                    l_spot_->set_diffuse(state4 ? orig_spot : glm::vec3(0.0f));
                    l_spot_->set_specular(state4 ? orig_spot : glm::vec3(0.0f));
                    press4 = true;
                }
            }
            else press4 = false;
        }
    };
}
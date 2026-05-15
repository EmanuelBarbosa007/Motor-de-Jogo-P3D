#include <iostream>
#include <string>
#include <vector>

#include "Game.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "Shader.h"
#include "Renderer.h"

#include "BilliardStrike.h" 
#include "LightController.h"

using namespace game_engine_p3d;

int main() {
    Game game(1280, 720, "Trabalho Pratico - Mesa de Bilhar");

    // CÂMARA: Imóvel orientada para o centro, com visão total da mesa
    Camera* camera = new Camera();
    camera->set_background_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    camera->LookAt(
        glm::vec3(0.0f, 22.0f, 25.0f),  // Posição: Alta e atrás
        glm::vec3(0.0f, 0.0f, -2.0f),   // Centro: Orientada para o meio
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    camera->Prespective(40.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
    game.AddCamera(camera);

    // 4 FONTES DE LUZ (Requisito)

    // Luz Ambiente (Tecla 1)
    Light* ambient_light = new Light();
    ambient_light->set_type(LightType::kAmbient);
    ambient_light->set_ambient(glm::vec3(0.4f, 0.4f, 0.4f));
    game.AddLight(ambient_light);

    // Luz Direcional (Tecla 2)
    Light* dir_light = new Light();
    dir_light->set_type(LightType::kDirectional);
    dir_light->set_direction(glm::vec3(0.5f, -1.0f, -0.5f));
    dir_light->set_diffuse(glm::vec3(0.3f, 0.3f, 0.3f));
    game.AddLight(dir_light);

    // Luz Pontual (Tecla 3)
    Light* point_light = new Light();
    point_light->set_type(LightType::kPoint);
    point_light->set_position(glm::vec3(0.0f, 5.0f, -4.0f));
    point_light->set_diffuse(glm::vec3(0.7f, 0.1f, 0.1f));
    point_light->set_specular(glm::vec3(0.7f, 0.1f, 0.1f));
    point_light->set_constant(1.0f);
    point_light->set_linear(0.02f);
    game.AddLight(point_light);

    // Luz "Dummy" (Oculta para evitar bugs do OpenGL)
    Light* point_light_dummy = new Light();
    point_light_dummy->set_type(LightType::kPoint);
    point_light_dummy->set_constant(1.0f);
    game.AddLight(point_light_dummy);

    // Luz Cónica (Tecla 4)
    Light* spot_light = new Light();
    spot_light->set_type(LightType::kSpotlight);
    spot_light->set_position(glm::vec3(0.0f, 25.0f, -5.0f));
    spot_light->set_direction(glm::vec3(0.0f, -1.0f, 0.0f));
    spot_light->set_diffuse(glm::vec3(1.2f, 1.2f, 1.2f));
    spot_light->set_specular(glm::vec3(1.5f, 1.5f, 1.5f));
    spot_light->set_constant(1.0f);
    spot_light->set_linear(0.002f);
    spot_light->set_cutOff(45.0f);
    game.AddLight(spot_light);

    // SHADER
    std::vector<ShaderSource> shader_sources = {
        {GL_VERTEX_SHADER, "light.vert"},
        {GL_FRAGMENT_SHADER, "light.frag"}
    };
    Shader* shader = new Shader(shader_sources, "Shader de Luz");

    // CONTROLADOR DE LUZES
    LightController* l_controller = new LightController(ambient_light, dir_light, point_light, spot_light);

    // MESA DE BILHAR (Onde acoplamos o LightController para ouvir o teclado)
    Renderer* renderer_mesa = new Renderer(shader, "mesa.obj");
    Object* mesa = new Object(
        "Mesa de Bilhar",
        "Default",
        l_controller,
        renderer_mesa,
        0.0f, -1.15f, -5.0f,
        0.0f, 0.0f, 0.0f,
        20.0f, 0.1f, 30.0f
    );
    game.AddObject(mesa);

    // POSIÇÕES DO TRIÂNGULO COMPLETO (15 BOLAS)
    float d = 2.1f;
    float h = d * 0.866f;

    glm::vec3 posicoes[15] = {
        glm::vec3(0.0f, 0.0f, 0.0f),                                                            // Linha 1 (Vértice do triângulo)
        glm::vec3(-d / 2, 0.0f, -h), glm::vec3(d / 2, 0.0f, -h),                                // Linha 2
        glm::vec3(-d, 0.0f, -2 * h), glm::vec3(0.0f, 0.0f, -2 * h), glm::vec3(d, 0.0f, -2 * h), // Linha 3
        glm::vec3(-1.5f * d, 0.0f, -3 * h), glm::vec3(-0.5f * d, 0.0f, -3 * h), glm::vec3(0.5f * d, 0.0f, -3 * h), glm::vec3(1.5f * d, 0.0f, -3 * h), // Linha 4
        glm::vec3(-2.0f * d, 0.0f, -4 * h), glm::vec3(-1.0f * d, 0.0f, -4 * h), glm::vec3(0.0f, 0.0f, -4 * h), glm::vec3(1.0f * d, 0.0f, -4 * h), glm::vec3(2.0f * d, 0.0f, -4 * h) // Linha 5
    };

    // CRIAR AS 15 BOLAS COM A ANIMAÇÃO ATIVA
    for (int i = 1; i <= 15; i++) {
        std::string filename = "Ball" + std::to_string(i) + ".obj";
        Renderer* renderer = new Renderer(shader, filename);

        // Atribui uma nova instância do comportamento a cada bola
        BilliardStrike* strike = new BilliardStrike();

        Object* ball = new Object(
            "Bola " + std::to_string(i),
            "Default",
            strike,                         // Agora a animação é igual para TODAS
            renderer,
            posicoes[i - 1].x, posicoes[i - 1].y, posicoes[i - 1].z
        );
        game.AddObject(ball);
    }

    // Iniciar Jogo
    game.Run();
    return 0;
}
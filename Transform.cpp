#include "Common.h" // Inclui definições comuns e macros para o motor de jogo
#include "Transform.h"

namespace game_engine_p3d {

	Transform::Transform(float x, float y, float z)
		: position_(x, y, z), orientation_quat_(1.0f, 0.0f, 0.0f, 0.0f), scale_(1.0f) {

		UpdateMatrix();
		LOG("Transform created at position: (" << x << ", " << y << ", " << z << ")");
	}

	Transform::Transform(float x, float y, float z, float pitch, float yaw, float roll, float scale_x, float scale_y, float scale_z)
		: position_(x, y, z), scale_(scale_x, scale_y, scale_z) {

		// Inicializa o quaternion a partir dos ângulos de Euler (convertidos para radianos)
		orientation_quat_ = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));

		UpdateMatrix();
		LOG("Transform created at position: (" << x << ", " << y << ", " << z << ") with orientation: (" << pitch << ", " << yaw << ", " << roll << ") and scale: (" << scale_x << ", " << scale_y << ", " << scale_z << ")");
	}

	void Transform::Translate(float delta_x, float delta_y, float delta_z) {
		position_ += glm::vec3(delta_x, delta_y, delta_z);
		UpdateMatrix();
		LOG("Translated by (" << delta_x << ", " << delta_y << ", " << delta_z << ")");
	}

	void Transform::Rotate(float delta_pitch, float delta_yaw, float delta_roll) {
		// 1. Cria um novo quaternion com os valores de rotação recebidos (deltas)
		glm::quat delta_rot = glm::quat(glm::vec3(glm::radians(delta_pitch), glm::radians(delta_yaw), glm::radians(delta_roll)));

		// 2. Multiplica a rotação delta pela rotação atual (no mundo 3D aplica a rotação!)
		orientation_quat_ = delta_rot * orientation_quat_;

		// 3. Normaliza o quaternion para evitar distorções de ponto flutuante (floating point drift)
		orientation_quat_ = glm::normalize(orientation_quat_);

		UpdateMatrix();
		LOG("Rotated by (" << delta_pitch << ", " << delta_yaw << ", " << delta_roll << ")");
	}

	void Transform::Scale(float scale_x, float scale_y, float scale_z) {
		scale_ *= glm::vec3(scale_x, scale_y, scale_z);
		UpdateMatrix();
		LOG("Scaled by (" << scale_x << ", " << scale_y << ", " << scale_z << ")");
	}

	void Transform::UpdateMatrix() {
		// A ordem correta de multiplicação para transformar um modelo 3D é: Translação * Rotação * Escala (TRS)
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), position_);
		glm::mat4 rot = glm::mat4_cast(orientation_quat_); // Converte o quaternion numa matriz 4x4
		glm::mat4 scl = glm::scale(glm::mat4(1.0f), scale_);

		matrix_ = trans * rot * scl;
	}
}
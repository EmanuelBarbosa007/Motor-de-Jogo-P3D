#include <fstream>
#include <sstream>
#include <algorithm>

#include "Common.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"

namespace game_engine_p3d {

	Material::Material(const Shader* shader, const std::string obj_filename) {
		if (!shader) {
			LOG("Error: Shader is null. Cannot create Material object without a shader.");
			exit(EXIT_FAILURE);
		}
		if (obj_filename.empty()) {
			LOG("Error: Object filename is empty. Cannot create Material object without an object path.");
			exit(EXIT_FAILURE);
		}

		LOG("Creating Material with shader and OBJ file: " + obj_filename);

		shader_ = (Shader*)shader;

		std::string obj_filename_full = kAssetPath + obj_filename;

		if (findMaterialFile(obj_filename_full) == false) {
			LOG("No MTL file found in OBJ file: '" << obj_filename_full << "'");
			return;
		}

		LoadMaterialFromFile();

		if (kDebugMode) {
			LOG("Material properties:");
			LOG("  Ambient Color: " << ka_.r << " " << ka_.g << " " << ka_.b);
			LOG("  Diffuse Color: " << kd_.r << " " << kd_.g << " " << kd_.b);
			LOG("  Specular Color: " << ks_.r << " " << ks_.g << " " << ks_.b);
			LOG("  Emissive Color: " << ke_.r << " " << ke_.g << " " << ke_.b);
			LOG("  Shininess: " << Ns_);
			LOG("  Index of Refraction: " << Ni_);
			LOG("  Opacity: " << d_);
			LOG("  Illumination Model: " << illum_);

			// CORREÇÃO: Usar a seta (->) para aceder aos métodos do ponteiro inteligente
			if (texture_.size() >= 1) LOG("  Texture: '" << texture_.at(0)->path() << "'");
			else LOG("  Texture: None");
			if (texture_.size() >= 2) LOG("  Normal Map: '" << texture_.at(1)->path() << "'");
			else LOG("  Normal Map: None");
		}

		LOG("Material created with shader and object path: " + obj_filename);
	}

	Material::Material(glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 ke, float Ns, float Ni, float d, int illum,
		Shader* shader, std::vector<std::shared_ptr<Texture>> textures) {
		ka_ = ka;
		kd_ = kd;
		ks_ = ks;
		ke_ = ke;
		Ns_ = Ns;
		Ni_ = Ni;
		d_ = d;
		illum_ = illum;
		shader_ = shader;
		texture_ = textures;
	}

	void Material::Use() const {
		if (shader_) {
			shader_->Use();
		}

		// CORREÇÃO: Usar seta (->) para ativar as texturas armazenadas nos ponteiros
		for (const auto& tex : texture_) {
			tex->Use();
		}

		if (shader_) {
			shader_->SetUniform<glm::vec3>(kMaterialAmbientName, ka_);
			shader_->SetUniform<glm::vec3>(kMaterialDiffuseName, kd_);
			shader_->SetUniform<glm::vec3>(kMaterialSpecularName, ks_);
			shader_->SetUniform<glm::vec3>(kMaterialEmissiveName, ke_);
			shader_->SetUniform<float>(kMaterialShininessName, Ns_);
			shader_->SetUniform<float>(kMaterialIndexOfRefractionName, Ni_);
			shader_->SetUniform<float>(kMaterialOpacityName, d_);
			shader_->SetUniform<int>(kMaterialIlluminationModelName, illum_);
		}

		LOG("Material used.");
	}

	bool Material::findMaterialFile(const std::string& obj_filename) {
		std::string delimiter = "mtllib ";
		std::string mtlFile;

		LOG("Looking for MTL file in OBJ file: '" << obj_filename << "'");

		std::ifstream obj_file(obj_filename);
		if (!obj_file.is_open()) {
			LOG("Failed to open OBJ file: '" << obj_filename << "'");
			return false;
		}

		std::string obj_line;
		while (std::getline(obj_file, obj_line)) {
			size_t pos = obj_line.find(delimiter, 0);
			if (pos != std::string::npos) {
				mtlFile = obj_line.substr(pos + delimiter.length(), obj_line.find('\n', pos) - (pos + delimiter.length()));
				mtlFile.erase(std::remove(mtlFile.begin(), mtlFile.end(), '\r'), mtlFile.end());
				obj_file.close();
				mtl_filename_ = mtlFile;
				return true;
			}
		}

		return false;
	}

	void Material::LoadMaterialFromFile(void) {
		std::string mtl_filename_full = kAssetPath + mtl_filename_;
		LOG("Loading material properties from file: '" << mtl_filename_full << "'");

		// Muda a locale para "C" para ler os pontos decimais corretamente
		auto old_locale = std::locale();
		std::locale::global(std::locale("C"));

		std::ifstream file(mtl_filename_full);
		if (!file.is_open()) {
			LOG("Failed to open MTL file: '" << mtl_filename_full << "'");
			std::locale::global(old_locale);
			return;
		}

		std::string line;
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::string prefix;
			iss >> prefix;

			if (prefix == "Ka") { iss >> ka_.r >> ka_.g >> ka_.b; }
			else if (prefix == "Kd") { iss >> kd_.r >> kd_.g >> kd_.b; }
			else if (prefix == "Ks") { iss >> ks_.r >> ks_.g >> ks_.b; }
			else if (prefix == "Ke") { iss >> ke_.r >> ke_.g >> ke_.b; }
			else if (prefix == "Ns") { iss >> Ns_; }
			else if (prefix == "Ni") { iss >> Ni_; }
			else if (prefix == "d") { iss >> d_; }
			else if (prefix == "illum") { iss >> illum_; }

			// CORREÇÃO: Criação segura de texturas com std::make_shared
			else if (prefix == "map_Kd") {
				std::string texture_path;
				iss >> texture_path;
				texture_.push_back(std::make_shared<Texture>(texture_path));
			}
			else if (prefix == "map_Ks") {
				std::string specular_map_path;
				iss >> specular_map_path;
				texture_.push_back(std::make_shared<Texture>(specular_map_path));
			}
			else if (prefix == "map_Ke") {
				std::string emissive_map_path;
				iss >> emissive_map_path;
				texture_.push_back(std::make_shared<Texture>(emissive_map_path));
			}
			else if (prefix == "map_Bump") {
				std::string normal_map_path;
				iss >> normal_map_path;
				texture_.push_back(std::make_shared<Texture>(normal_map_path));
			}
		}

		file.close();

		// CORREÇÃO: Usar a seta (->) para configurar a Unidade de Textura no ponteiro
		for (size_t i = 0; i < texture_.size(); ++i) {
			texture_[i]->set_texture_unit(static_cast<unsigned int>(i));
		}

		std::locale::global(old_locale);
		LOG("Material properties loaded.");
	}
}
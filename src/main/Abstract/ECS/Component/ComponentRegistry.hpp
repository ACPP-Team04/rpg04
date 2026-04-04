#pragma once

#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"

#include <any>
#include <functional>

#include <nlohmann/json.hpp>
#include <unordered_map>

using FunctionCreator = std::function<void(ArchetypeManager &, EntityID, nlohmann::json &)>;

class ComponentRegistry {

  public:
	~ComponentRegistry() = default;
	std::unordered_map<std::string, FunctionCreator> components;

	template <typename T>
	void registerComponent(const std::string &name)
	{
		components[name] = [](ArchetypeManager &mgr, EntityID id, nlohmann::json &data) {
			mgr.addComponentToEntity<T>(id);
			mgr.getComponent<T>(id).readFromJson(data);
		};
	}

	static ComponentRegistry &getInstance()
	{
		static ComponentRegistry instance;
		return instance;
	}

	FunctionCreator getCreationFunctions(const std::string &name)
	{
		if (components.contains(name)) {
			return components[name];
		}
		return {};
	}

  private:
	ComponentRegistry() = default;
};
#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "ParseContext.hpp"
#include "tileson.h"
#include <algorithm>
#include <functional>
#include <unordered_map>

using FunctionCreator = std::function<void(ArchetypeManager &, EntityID, tson::TiledClass &)>;
using DefaultComponentCreator = std::function<void(ArchetypeManager &, EntityID, tson::Object &)>;
class ComponentRegistry {

  public:
	~ComponentRegistry() = default;
	std::unordered_map<std::string, FunctionCreator> components;


	template <typename T>
	void registerComponent(const std::string &name)
	{
		components[name] = [](ArchetypeManager &mgr, EntityID id, tson::TiledClass &data) {
			mgr.addComponentToEntity<T>(id);
			mgr.getComponent<T>(id).readFromJson(data);
		};
	}
	template <typename T>
	void addDefaultComponent(ArchetypeManager &mgr, EntityID id, tson::Object &obj, ParseContext &context)
	{
		mgr.addComponentToEntity<T>(id);
		mgr.getComponent<T>(id).readFromObject(obj, context);
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
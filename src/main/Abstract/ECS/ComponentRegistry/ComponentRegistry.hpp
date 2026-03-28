#pragma once
#include "Abstract/ECS/Component.hpp"

#include <memory>
#include <set>
#include <typeinfo>
#include <unordered_map>

class ComponentRegistry {
	public:
		static int counter;

		static ComponentRegistry getInstance()
		{
			static ComponentRegistry instance;
			return instance;
		}
		template <typename T>
		void registerComponent()
		{
			auto t = typeid(T).hash_code();
			auto insertResult = componentRegister.insert({t,counter++});
			if (!insertResult.second) {
				throw std::runtime_error("Key already exists!");
			}
		};

		template <typename T>
		int getComponentId()
		{
			return componentRegister[typeid(T).hash_code()];
		}

	private:
		ComponentRegistry()= default;
		std::unordered_map<size_t,int> componentRegister;

};
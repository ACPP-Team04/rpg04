#pragma once
#include "Abstract/ECS/Component.hpp"

#include <iostream>
#include <memory>
#include <ostream>
#include <set>
#include <typeinfo>
#include <unordered_map>

class ComponentRegistry {
	public:
		inline static int counter = 0;

		static ComponentRegistry& getInstance()
		{
			static ComponentRegistry instance;
			return instance;
		}
		template <typename T>
		void registerComponent()
		{
			auto t = typeid(T).hash_code();
			std::cout << typeid(T).name() << "HashCOde" << t<< std::endl;
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
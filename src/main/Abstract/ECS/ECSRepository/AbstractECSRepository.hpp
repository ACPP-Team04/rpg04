#pragma once
#include "../Archetype/Archetype.hpp"
#include "Abstract/ECS/Component.hpp"
#include <future>
#include <unordered_map>
#include <vector>
// Positio, Velocit
ARchtyep{

	EntityID entity[];
	Position pos[x,y];
	Velocity velocity[x,y];
};
createAechtype<Vlocity,Position> -> EnitityId. Entiyid = 0.

class AbstractECSRepository {

	public:

	template<typename ...T>
	void createNewArchType()
	{



	};


	private:
	std::vector<std::shared_ptr<Archetype>> archTypes;
};




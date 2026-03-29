#pragma once
#include "Abstract/ECS/Component/ComponentTypeInfo.hpp"

struct ArchetypeBitSignature {
	bitset signature;
	ArchetypeBitSignature() : signature(0) {};
	ArchetypeBitSignature(bitset b):signature(b) {};

	friend bool operator==(const ArchetypeBitSignature& a,const ArchetypeBitSignature& b)
	{
		return a.signature == b.signature;
	}

	friend bool operator!=(const ArchetypeBitSignature a, const ArchetypeBitSignature& b)
	{
		return a.signature != b.signature;
	}

	template <typename ...U>
	static ArchetypeBitSignature get() {
		static bitset signature = (bitset(0) | ... | TypeInfo<U>::bitRepr());
		return {signature};
	}
	static ArchetypeBitSignature intersect(const ArchetypeBitSignature& a, const ArchetypeBitSignature& b) {
		return {a.signature & b.signature};
	}

};
namespace std {
template <>
struct hash<ArchetypeBitSignature> {
	size_t operator()(const ArchetypeBitSignature& sig) const
	{
		return hash<std::bitset<MAX_COMPONENTS>>{}(sig.signature);
	}
};
}
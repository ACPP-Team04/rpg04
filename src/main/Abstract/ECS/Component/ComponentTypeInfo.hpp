#pragma once
#include <bitset>
#define MAX_COMPONENTS 64
enum class BitwiseOperator {
	OR,
	AND
};
using bitset = std::bitset<MAX_COMPONENTS>;
struct TypeId {
	static int nextId()
	{
		static int nextId = 0;
		return nextId++;
	}
};

template <typename T>
struct TypeInfo {
	static int id() {
		static int id = TypeId::nextId();
		return id;
	}

	static bitset bitRepr() {
		bitset bits;
		bits.set(id());
		return bits;
	}
};
#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

struct IPool {
	IPool() = default;
	virtual ~IPool() = default;
	IPool(const IPool &other) = default;
	IPool(IPool &&other) = default;
	IPool &operator=(const IPool &other) = default;
	IPool &operator=(IPool &&other) = default;
	virtual size_t addEntity() = 0;
	virtual void removeLastEntity() = 0;
	virtual void moveFrom(size_t indexTo, size_t indexFrom) = 0;
	virtual void copyTo(size_t oldIndex, IPool *otherPool, size_t newIndex) = 0;
	virtual std::unique_ptr<IPool> createEmpty() = 0;
};

template <typename T>
struct ComponentPool : IPool {
	std::vector<T> components;

	size_t addEntity() override
	{
		this->components.emplace_back();
		return this->components.size() - 1;
	}

	void removeLastEntity() override { this->components.pop_back(); }

	T &getComponent(size_t location) { return this->components[location]; }
	T &getLastEntityComponent() { return this->components.back(); }
	T *getComponentArrayAsReference() { return this->components.data(); }
	void moveFrom(size_t indexTo, size_t indexFrom) override
	{
		this->components[indexTo] = std::move(this->components[indexFrom]);
	}

	void copyTo(size_t oldIndex, IPool *newPool, size_t newIndex) override
	{
		std::cout << oldIndex << " " << newIndex << std::endl;
		auto *newPoolCast = static_cast<ComponentPool<T> *>(newPool);
		newPoolCast->components[newIndex] = std::move(this->components[oldIndex]);
	}

	std::unique_ptr<IPool> createEmpty() override { return std::make_unique<ComponentPool<T>>(); }
};
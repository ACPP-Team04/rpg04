#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/Utils/Color.hpp"

struct DialogComponent : public Component<DialogComponent> {
	std::vector<std::string> sentences;
	sf::Color color;
	int characterSize;
	std::string currentSentence;
	int senetnceId;
	bool isActive;
	void readFromJson(const nlohmann::json &j) override
	{
		std::string raw = j.value("dialogSentences", "[]");
		auto parsed = nlohmann::json::parse(raw);
		for (const auto &sentence : parsed) {
			sentences.push_back(sentence.get<std::string>());
		}
		color = parseColorString(j.value("fillColor", "#f7f7f7"));
		characterSize = j.value("characterSize", 10);
		currentSentence = this->sentences.back();
	}

	std::string getSentence() { return currentSentence; }

	void nextSentence()
	{
		int newSentence = senetnceId % sentences.size();
		currentSentence = sentences[newSentence];
		senetnceId++;
	}
};

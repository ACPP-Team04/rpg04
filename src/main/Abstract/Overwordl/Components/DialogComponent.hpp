#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/Utils/Color.hpp"

#include <nlohmann/json.hpp>

struct DialogComponent : public Component<DialogComponent> {
	std::vector<std::string> sentences;
	sf::Color color;
	int characterSize;
	std::string currentSentence;
	int senetnceId;
	bool isActive;
	void readFromJson(tson::TiledClass &j) override
	{
		sentences.clear();

		if (auto prop = j.getMember("dialogSentences")) {
			std::string raw = prop->getValue<std::string>();

			if (!raw.empty()) {
				std::stringstream ss(raw);
				std::string segment;

				while (std::getline(ss, segment, '|')) {
					sentences.push_back(segment);
				}
			}
		}
		characterSize = j.getMember("characterSize")->getValue<int>();
		color = parseColorString(j.getMember("fillColor")->getValue<std::string>());

		if (!sentences.empty()) {
			currentSentence = sentences[0];
		} else {
			sentences.push_back("...");
			currentSentence = sentences[0];
		}
		senetnceId = 0;
	}

	std::string getSentence() { return currentSentence; }

	void nextSentence()
	{
		int newSentence = senetnceId % sentences.size();
		currentSentence = sentences[newSentence];
		senetnceId++;
	}
};

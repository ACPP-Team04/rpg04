#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Abstract/Utils/Color.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

struct DialogChoice;
template <typename T>
T getValueFromJson(const nlohmann::json &j, std::string key, T defaultValue, bool throwErrorWhenNotPresent = true)
{
	nlohmann::json target = j;
	if (j.is_object() && j.contains("value")) {
		target = j["value"];
	}

	if (!target.is_object() || !target.contains(key)) {
		if (throwErrorWhenNotPresent) {
			throw std::runtime_error("Key is not present in JSON: " + key);
		}
		return defaultValue;
	}

	return target.value(key, defaultValue);
}

struct DialogAction {
	DIALOG_ACTIONS action = DIALOG_ACTIONS::NO_ACTION;
	virtual ~DialogAction() = default;
	virtual void readFromNlohmannJson(const nlohmann::json &j) {}
};

struct SwitchLayerAction : public DialogAction {
	int destinationId = 0;
	SwitchLayerAction() { this->action = DIALOG_ACTIONS::SWITCH_LAYER_DIALOG_ACTION; }

	void readFromNlohmannJson(const nlohmann::json &j) override
	{
		destinationId = getValueFromJson(j, "switch_layer_id_destination_id", 0, true);
	}
};

struct GET_ITEM_ACTION : public DialogAction {
	int itemId = 0;
	GET_ITEM_ACTION() { this->action = DIALOG_ACTIONS::GET_ITEM; }

	void readFromNlohmannJson(const nlohmann::json &j) override
	{
		itemId = getValueFromJson(j, "get_item_id", 0, true);
	}
};

struct DialogChoice {
	std::string text;
	int nextNodeIndex = -1;
	std::shared_ptr<DialogAction> action;

	void readFromNlohmannJson(const nlohmann::json &j)
	{
		text = getValueFromJson(j, "text", text);
		auto isChoiceData = getValueFromJson(j, "isChoiceData", nlohmann::json());
		DIALOG_ACTIONS actionType = getValueFromJson(isChoiceData, "action", DIALOG_ACTIONS::NO_ACTION);
		nextNodeIndex = getValueFromJson(isChoiceData, "nextnodeId", -1, false);
		switch (actionType) {
		case DIALOG_ACTIONS::GET_ITEM:
			this->action = std::make_shared<GET_ITEM_ACTION>();
			break;
		case DIALOG_ACTIONS::SWITCH_LAYER_DIALOG_ACTION:
			this->action = std::make_shared<SwitchLayerAction>();
			break;
		default:
			this->action = std::make_shared<DialogAction>();
			break;
		}
		this->action->readFromNlohmannJson(isChoiceData);
	}
};

struct DialogNode {
	std::string text;
	int speakerId = 0;
	int nextNodeIndex = -1;
	std::vector<DialogChoice> choices;

	void readFromNlohmannJson(const nlohmann::json &j)
	{
		text = getValueFromJson(j, "text", text);
		auto isNodeData = getValueFromJson(j, "isNodeData", nlohmann::json());
		speakerId = getValueFromJson(isNodeData, "speaker", 0, true);
	}
};

struct DialogComponent : public Component<DialogComponent> {
	bool isActive = false;
	bool currentNodeSent = false;
	std::vector<DialogNode> nodes;
	int currentNodeIndex = 0;

	void readFromJson(tson::TiledClass &j) override {}

	DialogNode &current()
	{
		if (nodes.empty() || currentNodeIndex < 0) {
			throw std::runtime_error("No current node available");
		}
		return nodes[currentNodeIndex];
	}
	bool isPassedOnce() const
	{
		if (nodes.empty())
			return true;
		return currentNodeIndex < 0;
	}

	void advance(int choiceIndex = -1)
	{

		if (choiceIndex >= 0 && !current().choices.empty()) {
			int nextIdx = current().choices[choiceIndex].nextNodeIndex;
			currentNodeIndex = (nextIdx >= 0 && nextIdx < (int)nodes.size()) ? nextIdx : -1;
		} else {
			int nextIdx = current().nextNodeIndex;
			currentNodeIndex = (nextIdx >= 0 && nextIdx < (int)nodes.size()) ? nextIdx : -1;
		}
		currentNodeSent = false;
	}

	void readFromNlohmannJson(const nlohmann::json &j)
	{
		auto &dialogSequence = j["dialog_node"];
		if (!dialogSequence.is_array() || dialogSequence.empty()) {
			throw std::invalid_argument("dialog_node must be a non-empty array");
		}

		if (getTypeFromJson(dialogSequence.front()) == DIALOG_TYPE::CHOICE) {
			DialogNode fakeStart;
			fakeStart.text = "Select";
			fakeStart.nextNodeIndex = -1;
			nodes.push_back(fakeStart);
		}

		std::vector<std::pair<int, nlohmann::json>> pendingChoices;
		std::unordered_map<int, int> jsonIndexToNodeIndex;

		for (size_t i = 0; i < dialogSequence.size(); ++i) {
			auto &nodeJson = dialogSequence[i];
			DIALOG_TYPE type = getTypeFromJson(nodeJson);
			if (type == DIALOG_TYPE::NODE) {
				jsonIndexToNodeIndex[i] = (int)nodes.size();
				DialogNode node;
				node.readFromNlohmannJson(nodeJson);
				nodes.push_back(node);
			} else if (type == DIALOG_TYPE::CHOICE) {
				pendingChoices.emplace_back((int)nodes.size() - 1, nodeJson);
			}
		}
		for (auto &[nodeIdx, choiceJson] : pendingChoices) {
			if (nodeIdx >= 0 && nodeIdx < (int)nodes.size()) {
				DialogChoice choice;
				choice.readFromNlohmannJson(choiceJson);
				if (jsonIndexToNodeIndex.contains(choice.nextNodeIndex)) {
					choice.nextNodeIndex = jsonIndexToNodeIndex[choice.nextNodeIndex];
				} else {
					choice.nextNodeIndex = -1;
				}
				nodes[nodeIdx].choices.push_back(choice);
			}
		}
		for (size_t i = 0; i < nodes.size(); ++i) {
			if (nodes[i].choices.empty() && nodes[i].nextNodeIndex < 0) {
				nodes[i].nextNodeIndex = (i + 1 < nodes.size()) ? (int)(i + 1) : -1;
			}
		}


	}

	void reset()
	{
		this->currentNodeSent = false;
		this->isActive = true;
		this->currentNodeIndex = 0;
	}

	void stop()
	{
		this->isActive = false;
	}

	void start()
	{
		reset();
		this->isActive = true;
	}

	DIALOG_TYPE getTypeFromJson(const nlohmann::json &j) { return getValueFromJson(j, "dialogType", DIALOG_TYPE()); }
};

#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

struct DialogChoice;


struct DialogAction {
	DIALOG_ACTIONS action = DIALOG_ACTIONS::NO_ACTION;
	virtual ~DialogAction() = default;
	virtual void readFromJson(tson::TiledClass &j) {}
};

struct SwitchLayerAction : public DialogAction {
	int destinationId = 0;
	SwitchLayerAction() { this->action = DIALOG_ACTIONS::SWITCH_LAYER_DIALOG_ACTION; }

	void readFromJson(tson::TiledClass &j) override
	{
		destinationId = j.get<int>("switch_layer_id_destination_id");
	}
};

struct GET_ITEM_ACTION : public DialogAction {
	int itemId = 0;
	GET_ITEM_ACTION() { this->action = DIALOG_ACTIONS::GET_ITEM; }

	void readFromJson(tson::TiledClass &j)  override
	{
		itemId = j.get<int>("get_item_id");
	}
};

struct COMPANION_ACTION : public DialogAction {
	COMPANION_ACTION() { this->action = DIALOG_ACTIONS::COMPANION; }
	void readFromJson(tson::TiledClass &j) override {

	};
};

struct DialogChoice {
	std::string text;
	int nextNodeIndex = -1;
	std::shared_ptr<DialogAction> action;

	void readFromJson(tson::TiledClass &j)
	{
		text = j.get<std::string>("text");
		auto isChoiceData = j.get<tson::TiledClass>("isChoiceData");
		DIALOG_ACTIONS actionType = DIALOG_ACTIONS(isChoiceData.get<int>("action"));
		nextNodeIndex = isChoiceData.get<int>("nextnodeId");
		switch (actionType) {
		case DIALOG_ACTIONS::GET_ITEM:
			this->action = std::make_shared<GET_ITEM_ACTION>();
			break;
		case DIALOG_ACTIONS::SWITCH_LAYER_DIALOG_ACTION:
			this->action = std::make_shared<SwitchLayerAction>();
			break;
		case DIALOG_ACTIONS::COMPANION:
			this->action = std::make_shared<COMPANION_ACTION>();
			break;
		default:
			this->action = std::make_shared<DialogAction>();
			break;
		}
		this->action->readFromJson(isChoiceData);
	}
};

struct DialogNode {
	std::string text;
	int speakerId = 0;
	int nextNodeIndex = -1;
	std::vector<DialogChoice> choices;

	void readFromJson(tson::TiledClass &j)
	{
		text = j.get<std::string>("text");
		auto isNodeData = j.get<tson::TiledClass>("isNodeData");
		speakerId = isNodeData.get<int>("speaker");
	}
};

struct DialogComponent : public Component<DialogComponent> {
	bool isActive = false;
	bool currentNodeSent = false;
	std::vector<DialogNode> nodes;
	int currentNodeIndex = 0;

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

	void readFromJson(tson::TiledClass &j) override
	{
		tson::TiledClass dialogSequence = j.get<tson::TiledClass>("dialog_node");
		tson::TiledClass first = dialogSequence.get<tson::TiledClass>("0");
		DIALOG_TYPE firstType = DIALOG_TYPE(first.get<int>("dialogType"));
		if (firstType == DIALOG_TYPE::CHOICE) {
			DialogNode fakeStart;
			fakeStart.text = "Select";
			fakeStart.nextNodeIndex = -1;
			nodes.push_back(fakeStart);
		}

		std::vector<std::pair<int, tson::TiledClass>> pendingChoices;
		std::unordered_map<int, int> jsonIndexToNodeIndex;

		int i = 0;
		while (dialogSequence.getMember(std::to_string(i)) != nullptr) {
			tson::TiledClass nodeClass = dialogSequence.get<tson::TiledClass>(std::to_string(i));
			DIALOG_TYPE type = DIALOG_TYPE(nodeClass.get<int>("dialogType"));

			if (type == DIALOG_TYPE::NODE) {
				jsonIndexToNodeIndex[i] = (int)nodes.size();
				DialogNode node;
				node.readFromJson(nodeClass);
				nodes.push_back(node);
			} else if (type == DIALOG_TYPE::CHOICE) {
				pendingChoices.emplace_back((int)nodes.size() - 1, nodeClass);
			}
			i++;
		}
		for (auto &[nodeIdx, choiceClass] : pendingChoices) {
			if (nodeIdx < 0 || nodeIdx >= (int)nodes.size()) continue;
			DialogChoice choice;
			choice.readFromJson(choiceClass);
			if (jsonIndexToNodeIndex.contains(choice.nextNodeIndex)) {
				choice.nextNodeIndex = jsonIndexToNodeIndex[choice.nextNodeIndex];
			} else {
				choice.nextNodeIndex = -1;
			}
			nodes[nodeIdx].choices.push_back(choice);
		}

		for (size_t k = 0; k < nodes.size(); ++k) {
			if (nodes[k].choices.empty() && nodes[k].nextNodeIndex < 0) {
				nodes[k].nextNodeIndex = (k + 1 < nodes.size()) ? (int)(k + 1) : -1;
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
		this->currentNodeSent = false;
	}

	void start()
	{
		reset();
		this->isActive = true;
	}
};

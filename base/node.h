#pragma once

#include <stdint.h>
#include <thread>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace base {

class Node;
struct Group;
typedef uint32_t node_id_t;
typedef std::function<void(Node* node)> RunFunc_t;
typedef std::function<bool()> PredictFunc_t;

struct Message {
	Message() {}
	virtual ~Message() {}
	virtual uint32_t uri() = 0;

	node_id_t from_node;
};

class Node {
public:
	Node(node_id_t id, Group *group);
	virtual ~Node();

public:
	void Handle(const std::unique_ptr<Message>& msg);
	void HandleWait(PredictFunc_t func);
	bool SendMessage(node_id_t id, std::unique_ptr<Message>&& msg);
	bool RecvMessage(std::unique_ptr<Message>&& msg);
	void Start(RunFunc_t func);
	void Join();
	node_id_t id() const {
		return id_;
	}

protected:
	std::map<uint32_t, 

private:
	node_id_t                             id_;
	Group*                                group_;
	std::thread                           thread_;
	std::queue<std::unique_ptr<Message>>  msgs_;
	std::mutex                            mutex_;
	std::condition_variable               cond_;
};

struct Group {
	template<typename NODE>
	static bool MakeGroup(uint32_t node_count,
						  RunFunc_t func,
						  std::unique_ptr<Group>& group) {
		std::unique_ptr<Group> tmp_group(new Group);
		if (!tmp_group) {
			return false; 
		}
		for (node_id_t id=0; id<node_count; id++) {
			tmp_group->nodes.emplace_back(new NODE(id, tmp_group.get()));
		}
		for (auto& node : tmp_group->nodes) {
			node->Start(func);
		}
		group = std::move(tmp_group);

		return true;
	}

	void Join();

	std::vector<std::unique_ptr<Node>> nodes;
};

}


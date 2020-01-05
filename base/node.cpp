#include "node.h"

namespace base {

Node::Node(node_id_t id, Group* group)
	: id_(id), group_(group) {
}

Node::~Node() {
	Join();
}

bool Node::SendMessage(node_id_t id, std::unique_ptr<Message>&& msg) {
	return group_->nodes[id]->RecvMessage(std::move(msg));
}

bool Node::RecvMessage(std::unique_ptr<Message>&& msg) {
	std::unique_lock<std::mutex> lock(mutex_);
	msgs_.push(std::move(msg));
	cond_.notify_one();
	return true;
}

void Node::Start(RunFunc_t func) {
	thread_ = std::thread(func, this);
}

void Node::Join() {
	thread_.join();
}

void Node::HandleWait(PredictFunc_t func) {
	for (;;) {
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cond_.wait(lock, [this] { return !msgs_.empty(); });
		}

		do {
			std::unique_ptr<Message> msg;
			if (!msgs_.empty()) {
				std::unique_lock<std::mutex> lock(mutex_);
				msg = std::move(msgs_.front());
				msgs_.pop();
			}
			if (msg) {
				Handle(msg);
			}
			if (func != nullptr && func()) {
				return;
			}
		} while(!msgs_.empty());
	}
}

void Group::Join() {
	for (auto& node : nodes) {
		node->Join();
	}
}

}

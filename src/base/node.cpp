#include "node.h"

namespace base {

Node::~Node() {
	Join();
}

bool Node::SendMessage(node_ip_t ip, std::unique_ptr<Message>&& msg) {
	msg->from_ip = ip_;
	{
		auto& node = group_->nodes[ip];
		std::unique_lock<std::mutex> lock(node->mutex_);
		node->msgs_.push(std::move(msg));
		node->cond_.notify_one();
	}
	return true;
}

void Node::HandleMessage(const std::unique_ptr<Message>& msg) {
	auto handler = handlers_[msg->uri()];
	assert(handler != nullptr);
	(this->*handler)(msg.get());
}

void Node::Start() {
	thread_ = std::move(std::thread(&Node::Run, this));
}

void Node::Stop() {
	std::unique_lock<std::mutex> lock(mutex_);
	stop_ = true;
	cond_.notify_one();
}

void Node::Join() {
	if (thread_.joinable()) {
		thread_.join();
	}
}

void Node::SetDone() {
	std::unique_lock<std::mutex> lock(group_->mutex);
	done_ = true;
	group_->cond.notify_one();
}

void Node::WaitFor(PredictFunc_t func) {
	auto is_end = [this, func]() -> bool {
		return stop_ || (func != nullptr && func());
	};

	for (;!is_end();) {
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cond_.wait(lock, [this] { return !msgs_.empty() || stop_; });
		}

		do {
			std::unique_ptr<Message> msg;
			if (!msgs_.empty()) {
				std::unique_lock<std::mutex> lock(mutex_);
				msg = std::move(msgs_.front());
				msgs_.pop();
			}
			if (msg) {
				HandleMessage(msg);
			}
		} while(!msgs_.empty() && !is_end());
	}
}

void Group::RunUntilDone() {
	Start();
	WaitDone();
	Stop();
	Join();
}

void Group::Start() {
	for (const auto& node : nodes) {
		node.second->Start();
	}
}

void Group::Stop() {
	for (const auto& node : nodes) {
		node.second->Stop();
	}
}

void Group::Join() {
	for (const auto& node : nodes) {
		node.second->Join();
	}
}

void Group::WaitDone() {
	std::unique_lock<std::mutex> lock(mutex);
	cond.wait(lock, [this] {
		for (const auto& node : nodes) {
			if (!node.second->is_done()) {
				return false;
			}
		}
		return true;
	});
}

}


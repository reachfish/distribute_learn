#pragma once

#include <stdint.h>
#include <assert.h>
#include <thread>
#include <queue>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>

#define REGISTER_HANDLER(uri, handler)                           \
	assert(base::Node::handlers_.count(uri) == 0);               \
    base::Node::handlers_[uri] = (base::Node::Handler_t)handler;

namespace base {

class Node;
struct Group;
typedef uint32_t node_id_t;
typedef uint64_t node_ip_t;
typedef std::string uri_t;
typedef std::function<bool()> PredictFunc_t;

const node_id_t INVALID_NODE_ID = (node_id_t)-1;
const node_ip_t INVALID_NODE_IP = (node_ip_t)-1;

struct Message {
	Message() {}
	virtual ~Message() {}
	virtual uri_t uri() = 0;

	node_ip_t from_ip;
};

class Node {
public:
	virtual ~Node();

public:
	virtual void Run() = 0;
	void HandleMessage(const std::unique_ptr<Message>& msg);
	bool SendMessage(node_ip_t id, std::unique_ptr<Message>&& msg);
	void WaitFor(PredictFunc_t func);
	void Start();
	void Stop();
	void Join();
	void SetDone();
	node_ip_t ip() const { return ip_; }
	node_id_t id() const { return id_; }
	bool is_done() const { return done_; }

protected:
	friend  class Group;
	node_ip_t                             ip_{INVALID_NODE_IP};
	node_id_t                             id_{INVALID_NODE_ID};
	Group*                                group_{nullptr};
	bool                                  done_{false};
	bool                                  stop_{false};
	std::thread                           thread_;
	std::queue<std::unique_ptr<Message>>  msgs_;
	std::mutex                            mutex_;
	std::condition_variable               cond_;
	typedef void (Node::*Handler_t)(const Message* msg);
	std::map<uri_t, Handler_t>            handlers_;
};

struct Group {
	template<typename NODE, typename... Args>
	static std::unique_ptr<Group> MakeGroup(uint32_t node_count, Args &&... args) {
		std::unique_ptr<Group> group(new Group);
		if (!group) {
			return nullptr; 
		}
		node_ip_t ip = 1;
		node_id_t id = 1;
		for (uint32_t i=0; i<node_count; i++, ip++, id++) {
			NODE* node = new NODE(std::forward<Args>(args)...);
			node->ip_ = ip;
			node->id_ = id;
			node->group_ = group.get();
			group->nodes.insert(std::make_pair(ip, std::unique_ptr<Node>(node)));
		}

		return group;
	}

	void RunUntilDone();
	void Start();
	void Stop();
	void Join();
	void WaitDone();

	std::map<node_ip_t, std::unique_ptr<Node>> nodes;
	std::mutex                                 mutex;
	std::condition_variable                    cond;
};

}


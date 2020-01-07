#include <iostream>
#include <set>
#include "vc_node.h"

namespace vertex_color {

VCNode::VCNode() {
	REGISTER_HANDLER(GetId().uri(), &VCNode::OnGetId);
	REGISTER_HANDLER(GetIdRes().uri(), &VCNode::OnGetIdRes);
	REGISTER_HANDLER(GetColorRes().uri(), &VCNode::OnGetColorRes);
}

void VCNode::InitNeigh(const std::vector<Edge>& edges) {
	for (const auto& edge : edges) {
		node_ip_t neigh_ip = base::INVALID_NODE_IP;
		if (edge.ip1 == ip_) {
			neigh_ip = edge.ip2;
		}
		if (edge.ip2 == ip_) {
			neigh_ip = edge.ip1;
		}
		if (neigh_ip != base::INVALID_NODE_IP) {
			neigh_id_[neigh_ip] = base::INVALID_NODE_ID;
			neigh_color_[neigh_ip] = INVALID_COLOR;
		}
	}
}

void VCNode::Run() {
	//get neighbor id
	for (const auto& neigh : neigh_id_) {
		node_ip_t ip = neigh.first;
		GetId* request = new GetId;
		SendMessage(ip, std::unique_ptr<base::Message>(request));
	}

	//recv neighbor id
	WaitFor([this]()-> bool {
		for (const auto& neigh : neigh_id_) {
			if (neigh.second == base::INVALID_NODE_ID) {
				return false;
			}
		}
		return true;
	});

	//wait neighbor color who's id is bigger then me
	WaitFor([this]()-> bool {
		for (const auto& neigh : neigh_id_) {
			node_ip_t ip = neigh.first;
			node_id_t id = neigh.second;
			if (id > id_ && 
				neigh_color_[ip] == INVALID_COLOR) {
				return false;
			}
		}
		return true;
	});

	//choose my color
	std::set<int32_t> colors;
	for (const auto& neigh : neigh_color_) {
		int32_t color = neigh.second;
		colors.insert(color);
	}
	for (int32_t color=1;; color++) {
		if (!colors.count(color)) {
			color_ = color;
			break;
		}
	}

	//brocast my color
	for (const auto& neigh : neigh_id_) {
		GetColorRes* request = new GetColorRes;
		request->ip = ip_;
		request->color = color_;
		SendMessage(neigh.first, std::unique_ptr<base::Message>(request));
	}

	SetDone();

	//loop process request
	WaitFor(nullptr);
}

void VCNode::OnGetId(const GetId* request) {
	GetIdRes* response = new GetIdRes;
	response->ip = ip_;
	response->id = id_;
	SendMessage(request->from_ip, std::unique_ptr<base::Message>(response));
}

void VCNode::OnGetIdRes(const GetIdRes* response) {
	if (neigh_id_.count(response->ip)) {
		neigh_id_[response->ip] = response->id;
	}
}

void VCNode::OnGetColorRes(const GetColorRes* response) {
	assert(neigh_color_.count(response->ip));
	assert(response->color != INVALID_COLOR);
	if (neigh_color_.count(response->ip)) {
		neigh_color_[response->ip] = response->color;
	}
}

}


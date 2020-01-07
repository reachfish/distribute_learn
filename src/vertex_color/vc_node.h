#pragma once

#include "base/node.h"

namespace vertex_color {

using base::uri_t;
using base::node_ip_t;
using base::node_id_t;

const int32_t INVALID_COLOR = -1;

struct GetId : public base::Message {
	virtual uri_t uri() override {
		return "GetId";
	}
};

struct GetIdRes : public base::Message {
	virtual uri_t uri() override {
		return "GetIdRes";
	}

	node_ip_t  ip;
	node_id_t  id;
};

struct GetColor : public base::Message {
	virtual uri_t uri() override {
		return "GetColor";
	}

	node_ip_t  ip;
};

struct GetColorRes : public base::Message {
	virtual uri_t uri() override {
		return "GetColorRes";
	}

	node_ip_t   ip;
	int32_t     color;
};

struct Edge {
	node_ip_t   ip1;
	node_ip_t   ip2;
};

class VCNode : public base::Node {
public:
	VCNode();

public:
	virtual void Run() override;
	void InitNeigh(const std::vector<Edge>& edges);
	int32_t color() const {
		return color_;
	}

private:
	void OnGetId(const GetId* request); 
	void OnGetIdRes(const GetIdRes* response); 
	void OnGetColorRes(const GetColorRes* response);

private:
	int32_t                         color_{INVALID_COLOR};
	std::map<node_ip_t, int32_t>    neigh_color_;
	std::map<node_ip_t, node_id_t>  neigh_id_;
};

}


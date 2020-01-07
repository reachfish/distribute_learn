#include <map>
#include <gtest/gtest.h>
#include "vertex_color/vc_node.h"

TEST(TestVC, TestStar) {
	std::vector<vertex_color::Edge> edges{
		{3, 1},
		{3, 2},
		{3, 4},
		{3, 5}
	};
	auto group = base::Group::MakeGroup<vertex_color::VCNode>(5);
	for (const auto& node : group->nodes) {
		((vertex_color::VCNode*)node.second.get())->InitNeigh(edges);
	}
	group->Start();
	group->WaitDone();
	std::map<base::node_ip_t, int32_t> colors;
	for (const auto& node : group->nodes) {
		vertex_color::VCNode* vcnode = (vertex_color::VCNode*)node.second.get();
		std::cout << "ip=" << vcnode->ip() << ", color=" << vcnode->color() << std::endl;
		colors[vcnode->ip()] = vcnode->color();
	}

	group->Stop();
	group->Join();

	for (base::node_ip_t ip=1; ip<=5; ip++) {
		if (ip == 3) {
			EXPECT_NE(colors[ip], colors[1]);
		} else {
			EXPECT_EQ(colors[ip], colors[1]);
		}
	}
}


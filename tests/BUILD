COPTS = [
	"-std=c++11",
	"-g",
]

cc_test(
	name = "vertex_color",
	srcs = glob([
		"test_vertex_color.cpp",
	]),
	deps = [
		"//src:vertex_color",
		"@gtest//:gtest_main",
	],
	copts = COPTS,
)


cc_library(
	name = "base",
	srcs = glob([
		"base/*.cpp",
	]),
	hdrs = glob([
		"base/*.h",
	]),
	linkopts = [
		"-lpthread",
	],
	copts = [
		"-std=c++11",
		"-g",
	],
	visibility = ["//visibility:public"],
)

cc_library(
	name = "vertex_color",
	srcs = glob([
		"vertex_color/*.cpp",
	]),
	hdrs = glob([
		"vertex_color/*.h",
	]),
	deps = [
		":base",
	],
	copts = [
		"-std=c++11",
		"-g",
	],
	includes = [
		".",
	],
	visibility = ["//visibility:public"],
)


COPTS = [
	"-std=c++11",
	"-g",
]

VISIBILITY = [
	"//visibility:public",
]

INCLUDES = [
	".",
]

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
	copts = COPTS,
	visibility = VISIBILITY,
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
	copts = COPTS,
	includes = INCLUDES,
	visibility = VISIBILITY,
)


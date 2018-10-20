cc_library(
	name = "nqueens_lib",
	hdrs = ["nqueens.h"],
	visibility = ["//visibility:public"],
	deps = [
	     "//:dlx",
	],
)

cc_library(
	name = "sudoku_lib",
	hdrs = ["sudoku.h"],
	visibility = ["//visibility:public"],
	deps = [
	     "//:dlx",
	],
)

cc_binary(
	name = "nqueens",
	srcs = ["nqueens.cc"],
	deps = [
	     "//:dlx",
	     ":nqueens_lib",
	],
)

cc_binary(
	name = "sudoku",
	srcs = ["sudoku.cc"],
	deps = [
	     "//:dlx",
	     ":sudoku_lib",
	],
	data = [
	     "//data:sudoku"
	],
)

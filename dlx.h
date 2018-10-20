#pragma once

// The Dancing Links Class: An exercise in modern C++ design.
//
// - A highly configurable and efficient implementation of Algorithm X
//   (see https://arxiv.org/pdf/cs/0011047.pdf) that solves the Exact
//   Cover problem. An instance of this problem is simply a matrix of
//   1s and 0s, and the goal is to find a subset of rows that sum up
//   to the all 1s vector. This class implements routines to solve
//   this problem both recursively (standard) and iteratively (when
//   one is concerned about overflowing the stack at runtime).
//
// Configuration options:
//
// - A basic convenience in this implementation is the ability to
//   specify primary and secondary columns (notation from the original
//   paper), where primary columns are required to be covered whereas
//   secondary columns may or may not be covered with 1s in a valid
//   solution. W.L.O.G, we assume input instances have all their
//   primary columns grouped before all their seondary columns so that
//   it suffices to specify the (zero-based) column index of the first
//   secondary column to infer the set of secondary columns.
//
// - The input instance (0/1 Matrix along with the index of the first
//   secondary column) is injected through a *MatrixInterface* object.
//
// - By specifying different *VisitorInterface* objects to the
//   solution methods (RSolve/ISolve), it is possible to print some/all
//   of the solutions, gather statistics on the solutions, etc.
//
// - The policy class *ColumnPickingPolicy* allows different column
//   picking heuristics to be baked in at compile time. The default
//   policy works well (fast), but you may pick another policy like
//   FirstAvailableColumn if you are interested in a specific ordering
//   of the solutions.

#include "include/dlx_internal.h"
#include "include/matrix.h"
#include "include/policies.h"
#include "include/visitor.h"

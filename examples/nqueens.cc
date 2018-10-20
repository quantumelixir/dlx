#include "examples/nqueens.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
  std::ios_base::sync_with_stdio(false);
  int n = (argc > 1) ? atoi(argv[1]) : 8;
  int k = (argc > 2) ? atoi(argv[2]) : 1;
  NQueens<dlx::UniformlyRandomColumn> nqueens{n};
  NQueensVisitor visitor{n, k}; // Only print the first k solutions.
  nqueens.Solve(visitor, dlx::SolutionMethod::ITERATIVE);
  return 0;
}

#include <cassert>
#include <iostream>

#include "dlx.h"
#include "examples/sudoku.h"

void TEST_secondary_columns() {
  std::vector<std::vector<int>> mat(4, std::vector<int>(5, 1));
  mat[2][3] = 0;
  mat[3][4] = 0;
  dlx::MatrixFromVector<int> mat_view(mat, 4);
  dlx::DancingLinks<dlx::FirstAvailableColumn> dlx{mat_view};
  std::vector<std::vector<int>> solns, ans;
  for (int row_idx : std::vector<int>{0, 1, 3})
    ans.push_back(std::vector<int>{row_idx});
  dlx::SavingVisitor visitor{&solns};
  dlx.Solve(visitor, dlx::SolutionMethod::ITERATIVE);
  assert(ans == solns);
  std::cout << "PASSED: TEST_secondary_columns." << std::endl;
};

void TEST_sudoku_2x2() {
  Sudoku<dlx::ColumnWithLeastOnes> sudoku{2};
  assert(sudoku.MoreThanOneSolution());
  assert(288 == sudoku.Count());
  std::cout << "PASSED: TEST_sudoku_2x2." << std::endl;
}

void TEST_sudoku_3x3() {
  // Sourced from http://norvig.com/sudoku.html
  std::string non_unique(".....6....59.....82....8....45........3........6..3."
                         "54...325..6.................."),
      impossible(".....5.8....6.1.43..........1.5........1.6...3.......553....."
                 "61........4.........");
  Sudoku<dlx::ColumnWithLeastOnes> sudoku{3};
  sudoku.SetProblem(non_unique);
  assert(sudoku.MoreThanOneSolution());
  sudoku.SetProblem(impossible);
  assert(0 == sudoku.Count());
  std::cout << "PASSED: TEST_sudoku_3x3." << std::endl;
}

int main() {
  TEST_secondary_columns();
  TEST_sudoku_2x2();
  TEST_sudoku_3x3();
  return 0;
}

#include "examples/sudoku.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> ReadLines(const std::string &filename) {
  std::vector<std::string> lines;
  char line[256];
  std::ifstream input;
  input.open(filename, std::ifstream::in);
  while (input.getline(line, 256)) {
    lines.push_back(line);
  }
  input.close();
  return lines;
}

int main(int argc, char **argv) {
  Sudoku<dlx::ColumnWithLeastOnes> sudoku{3};
  SudokuVisitor visitor{3, SudokuFormat::ONELINE, 1};
  const std::vector<std::string> &input{ReadLines("data/sudoku.in.txt")};
  for (const auto &line : input) {
    if (!sudoku.SetProblem(line))
      continue;
    sudoku.Solve(visitor, dlx::SolutionMethod::RECURSIVE);
  }
  return 0;
}

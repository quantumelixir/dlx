#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "dlx.h"

// n = 3 for the regular sudoku.
class SudokuMatrix : public dlx::MatrixInterface {
public:
  SudokuMatrix(int n = 3) : n_(n) {}
  void SetN(int n) { n_ = n; }
  // One row for each choice to be made.
  int Rows() override { return (n_ * n_) * (n_ * n_) * (n_ * n_); }
  // Four sets of columns for each type of constraint, along with the
  // problem specific constraints.
  int Cols() override { return 4 * (n_ * n_) * (n_ * n_) + row_idxs_.size(); }
  int Value(int i, int j) override {
    const int width = n_ * n_;
    int l = i % width, x = (i / width) % width, y = (i / width) / width;
    int b = (x / n_) * n_ + (y / n_);
    int ox = (j % (width * width)) / width, oy = (j % (width * width)) % width;
    switch (j / (width * width)) {
    case 0: // Cell
      return (x == ox && y == oy);
    case 1: // Row
      return (l == ox && x == oy);
    case 2: // Column
      return (l == ox && y == oy);
    case 3: // Box
      return (l == ox && b == oy);
    default: // Problem constraints
      int idx = j - 4 * (n_ * n_) * (n_ * n_);
      assert(0 <= idx && idx < row_idxs_.size());
      return i == row_idxs_[idx];
    }
    assert(false);
    return -1;
  }
  int FirstSecondaryColumnIndex() override { return Cols(); }

  // Force particular rows in the solution.
  void Preselect(int row_idx) { row_idxs_.push_back(row_idx); }
  void UndoAllPreselects() { row_idxs_.resize(0); }

private:
  int n_;
  std::vector<int> row_idxs_;
};

enum class SudokuFormat { ONELINE, MULTILINE };

// Decides the size, output format and visiting policy.
class SudokuVisitor : public dlx::VisitorInterface {
public:
  SudokuVisitor(int n = 3, SudokuFormat fmt = SudokuFormat::MULTILINE,
                int to_visit = 0)
      : n_(n), fmt_(fmt), to_visit_(to_visit), visited_(0) {}
  void SetN(int n) { n_ = n; }
  void SetFormat(SudokuFormat fmt) { fmt_ = fmt; }
  bool VisitSolution(const std::vector<int> &chosen) override {
    const int width = n_ * n_;
    static std::vector<std::vector<int>> board(width, std::vector<int>(width));
    for (auto row_idx : chosen) {
      int l = row_idx % width, x = (row_idx / width) % width,
          y = (row_idx / width) / width;
      board[x][y] = l + 1;
    }
    for (int i = 0; i < width; i++)
      for (int j = 0; j < width; j++)
        std::cout << board[i][j]
                  << (j == width - 1 && fmt_ == SudokuFormat::MULTILINE ? "\n"
                                                                        : "");
    std::cout << "\n";
    return to_visit_ != ++visited_;
  }

private:
  int n_;
  SudokuFormat fmt_;
  int to_visit_, visited_;
};

template <class ColumnPickingPolicy> class Sudoku {
public:
  Sudoku(int n = 3) : n_(n) {
    matrix_.SetN(n_);
    dlx_.Initialize(matrix_);
  }
  // Return value indicates if the problem was correctly parsed.
  bool SetProblem(const std::string &problem) {
    matrix_.UndoAllPreselects();
    const int width = n_ * n_;
    auto valid_character = [](char c) {
      return c == '.' || ('0' <= c && c <= '9');
    };
    if (std::count_if(problem.begin(), problem.end(), valid_character) !=
        width * width)
      return false;
    int pos = 0;
    for (int k = 0; pos != width * width; k++) {
      char c = problem[k];
      if (valid_character(c)) {
        if (c != '.' && c != '0') {
          int i = pos / width, j = pos % width;
          matrix_.Preselect(j * width * width + i * width + (c - '1'));
        }
        pos++;
      }
    }
    dlx_.Initialize(matrix_);
    return true;
  }
  void SetN(int n) {
    n_ = n;
    matrix_.SetN(n_);
    dlx_.Initialize(matrix_);
  }
  void Solve(SudokuVisitor &visitor,
             dlx::SolutionMethod method = dlx::SolutionMethod::ITERATIVE) {
    dlx_.Solve(visitor, method);
  }
  unsigned int
  Count(dlx::SolutionMethod method = dlx::SolutionMethod::ITERATIVE) {
    dlx::CountingVisitor<unsigned int> visitor;
    dlx_.Solve(visitor, method);
    return visitor.Count();
  }
  bool MoreThanOneSolution(
      dlx::SolutionMethod method = dlx::SolutionMethod::ITERATIVE) {
    dlx::UniquenessTestingVisitor visitor;
    dlx_.Solve(visitor, method);
    return visitor.MoreThanOneSolution();
  }

private:
  int n_;
  SudokuMatrix matrix_;
  dlx::DancingLinks<ColumnPickingPolicy> dlx_;
};

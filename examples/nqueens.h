#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "dlx.h"

class NQueensMatrix : public dlx::MatrixInterface {
public:
  NQueensMatrix(int q = 4) : n_(q) {}
  void SetN(int q) { n_ = q; }
  int Rows() override { return n_ * n_; }
  int Cols() override { return 2 * n_ + (2 * n_ - 1) * 2; }
  // n^2 rows, 2*n + (2*n-1)*2 columns
  int Value(int i, int j) override {
    int x = i / n_, y = i % n_;
    return (j == x || j == n_ + y || j == 2 * n_ + x + y ||
            j == 4 * n_ - 1 + y - x + n_);
  }
  int FirstSecondaryColumnIndex() override { return 2 * n_; }

private:
  int n_;
};

class NQueensVisitor : public dlx::VisitorInterface {
public:
  NQueensVisitor(int q = 4, int to_visit = 0) : n_(q), to_visit_(to_visit) {
    visited_ = 0;
  }
  void SetN(int q) { n_ = q; }
  bool VisitSolution(const std::vector<int> &chosen) override {
    static std::vector<std::vector<char>> board(n_, std::vector<char>(n_));
    std::fill(board.begin(), board.end(), std::vector<char>(n_, '.'));
    for (auto row_idx : chosen)
      board[row_idx / n_][row_idx % n_] = 'Q';
    for (int i = 0; i < n_; i++)
      for (int j = 0; j < n_; j++)
        std::cout << board[i][j] << (j == n_ - 1 ? "\n" : "");
    std::cout << "\n";
    return to_visit_ != ++visited_;
  }

private:
  int n_;
  int to_visit_, visited_;
};

template <class ColumnPickingPolicy> class NQueens {
public:
  NQueens(int n = 4) : n_(n) {
    matrix_.SetN(n_);
    dlx_.Initialize(matrix_);
  }
  void SetN(int q) {
    n_ = q;
    matrix_.SetN(n_);
    dlx_.Initialize(matrix_);
  }
  void Solve(NQueensVisitor &visitor, dlx::SolutionMethod method) {
    dlx_.Solve(visitor, method);
  }
  unsigned int
  Count(dlx::SolutionMethod method = dlx::SolutionMethod::ITERATIVE) {
    dlx::CountingVisitor<unsigned int> visitor;
    dlx_.Solve(visitor, method);
    return visitor.Count();
  }

private:
  int n_;
  NQueensMatrix matrix_;
  dlx::DancingLinks<ColumnPickingPolicy> dlx_;
};

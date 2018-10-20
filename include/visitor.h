#pragma once

#include <iostream>
#include <vector>

namespace dlx {

// Interface class for visiting solutions (a subset of row
// indices). Return value dictates whether to continue the
// backtracking search.
class VisitorInterface {
public:
  virtual bool VisitSolution(const std::vector<int> &solution) = 0;
};

// Default implementation prints solution to stdout.
template <bool visit_all> class DefaultVisitor : public VisitorInterface {
public:
  bool VisitSolution(const std::vector<int> &chosen) override {
    std::cout << "[";
    for (int i = 0; i < chosen.size(); i++)
      std::cout << chosen[i] << (i != chosen.size() - 1 ? ", " : "");
    std::cout << "]";
    return visit_all;
  }
};

// Saves the solutions to a vector that is not owned.
class SavingVisitor : public VisitorInterface {
public:
  SavingVisitor() = delete;
  explicit SavingVisitor(std::vector<std::vector<int>> *ptr) : ptr_(ptr) {}
  bool VisitSolution(const std::vector<int> &chosen) override {
    ptr_->push_back(chosen);
    return true;
  }

private:
  std::vector<std::vector<int>> *ptr_;
};

// Just counts solutions.
template <class T> class CountingVisitor : public VisitorInterface {
public:
  CountingVisitor() : count_(0) {}
  bool VisitSolution(const std::vector<int> &chosen) override {
    count_++;
    return true;
  }
  T Count() const { return count_; }

private:
  T count_;
};

// Test uniqueness.
class UniquenessTestingVisitor : public VisitorInterface {
public:
  UniquenessTestingVisitor() : count_(0) {}
  bool VisitSolution(const std::vector<int> &chosen) override {
    count_++;
    return count_ < 2;
  }
  bool MoreThanOneSolution() { return count_ >= 2; }

private:
  int count_;
};

} // namespace dlx

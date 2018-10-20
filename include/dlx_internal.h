#pragma once

#include "cell.h"
#include "matrix.h"
#include "policies.h"
#include "visitor.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <stack>
#include <thread>
#include <tuple>
#include <vector>

namespace dlx {

enum class SolutionMethod { RECURSIVE, ITERATIVE };

template <class ColumnPickingPolicy = ColumnWithLeastOnes>
class DancingLinks : public ColumnPickingPolicy {
  // Make all policies friends of this class to avoid bidirectional
  // coupling syntactically (semantically it's unavoidable).
  friend ColumnPickingPolicy;

public:
  // Default construction creates a trivial instance.
  DancingLinks() { Reset(); }

  // Setup the internal data structures to solve the input instance.
  DancingLinks(MatrixInterface &matrix) { Initialize(matrix); }

  // Initialize the cells according to the initialization policy.
  void Initialize(MatrixInterface &matrix) {
    Reset(); // Ensure we are in the default constructed state
             // before initialization.
    nrows_ = matrix.Rows();
    ncols_ = matrix.Cols();
    O_.resize(ncols_ + 1);

    // Insert all the headers.
    for (int j = 0; j < ncols_; j++) {
      C_.emplace_back(Cell{});
      auto &c = C_.back();
      c.l = j;
      c.r = 0;
      C_[c.l].r = C_[c.r].l = j + 1;
      c.u = c.d = c.h = j + 1;
      c.row_idx = -1;
    }

    // Keep track of the arena index of the bottommost cell in each column.
    std::vector<int> U;
    U.resize(ncols_);
    for (int j = 0; j < ncols_; j++) {
      U[j] = AIdx(j);
    }

    // Insert the cells
    for (int i = 0; i < nrows_; i++) {
      int left = -1, right = -1;
      for (int j = 0; j < ncols_; j++) {
        if (matrix.Value(i, j) == 1) {
          O_[AIdx(j)]++; // Increment the number of ones in the j-th column.
          int idx = C_.size();
          C_.emplace_back(Cell{});
          auto &c = C_.back();
          c.h = AIdx(j);
          c.row_idx = i;
          if (left == -1) { // First in the row!
            c.l = c.r = idx;
            c.u = U[j];
            c.d = AIdx(j);
            C_[U[j]].d = C_[AIdx(j)].u = idx;
          } else {
            c.l = left;
            c.r = right;
            C_[left].r = C_[right].l = idx;
            c.u = U[j];
            c.d = AIdx(j);
            C_[U[j]].d = idx;
            C_[AIdx(j)].u = idx;
          }
          U[j] = left = idx;
          right = (right == -1) ? idx : right;
        }
      }
    }

    // Save the arena index of the first secondary column.
    sec_idx_ = AIdx(matrix.FirstSecondaryColumnIndex());
  }

  // Print the active state of the board. Each active cell is
  // identified by its row and column index in the input matrix.
  void PrintBoard() const {
    std::cout << "The active board is " << nrows_ << " x " << ncols_
              << " in size.\n";
    for (int hdr_idx = C_[0].r; hdr_idx != 0; hdr_idx = C_[hdr_idx].r) {
      for (int cell_idx = C_[hdr_idx].d; C_[cell_idx].row_idx != -1;
           cell_idx = C_[cell_idx].d) {
        std::cout << "Cell (" << C_[cell_idx].row_idx << ", " << CIdx(hdr_idx)
                  << ") at index " << cell_idx << ".\n";
      }
      std::cout << "Column " << CIdx(hdr_idx) << " contains " << O_[hdr_idx]
                << " ones.\n";
    }
  }

  // Convenience wrapper.
  void Solve(VisitorInterface &visitor,
             SolutionMethod method = SolutionMethod::ITERATIVE) {
    (method == SolutionMethod::RECURSIVE) ? RSolve(visitor) : ISolve(visitor);
  }

  /////////////////////
  // Private methods //
  /////////////////////
private:
  // Solve iteratively. The supplied `visitor` allows the backtracking
  // to end prematurely (before visiting all solutions). Even in the
  // case of a premature exit, the internal state after the call is
  // left identical to the one before the call to ISolve(..) relieving
  // callers of any kind of bookkeeping.
  void ISolve(VisitorInterface &visitor) {
    // Stores the state at which context on the stack should be
    // interpreted.
    enum class State {
      FIND_COLUMN,
      UNCOVER_COLUMN,
      CHOOSE_ROW,
      UNCHOOSE_ROW,
    };

    // Store the hdr_idx, c1_idx pair along with the state.
    std::stack<std::tuple<int, int, State>> s{{{-1, -1, State::FIND_COLUMN}}};
    std::vector<int> chosen;
    bool should_continue = true; // Controls whether to explore new branches.
    while (!s.empty()) {
      int hdr_idx = std::get<0>(s.top()), c1_idx = std::get<1>(s.top());
      State state = std::get<2>(s.top());
      s.pop();

      // Figure out what to do.
      if (should_continue && state == State::FIND_COLUMN) {
        hdr_idx = ColumnPickingPolicy::ChooseColumn(*this);
        if (hdr_idx == -1) { // Found a solution.
          should_continue = visitor.VisitSolution(chosen);
          continue;
        }
        if (C_[hdr_idx].d == hdr_idx) { // No solution.
          continue;
        }
        Cover(hdr_idx);
        s.push({hdr_idx, -1, State::UNCOVER_COLUMN});
        for (int c1_idx = C_[hdr_idx].u; C_[c1_idx].row_idx != -1;
             c1_idx = C_[c1_idx].u) {
          s.push({hdr_idx, c1_idx, State::CHOOSE_ROW});
        }
      } else if (should_continue && state == State::CHOOSE_ROW) {
        for (int c2_idx = C_[c1_idx].r; C_[c2_idx].h != hdr_idx;
             c2_idx = C_[c2_idx].r) {
          Cover(C_[c2_idx].h);
        }
        chosen.push_back(C_[c1_idx].row_idx);
        s.push({hdr_idx, c1_idx, State::UNCHOOSE_ROW});
        s.push({-1, -1, State::FIND_COLUMN});
      } else if (state == State::UNCHOOSE_ROW) {
        chosen.pop_back();
        for (int c2_idx = C_[c1_idx].l; C_[c2_idx].h != hdr_idx;
             c2_idx = C_[c2_idx].l) {
          Uncover(C_[c2_idx].h);
        }
      } else if (state == State::UNCOVER_COLUMN) {
        Uncover(hdr_idx);
      }
    }
  }

  // Solve recursively. Comment preceding ISolve(..) applies here too.
  void RSolve(VisitorInterface &visitor) {
    std::vector<int> chosen;
    std::function<bool(std::vector<int> &, VisitorInterface &)> recursive_solve;
    recursive_solve = [&](std::vector<int> &chosen,
                          VisitorInterface &visitor) -> bool {
      int hdr_idx = ColumnPickingPolicy::ChooseColumn(*this);
      if (hdr_idx == -1) {
        return visitor.VisitSolution(chosen);
      }
      if (C_[hdr_idx].d == hdr_idx) {
        return true;
      }

      // Cover the chosen column.
      Cover(hdr_idx);
      bool should_continue = true; // Controls whether to explore new branches.
      // Otherwise, pick a row and add it to the tentative solution.
      for (int c1_idx = C_[hdr_idx].d;
           should_continue && C_[c1_idx].row_idx != -1; c1_idx = C_[c1_idx].d) {
        for (int c2_idx = C_[c1_idx].r; C_[c2_idx].h != hdr_idx;
             c2_idx = C_[c2_idx].r) {
          Cover(C_[c2_idx].h);
        }
        chosen.push_back(C_[c1_idx].row_idx);
        should_continue = recursive_solve(chosen, visitor);
        chosen.pop_back();
        for (int c2_idx = C_[c1_idx].l; C_[c2_idx].h != hdr_idx;
             c2_idx = C_[c2_idx].l) {
          Uncover(C_[c2_idx].h);
        }
      }
      // Uncover the chosen column.
      Uncover(hdr_idx);
      return should_continue;
    };
    recursive_solve(chosen, visitor);
  }

  // Reverts to default constructed state. There is no reason for this
  // method to be public because ISolve(..)/RSolve(..) leave the
  // internal state invariant (and in particular, well defined) for
  // any kind of supplied visitor object.
  void Reset() {
    // Create the sentinel header cell.
    C_.resize(1);
    C_[0].l = C_[0].r = C_[0].u = C_[0].d = C_[0].h = 0;
    C_[0].row_idx = -1;
    O_.resize(1);
    O_[0] = 1;

    // Make an instance with zero rows and zero columns.
    nrows_ = ncols_ = 0;
    sec_idx_ = 1;
  }

  // Arena index of the j-th column.
  inline int AIdx(int j) const { return j + 1; }

  // Inverse of AIdx(..): Get a column index from the arena index of
  // some header cell.
  inline int CIdx(int hdr_idx) const { return hdr_idx - 1; }

  // Covers the column whose arena index is specified.
  void Cover(int hdr_idx) {
    for (int c1_idx = C_[hdr_idx].d; C_[c1_idx].row_idx != -1;
         c1_idx = C_[c1_idx].d) {
      for (int c2_idx = C_[c1_idx].r; C_[c2_idx].h != hdr_idx;
           c2_idx = C_[c2_idx].r) {
        auto &c = C_[c2_idx];
        C_[c.u].d = c.d;
        C_[c.d].u = c.u;
        O_[C_[c2_idx].h]--;
      }
      nrows_--;
    }
    // Unlink the column.
    C_[C_[hdr_idx].l].r = C_[hdr_idx].r;
    C_[C_[hdr_idx].r].l = C_[hdr_idx].l;
    ncols_--;
  }

  // Uncovers the column whose arena index is specified.
  void Uncover(int hdr_idx) {
    // Link the column.
    ncols_++;
    C_[C_[hdr_idx].l].r = hdr_idx;
    C_[C_[hdr_idx].r].l = hdr_idx;
    for (int c1_idx = C_[hdr_idx].u; C_[c1_idx].row_idx != -1;
         c1_idx = C_[c1_idx].u) {
      nrows_++;
      for (int c2_idx = C_[c1_idx].l; C_[c2_idx].h != hdr_idx;
           c2_idx = C_[c2_idx].l) {
        auto &c = C_[c2_idx];
        O_[C_[c2_idx].h]++;
        C_[c.d].u = c2_idx;
        C_[c.u].d = c2_idx;
      }
    }
  }

  /////////////////////
  // Private members //
  /////////////////////
private:
  // Number of active rows and columns.
  int nrows_, ncols_;
  // Count active ones in a given column. Arena indices!
  std::vector<int> O_;
  // Arena for storing all the cells. Cell at index zero is a special
  // sentinel cell that is guaranteed to exist and C_[0].r points to
  // the first header cell aka that of column at index zero.
  std::vector<Cell> C_;
  // The arena index of the first secondary column. A secondary
  // column need not be covered, and in each instance we assume they
  // are grouped together so that all primary columns come before
  // all secondary columns.
  int sec_idx_;
};

} // namespace dlx

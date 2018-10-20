#pragma once

#include <vector>

namespace dlx {

// Interface class for matrices of Exact Cover instances. The rows and
// columns indices are zero-based and in the case that there are no
// secondary columns in the instance, FirstSecondaryColumnIndex()
// and Cols() should return identical values.
class MatrixInterface {
public:
  virtual int Rows() = 0;
  virtual int Cols() = 0;
  virtual int Value(int i, int j) = 0;
  virtual int FirstSecondaryColumnIndex() = 0;
};

// Example implementation.
template <class T> class MatrixFromVector : public MatrixInterface {
public:
  MatrixFromVector(const std::vector<std::vector<T>> &matrix, int sec_idx)
      : m_(matrix), sec_idx_(sec_idx) {}
  int Rows() override { return m_.size(); }
  int Cols() override { return (m_.size() > 0 ? m_[0].size() : 0); }
  int Value(int i, int j) override { return m_[i][j]; }
  int FirstSecondaryColumnIndex() override { return sec_idx_; }

private:
  std::vector<std::vector<T>> m_;
  int sec_idx_;
};

} // namespace dlx

#pragma once

#include <random>
#include <vector>

namespace dlx {

/////////////////////////////
// Column Picking Policies //
/////////////////////////////

// Each column picking policy has access (through friendship) to all
// the private members of dlx::DancingLinks. Using this information a
// single static ChooseColumn(..) method must be defined which should
// return a valid arena index of some active primary column (or -1 in
// case there are no such columns).

struct FirstAvailableColumn {
  template <class T> static int ChooseColumn(const T &dlx) {
    return dlx.C_[0].r != 0 && dlx.C_[0].r < dlx.sec_idx_ ? dlx.C_[0].r : -1;
  }
};

struct LastAvailableColumn {
  template <class T> static int ChooseColumn(const T &dlx) {
    int hdr_idx = dlx.C_[0].l;
    while (hdr_idx != 0 && hdr_idx >= dlx.sec_idx_)
      hdr_idx = dlx.C_[hdr_idx].l;
    return hdr_idx == 0 ? -1 : hdr_idx;
  }
};

struct UniformlyRandomColumn {
  template <class T> static int ChooseColumn(const T &dlx) {
    if (dlx.C_[0].r == 0 || dlx.C_[0].r >= dlx.sec_idx_)
      return -1;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    int hdr_idx = dlx.C_[0].r;
    int rnd_idx = hdr_idx, i = 1;
    while (hdr_idx != 0 && hdr_idx < dlx.sec_idx_) {
      rnd_idx = ((uniform(gen) <= double(1.0) / i) ? hdr_idx : rnd_idx);
      hdr_idx = dlx.C_[hdr_idx].r;
      i++;
    }
    return rnd_idx;
  }
};

struct ColumnWithLeastOnes {
  template <class T> static int ChooseColumn(const T &dlx) {
    int best_idx = -1, best_val = dlx.nrows_ + 1;
    for (int hdr_idx = dlx.C_[0].r; hdr_idx != 0 && hdr_idx < dlx.sec_idx_;
         hdr_idx = dlx.C_[hdr_idx].r) {
      if (dlx.O_[hdr_idx] < best_val) {
        best_val = dlx.O_[hdr_idx];
        best_idx = hdr_idx;
      }
    }
    return best_idx;
  }
};

} // namespace dlx

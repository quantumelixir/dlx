#pragma once

namespace dlx {

// An exact cover input instance is a matrix of 1s and 0s. A Cell is
// common structure to hold, and link in four directions, the Column
// Headers and the 1s in the matrix.
struct Cell {
  int l, r, u, d; // Arena indices of the cell's neighbors.
  int h;          // Arena index of the cell's header.
  int row_idx;    // Row index (in the matrix) of the cell.
};

} // namespace dlx

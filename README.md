# Configurable and Efficient Implementation of Algorithm X

This is a modern C++ implementation of Donald Knuth's [Dancing
Links](https://en.wikipedia.org/wiki/Dancing_Links) algorithm to solve
Exact Cover problems. I wrote this as a design exercise. It also seems
to run quite fast.

I'm putting this out there in case someone finds it useful. Take a
look at the examples directory and have fun with it!

# Design features

- Static polymorphism in `dlx::DancingLinks` via policy classes to
  squeeze out as much performance as possible, while allowing clients
  to alter the column picking heuristics.

- Dependency injection through abstract interface classes allows the
  solver to perform different functions at runtime like count the
  number of solutions, test uniqueness, or collect the solutions into
  a container.

- Examples like NQueens and Sudoku use composition along with a single
  object of type `dlx::DancingLinks` to handle a variety of input and
  output formats for different problems.

# Crude Benchmarks

Included in `examples/data/sudoku.in.txt` is a collection of 91 sudoku
problems. Except for 4 problems this is the same problem set as the
one featured in [this article](http://norvig.com/sudoku.html), where
it is claimed to be a collection of 95 hard sudoku problems.

It takes less than half a second on a Mid-2015 2.2GHz Core i7 Macbook
Pro for this library to solve (and verify uniqueness of) all the 91
problems in this data set.

# Getting Started
Check out some of the examples to get started using this library. To
build the examples (and run tests) you need the
[Bazel](https://www.bazel.build/) build system. From the root
directory of the project you can build and run examples like this:

      $ bazel run -c opt examples:sudoku # the basic basic benchmark
      $ bazel run examples:nqueens 42    # 42 non-attacking queens
      $ bazel run tests:tests            # not using google test ATM

# PEDaLS
PEDaLS is an approach to persist versioned data structures in distributed settings (e.g. cloud + edge) using append-only storage (i.e. logs). The design of PEDaLS is agnostic to the underlying log system and only requires a set of basic functionalities from it (e.g. log creation, appending to a log, retrieval of an entry corresponding to a sequence number from a log, etc.). The current implementation is based on [CSPOT](https://github.com/MAYHEM-Lab/cspot), however, other log systems such as [KAFKA](https://kafka.apache.org/) and [LogDevice](https://logdevice.io/) also provide features necessary for implementing PEDaLS.

## Installation and First Run
1. Install [CSPOT](https://github.com/MAYHEM-Lab/cspot)
2. Clone this repository in the same directory where CSPOT is installed
3. `cd` into `PEDaLS`
4. Run `make`
5. `cd` to the `cspot` directory inside `bst-example`(`linked-list-example`) and run `bst-example`(`linked-list-example`) to see a simple PEDaLS binary search tree (linked-list) in action. This program executes four update operations and prints out the contents in each of these four versions. Make sure `woofc-namespace-platform` is also running inside the `cspot` directory before running the example.

## Project Structure
- `bst` and `linked-list` contain the source code for PEDaLS bst and linked-list respectively.
- `auxiliary-structures`, `data-structures`, `options`, and `helpers` contain supporting codes for PEDaLS bst and PEDaLS linked-list.
- The other folders along with the above contain codes to run results presented in the PEDaLS paper (the paper will be linked once it is presented in IC2E 2021).

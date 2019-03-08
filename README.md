s1o_example
===========

[![Build Status](https://travis-ci.org/hpg-cepetro/s1o_example.svg?branch=master)](https://travis-ci.org/hpg-cepetro/s1o_example)

This is an example on how to use the Spatial IO project (s1o) with seismic data in the Seismic Unix (SU) format.

This project contains three programs:

- `su2s1o`: Packs several SU files **with idential headers** into a single s1o multiset with an accelerated 4D search structure stored on disk (rtree) indexed by **midpoint and half-offset coordinates**.

- `s1o2su`: Extracts a single SU file from the s1o multiset directly to `stdout` so it can be used directly with other SU tools.

- `s1o2su_q`: Similar to `s1o2su`, but with support for queries so data in the dataset can be filtered.

## Requirements

The following projects are required in order to compile and run this example:

- [boost](https://www.boost.org/) (>= 1.67.0)
- [ex3](https://github.com/Caian/ex3)
- [f1d](https://github.com/Caian/f1d)
- [s1o](https://github.com/Caian/s1o)
- [z0rg](https://github.com/Caian/z0rg)

All repositories except for boost are included in this example through git submodules, so be sure to use the proper `git clone` command in order to pull everything.

For linking, the dynamic load `ldl` library is also required. This library should be available out-of-box in most Linux distributions.

This project uses CMake to compile the project, though it is easy to compile it from command-line (everything is header only).

## Usage

### su2s1o

To pack several SU files into a single s1o dataset (or multiset):

```
su2s1o tacutu.A.su tacutu.V.su tacutu.coher.su tacutu.stack.su tacutu-pack
```

**Note:** All SU files are required to have the same trace headers in the same order, so this program works best with outputs that are generated simultaneously by the same program.

### s1o2su

To extract one of the SU files you must pass the number of files (slots) originally packed (limitation of s1o) and the index you want to extract:

```
s1o2su tacutu-pack 4 1 > tacutu-extracted.V.su
```

This program works with pipe so it can be integrated with other SU tools:

```
s1o2su tacutu-pack 4 2 | suximage legend=1
```

### s1o2su_q

To allow more complex queries when selecting which traces will be extracted:

```
s1o2su_q tacutu-pack 4 1 [query] > tacutu-subset.V.su
```

Where the syntax for the query is one of the following:

- `exact,mx,my,hx,hy` - Search for traces matching this exact coordinates.
- `nearest,mx,my,hx,hy,k` - K-nearest neighbor search around these coordinates.
- `range,mx0:mx1,my0:my1,hx0:hx1,hy0:hy1` - Range search in each coordinate.

The range search syntax also allows all coordinates searches to be open-ended:

- `:` - Selects the entire range of the data.
- `v0:` - Selects data greater than or equal to `v0`.
- `:v1` - Selects data smaller than or equal to `v1`.

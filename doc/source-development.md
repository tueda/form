# FORM Source Development

Read this document completely before creating or modifying project-maintained
C/C++ source or header files. It covers FORM-specific architecture and
correctness invariants; formatting, folds, Doxygen, and license rules are in
`coding-style.md`.

## Architecture

- `sources/` contains the shared engine and code for all executable variants.
  Major functional areas include startup (`startup.c`), preprocessing (`pre.c`),
  compilation (`compiler.c`, `comexpr.c`, `compcomm.c`), module execution
  (`module.c`, `execute.c`, `proces.c`), normalization and sorting (`normal.c`,
  `sort.c`), and persistent storage (`store.c`).
- `threads.c` contains most TFORM-specific code. `mpi.c` and `parallel.c`
  contain ParFORM-specific code. Most other sources are compiled into multiple
  variants through preprocessor-controlled builds.
- Headers are not generally paired one-to-one with source files. `declare.h`
  holds widely used declarations and macros; `form3.h` provides core
  preprocessor definitions and common includes; `ftypes.h` defines internal
  codes; `structs.h` defines core state; and `variable.h` exposes state-access
  macros.

FORM's runtime state is grouped in structures declared in `structs.h`. Serial
FORM uses the global `A`. TFORM keeps shared data in `A` and thread-private
`N_const`, `R_const`, and `T_const` data referenced through `AB`; `variable.h`
macros make access look uniform. Many internal expressions are arrays of `WORD`
values. Data stored in movable or resizable buffers often uses offsets rather
than pointers.

## Correctness Invariants

- Keep optional dependencies optional. Do not make common code require FLINT,
  GMP, MPFR, zlib, or zstd unless the task explicitly changes supported
  configurations.
- Do not incidentally drop supported platforms or raise compiler,
  language-standard, or standard-library requirements in a source change. Treat
  either as explicit compatibility-policy work.
- Treat `WORD`-encoded terms, numeric codes in `ftypes.h`, and persistent data
  written or read by `store.c` or `checkpoint.c` as format-sensitive. When
  changing these representations, update every producer and consumer together.
- Do not assume that `WORD` is 32 bits or that `sizeof(WORD) == 4`. It is 32
  bits on 64-bit targets and 16 bits on 32-bit targets. Use `BITSINWORD`,
  `WORD_MIN_VALUE`, and `WORD_MAX_VALUE` when width or range matters, and audit
  shifts, conversions, and encoded data for both widths.
- In ordinary code running for the current TFORM worker, access thread-private
  state through `AN`, `AR`, and `AT`, with `B` supplied via `PHEAD` / `BHEAD` or
  obtained with `GETIDENTITY`. Coordination, initialization, cleanup, and
  similar management code may deliberately address a specific worker through
  `AB[id]`; keep such access explicit and do not introduce it into ordinary
  shared algorithms.
- Keep executable variants unified through established macros and conditional
  compilation. Do not duplicate a common algorithm merely to handle one
  executable variant.
- Preserve user-visible FORM-language syntax, semantics, and output unless a
  behavior change is intentional. Such changes require a regression test. Update
  the relevant manual source when documented behavior changes or a new feature
  requires documentation.

## Implementation Conventions

- Prefer existing FORM allocation, diagnostics, file, and cleanup facilities
  such as `Malloc1`, `M_free`, and `MesPrint` where the subsystem already uses
  them.
- Pointers marked `[D]` in global-structure documentation refer to dynamically
  allocated storage. Check the actual ownership and sharing rules before
  changing how that storage is allocated, shared, or freed. When adding or
  changing such state, audit the applicable initialization, resizing,
  checkpoint and recovery, and shutdown paths.

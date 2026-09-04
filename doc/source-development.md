# FORM Source Development

Read this document completely before creating or modifying project-maintained
C or C++ source or header files. It describes FORM-specific architecture and
correctness invariants. Formatting, folds, Doxygen, and license rules are in
`coding-style.md`.

## Architecture

- `sources/` contains the shared engine and all executable variants. Important
  flow boundaries include startup (`startup.c`), preprocessing (`pre.c`),
  compilation (`compiler.c`, `comexpr.c`, `compcomm.c`), module execution
  (`module.c`, `execute.c`, `proces.c`), normalization and sorting
  (`normal.c`, `sort.c`), and persistent storage (`store.c`).
- `threads.c` contains most TFORM-specific code. `mpi.c` and `parallel.c`
  contain ParFORM-specific code. Most other sources are compiled into multiple
  variants through preprocessor-controlled builds.
- Headers are not generally paired one-to-one with source files. `declare.h`
  holds widely used declarations and macros; `form3.h` is the main include and
  configuration boundary; `ftypes.h` defines internal codes; `structs.h`
  defines core state; and `variable.h` exposes state-access macros.

FORM's runtime state is grouped in structures declared in `structs.h`. Serial
FORM uses the global `A`. TFORM keeps shared data in `A` and thread-private
`N_const`, `R_const`, and `T_const` data behind `AB`; `variable.h` macros make
access look uniform. Many internal expressions are arrays of `WORD` values and
use offsets instead of pointers so buffers can be resized, moved, and written
to disk.

Use `doc/devref/source.tex` and related developer-reference chapters for
architectural orientation only. The official project resources label this
developer reference as outdated; verify every claim against current code and
tests.

## Correctness Invariants

- Keep optional dependencies optional. Common code must retain the guarded
  non-FLINT, non-GMP/MPFR, non-zlib, and non-zstd paths unless the task
  explicitly changes supported configurations.
- Do not incidentally drop supported platforms or raise compiler,
  language-version, or standard-library requirements in a source change.
  Treat either as explicit compatibility-policy work.
- Treat `WORD`-encoded terms, numeric codes in `ftypes.h`, and data handled by
  `store.c` or `checkpoint.c` as format-sensitive. Change every producer and
  consumer together and add focused round-trip or recovery tests.
- Do not assume that `WORD` is 32 bits or that `sizeof(WORD) == 4`. It is 32
  bits on 64-bit targets and 16 bits on 32-bit targets. Use `BITSINWORD`,
  `WORD_MIN_VALUE`, and `WORD_MAX_VALUE` when width or range matters, and audit
  shifts, conversions, and encoded data for both widths.
- In ordinary code running for the current TFORM worker, access `AN`, `AR`, and
  `AT` through the private-state pointer supplied by `PHEAD` / `BHEAD` or
  `GETIDENTITY`. Coordination, initialization, cleanup, and similar management
  code may deliberately address a specific worker through `AB[id]`; keep such
  access explicit and do not introduce it into ordinary shared algorithms.
- Keep executable variants unified through established macros and conditional
  compilation. Do not duplicate a common algorithm merely to handle one
  executable variant.
- Preserve public FORM-language syntax and output unless a behavior change is
  intentional. User-visible syntax or semantics changes require a regression
  test. Update the relevant manual source when documented behavior changes or a
  new feature requires documentation.

## Implementation Conventions

- Prefer existing FORM allocation, diagnostics, file, and cleanup facilities
  such as `Malloc1`, `M_free`, and `MesPrint` where the subsystem already uses
  them; they participate in project-specific debugging and behavior.
- Pointers marked `[D]` in global-structure documentation own dynamically
  allocated memory. When adding or changing such state, audit initialization,
  resizing, checkpoint and recovery, and shutdown paths.
- Match the subsystem's established names and interfaces. Do not perform broad
  renaming, modernization, or style conversion as incidental cleanup.

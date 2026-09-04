# AGENTS.md

## Project

FORM is a symbolic manipulation system for very large algebraic expressions. It
normally reads `.frm` programs, performs symbolic and algebraic transformations
with exact arithmetic and optional arbitrary-precision floating-point
arithmetic, and writes textual mathematical results. Streaming and sorting
limit expression size by available disk space rather than RAM.

Serial FORM (`form`), threaded TFORM (`tform`), MPI-based ParFORM (`parform`),
and their debug variants are implemented in C and C++ and built with Autoconf
and Automake.

For this project, correctness includes mathematical results, established
FORM-language behavior, affected on-disk data, and consistency across applicable
executables and optional-library configurations.

## Task-Specific Guidance

- Before creating or modifying project-maintained C or C++ source or header
  files, read and follow `doc/source-development.md` and
  `doc/coding-style.md`.
- Before modifying maintained build-system inputs or related generation and
  distribution logic, read `doc/build-system.md`.
- Read `check/README.md` before adding or modifying tests, changing the test
  runner, or changing marked manual examples covered by `check/examples.frm`.
- Apply the same reading requirements when reviewing changes in these areas.
- Read `doc/coding-style-rationale.md` only when proposing, reviewing, or
  changing the coding-style policy.
- Read `README.md` and `INSTALL` before setting up a build or changing
  configuration, dependencies, installation, or supported platforms. Confirm
  current switches with `./configure --help`.
- Treat `doc/manual/*.tex` as the intended user-facing specification for
  FORM-language behavior. If the manual, regression tests, and implementation
  disagree about current behavior, report the discrepancy before deciding what
  to change. Update the relevant source when documented behavior changes or a
  new feature requires manual documentation.
- Consult the
  [official FORM resources](https://github.com/form-dev/form/wiki/FORM-Resources)
  when local documentation does not cover a current external tool, platform,
  or project resource.

## Development Workflow

### Commands and Verification

`configure` requires the zstd wrapper sources even when zstd library support is
disabled. In a Git checkout it attempts to initialize the submodule
automatically; initialize it explicitly if needed before configuring:

```sh
git submodule update --init extern/zstd
```

Do not assume an existing build directory is disposable; it may be managed by
local tooling. Before reusing one, inspect its `config.status --config` output
and confirm that its ownership, compiler, dependencies, and enabled variants fit
the task. Otherwise choose a new task-owned out-of-source directory. The
examples below use `build-agent`; adapt the directory and source-relative paths
when the task or local tooling requires another location:

```sh
autoreconf -i
mkdir -p build-agent
(cd build-agent && ../configure --enable-debug)
make -C build-agent -j4
```

A reachable release tag is needed for complete Git revision information, but
not for the build itself.

Run the narrowest relevant test first. From the repository root:

```sh
./check/check.rb ./build-agent/sources/form TestName
./check/check.rb -w 2 ./build-agent/sources/tform TestName
./check/check.rb ./build-agent/sources/form fixes.frm
```

Test names may be exact names or patterns. Use `./check/check.rb --list` to
discover them and `./check/check.rb --help` for runner options.

For the standard suite on every release executable enabled in the build:

```sh
make -C build-agent check
```

Do not count this as a standard-suite pass unless `configure` detected Ruby and
`test/unit` and the log confirms that the standard `.frm` tests ran for every
intended executable. Without the Ruby harness, Automake may run only the
benchmark test.

For changes to shared execution code, test at least `form` and `tform`, with a
non-default TFORM worker count when concurrency could matter. Test `parform`
when MPI-specific code or parallel code paths used by both TFORM and ParFORM
change. If an MPI-capable build environment is unavailable, run all applicable
`form` and `tform` tests and report explicitly that `parform` was not built or
tested, why, and what remains unverified. The test workflow additionally covers
extra tests, no-FLINT builds, 32-bit builds, Valgrind, and coverage. Reproduce
those expensive or environment-sensitive jobs locally only when the affected
risk warrants it.

Never add, broaden, or relabel a coverage exclusion merely to improve the
reported score. Prefer testing reachable behavior. Every exclusion requires a
code-specific justification and an established marker from `.lcovrc`; changing
those marker categories is separate coverage-policy work.

The extra suite needs external FORM libraries on `FORMPATH` and uses a longer
timeout. Once those prerequisites are present, its local form is:

```sh
./check/check.rb --timeout 60 -C extra ./build-agent/sources/form
```

There is no approved repository-wide source formatter or separate local lint
command. Compiler warnings produced by the configured build are the applicable
local static check. Before finishing, inspect `git status --short`; run
`git diff --check` and `git diff --cached --check`; and inspect intended
untracked text files for whitespace errors and conflict markers. For committed
branch changes, identify the intended base branch and run
`git diff --check <base>...HEAD`. Verify intentional whitespace in folds or
Markdown rather than deleting it.

For changes under `doc/manual/`, run each of the following targets that was
enabled by `configure`:

```sh
make -C build-agent/doc/manual pdf
make -C build-agent/doc/manual latex2html
```

If a target is unavailable because `configure` did not find its required
tools, report it as not run.

### Version Control and External Actions

- Inspect the worktree before editing and before finishing.
- Preserve unrelated changes. Do not use destructive Git operations without
  explicit authorization.
- Do not commit, push, open or update a pull request, release, publish, or
  deploy unless the user explicitly requests that action or an explicitly
  requested repository procedure requires it.
- Never commit secrets, credentials, private keys, access tokens, or private
  user data.

#### Commit Recommendation

- End a repository change by stating whether it is ready to commit and why; do
  not call incomplete or failed work ready.
- When commit preparation is requested, propose independently reviewable commit
  units, exact staging commands, and complete messages, staging only intended
  changes and following repository conventions.

## Repository Guide

- `sources/` contains the shared engine and all executable variants. See
  `doc/source-development.md` for its architecture and correctness invariants.
- `check/` contains the Ruby runner and FORM-language tests. Test work runs in
  per-case temporary directories managed by the runner.
- `doc/manual/` is the user reference manual, `doc/doxygen/` configures source
  documentation, and `doc/devref/` contains an older developer reference.
  Task-specific development guidance is under `doc/` as routed above.
- `configure.ac`, the `Makefile.am` files, and `m4/` are the maintained build
  definitions. `.github/workflows/test.yml` defines test and analysis
  coverage; `.github/workflows/deploy.yml` validates source distributions,
  cross-platform builds, and PDF/HTML manual artifacts.
- `extern/zstd` is a Git submodule. Do not modify vendored or imported code as
  if it followed ordinary FORM source ownership and style.
- `AGENTS.md` is the shared coding-agent policy. `CLAUDE.md` only imports it;
  keep common instructions here instead of duplicating them in the shim.

## Working Principles

Correctness and established FORM behavior come first. Among otherwise correct
solutions, prioritize runtime performance, user-facing usability, and
readability, in that order when they genuinely conflict. Performance-driven
complexity requires evidence of a material benefit on representative workloads
proportionate to the added complexity and maintenance cost. Do not trade
readability for speculative or negligible gains. Optimize the resulting code,
not the speed of completing the task.

In addition, the following repository-agnostic principles guard against common
coding-agent failures.

### 1. Think Before Coding

- Inspect relevant code, tests, configuration, and documentation first.
  Separate repository facts from assumptions; never invent requirements,
  behavior, evidence, or results.
- Resolve ambiguity from repository evidence first. Ask only when different
  answers would materially affect behavior, interfaces, data, safety, or
  scope; otherwise use the smallest reversible assumption.
- Treat explanation, review, diagnosis, and planning as read-only unless
  changes are requested. For non-trivial changes, define the intended result
  and verification before editing.

**Before editing:** What do I know, what am I assuming, and what remains
unclear?

### 2. Simplicity First

- Implement the smallest clear solution that fully solves the task. Avoid
  speculative features, abstractions, dependencies, and extension points.
- Add complexity only when it protects a real requirement, invariant,
  testability need, or material performance benefit.

**Ask yourself:** Is this more complicated than the task requires?

### 3. Surgical Changes

- Every changed line must follow from the task or a necessary consequence of
  it. Avoid unrelated refactoring, cleanup, formatting, and dependency changes.
- Preserve unrelated work and existing behavior unless the task requires
  otherwise. Never weaken tests or criteria merely to make checks pass.

**The test:** Can every changed line be traced to the task or a necessary
consequence of it?

### 4. Goal-Driven Execution

- Translate the request into verifiable acceptance criteria. For bugs,
  reproduce the failure and add a focused regression test when practical.
- Run the narrowest relevant checks first, then required broader checks, and
  inspect their results. Review the final diff and report what changed, what
  was verified, and what remains unverified.

**Ask yourself:** What proves this task is complete, and have I verified it?

### Beyond Coding

Apply these four principles to writing, analysis, mathematical proofs, and
experiments, interpreting their terms for the task.

Match important claims to evidence appropriate to their kind, and do not claim
more than the evidence establishes. Tests and examples establish only the
cases they cover; numerical evidence alone does not prove a general
mathematical statement. State important assumptions and anything that remains
unverified.

## Definition of Done

A task is complete only when:

- Applicable builds, tests, and other required checks pass, or each omission,
  its reason, and remaining uncertainty are reported.
- Required documentation, generated files, distribution lists, and test
  expectations match the change.
- Temporary files and debug output created by the task are removed, and the
  change introduces no accidental formatting or exposed secrets.

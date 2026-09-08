# AGENTS.md

## Project

FORM is a symbolic manipulation system for very large algebraic expressions. It
normally reads `.frm` programs to perform symbolic and algebraic transformations
with exact arithmetic and optional arbitrary-precision floating-point
arithmetic. Streaming and sorting allow FORM to process expressions that do not
fit in available RAM.

Serial FORM (`form`), threaded TFORM (`tform`), MPI-based ParFORM (`parform`),
and their debug variants are implemented in C and C++ and built with Autoconf
and Automake.

For this project, correctness includes mathematical results, established
FORM-language behavior, persistent data and formats, and consistency across
applicable executables and optional-library configurations.

## Task-Specific Guidance

- Before creating or modifying project-maintained C or C++ source or header
  files, read and follow `doc/source-development.md` and `doc/coding-style.md`.
- Before modifying maintained build-system inputs or related generation and
  distribution logic, read and follow `doc/build-system.md`.
- Read `check/README.md` before adding or modifying tests or changing the test
  runner. Before changing an example in `doc/manual/*.tex`, check whether it is
  covered by `check/examples.frm`; if so, read `check/README.md`.
- Apply the same reading requirements when reviewing changes in these areas.
- Consult `doc/coding-style-rationale.md` only when proposing, reviewing, or
  changing the coding-style policy, or when a coding-style rule needs
  interpretation.
- Use the development workflow below for routine build setup. Consult `INSTALL`
  when that workflow is insufficient.
- Treat `doc/manual/*.tex` as the intended user-facing specification for
  FORM-language behavior. If the manual, regression tests, and implementation
  disagree about current behavior, investigate the discrepancy from repository
  evidence before deciding what to change, and report the discrepancy and any
  remaining uncertainty. Update the relevant manual source when documented
  behavior changes or a new feature requires documentation.
- Consult the
  [official FORM resources](https://github.com/form-dev/form/wiki/FORM-Resources)
  for project information not covered by local documentation.
  For external tools or platforms, prefer their official documentation.

## Development Workflow

### Worktree Safety

- Inspect the worktree before editing and identify any pre-existing changes.
- Preserve unrelated changes. Do not use destructive Git operations without
  explicit authorization.

### Build Setup

Do not assume an existing build directory is disposable. Before reusing one,
check whether it is already in use by the user or managed by local tooling, and
inspect `config.status --config`. Verify that its compiler, dependencies, and
enabled variants fit the task. Otherwise use a new task-owned out-of-source
directory. The examples below use `build-agent`; adapt the directory, configure
options, and source-relative paths as needed:

```sh
autoreconf -i
mkdir -p build-agent
(cd build-agent && ../configure --enable-debug)
make -C build-agent -j4
```

A reachable release tag is needed for complete Git revision information, but not
for the build itself.

### Testing

Run tests directly with `check/check.rb`. Start with the narrowest relevant
tests and expand coverage according to the affected behavior and risk. From the
repository root:

```sh
./check/check.rb ./build-agent/sources/form -n TestName
./check/check.rb ./build-agent/sources/tform -w 2 -n TestName
./check/check.rb ./build-agent/sources/form fixes.frm
```

Test names may be exact names or patterns. Use `./check/check.rb --list` to
discover them and `./check/check.rb --help` for runner options.

To run the standard suite for one executable, omit test names and file filters.
For changes with broad effects on shared execution or FORM-language behavior,
run the standard suite on each applicable executable. When changes affect
debug-specific behavior, test the corresponding `vorm`, `tvorm`, or `parvorm`.

For changes to shared execution code, test at least `form` and `tform`, with a
non-default TFORM worker count when concurrency could matter. Test `parform`
when MPI-specific code or parallel code paths used by both TFORM and ParFORM
change. If such testing is warranted but an MPI-capable build environment is
unavailable, run the applicable `form` and `tform` tests and report explicitly
that `parform` was not built or tested, why, and what remains unverified.

The extra suite needs external FORM libraries on `FORMPATH` and uses a longer
timeout. Once those prerequisites are present, run it locally as:

```sh
./check/check.rb --timeout 60 -C extra ./build-agent/sources/form
```

The CI test workflow also covers extra tests, no-FLINT builds, 32-bit builds,
Valgrind, and coverage. Reproduce relevant broader CI checks locally when
warranted by the affected risk and feasible in the available environment. If a
warranted check is not feasible, report what remains unverified and why.

Never add, broaden, or relabel a coverage exclusion merely to improve the
reported score. Prefer testing reachable behavior. Every exclusion requires a
code-specific justification and an established marker from `.lcovrc`; changing
those marker categories is separate coverage-policy work.

### Manual Verification

For changes under `doc/manual/`, run each of the following targets that
`configure` enabled:

```sh
make -C build-agent/doc/manual pdf
make -C build-agent/doc/manual latex2html
```

For any target that `configure` did not enable, report it as not run and state
why. A no-op target does not by itself establish that it was enabled by
`configure`.

### Final Checks

There is no approved repository-wide source formatter or separate local lint
command. Review compiler warnings from any builds run for the task. Before
finishing, inspect `git status --short`; run `git diff --check` and
`git diff --cached --check`; and inspect intended untracked text files for
whitespace errors and conflict markers. For committed branch changes, identify
the intended base branch and run `git diff --check <base>...HEAD`. If a check
reports whitespace in folds or Markdown that may be intentional, verify it
before deleting it.

### Commit Recommendation

- End a repository change by stating whether it is ready to commit and why; do
  not call work ready if it is incomplete or required verification has failed.
- When commit preparation is requested, propose independently reviewable commit
  units, exact staging commands, and complete messages that follow repository
  conventions. When staging is part of the requested work, stage only intended
  changes.

### Version Control and External Actions

- Do not commit, push, open or update a pull request, release, publish, or
  deploy unless the user explicitly requests that action or an explicitly
  requested repository procedure requires it.
- Never commit secrets, credentials, private keys, access tokens, or private
  user data.

## Repository Guide

- `sources/` contains the shared engine and all executable variants. See
  `doc/source-development.md` for its architecture and correctness invariants.
- `check/` contains the Ruby runner and FORM-language tests. Test work runs in
  per-case temporary directories managed by the runner.
- `doc/manual/` is the user reference manual, and `doc/doxygen/` configures
  source documentation.
- `configure.ac`, the `Makefile.am` files, and `m4/` are the maintained build
  definitions. `.github/workflows/test.yml` defines test and analysis CI jobs;
  `.github/workflows/deploy.yml` validates source distributions, cross-platform
  builds, and PDF/HTML manual artifacts.
- `extern/zstd` is a Git submodule; treat it as upstream code rather than
  ordinary FORM-maintained source.

## Working Principles

Correctness, including established FORM behavior, comes first. Among correct,
in-scope solutions, prioritize runtime performance, user-facing usability, and
readability, in that order when they genuinely conflict. Add complexity for
performance only when evidence from representative workloads shows a material
benefit commensurate with the added complexity and maintenance cost. Do not
trade readability for speculative or negligible gains. Optimize the resulting
code, not the speed of completing the task.

The following repository-agnostic principles guard against common coding-agent
failures.

### 1. Think Before Coding

- Inspect relevant code, tests, configuration, and documentation first. Separate
  repository facts from assumptions; never invent requirements, behavior,
  evidence, or results.
- Resolve ambiguity from repository evidence first. Ask only when different
  answers would materially affect the task outcome; otherwise use the smallest
  reversible assumption.
- During explanation, review, diagnosis, or planning, leave maintained project
  files unchanged unless changes are requested. Inspection and verification may
  create temporary files and build artifacts.

**Before editing:** What do I know, what am I assuming, and what remains
unclear?

### 2. Simplicity First

- Implement the smallest clear solution that fully solves the task. Avoid
  speculative features, abstractions, dependencies, and extension points.
- Add complexity only when justified by a real requirement, invariant,
  testability need, or material performance benefit.

**Ask yourself:** Is this more complicated than the task requires?

### 3. Surgical Changes

- Every changed line must follow from the task or a necessary consequence of it.
  Avoid unrelated refactoring, cleanup, formatting, and dependency changes.
- Preserve existing behavior outside the task's scope. Never weaken tests or
  acceptance criteria merely to make checks pass.

**The test:** Can every changed line be traced to the task or a necessary
consequence of it?

### 4. Goal-Driven Execution

- Before making a non-trivial change, translate the request into verifiable
  acceptance criteria. For bugs, reproduce the failure before making the fix
  whenever feasible. If adding a regression test, confirm that it fails for the
  expected reason before the fix and passes after it.
- Run the narrowest relevant checks first, then required broader checks, and
  inspect their results. Review the final diff and report what changed, what was
  verified, and what remains unverified.

**Ask yourself:** What proves this task is complete, and have I verified it?

### Beyond Coding

Apply these four principles to writing, analysis, mathematical proofs, and
experiments, interpreting their terms for the task.

Support important claims with evidence appropriate to their kind. Do not claim
more than the evidence establishes. Tests and examples establish only the cases
they cover; numerical evidence alone does not prove a general mathematical
statement. State important assumptions and anything that remains unverified.

## Definition of Done

A task is complete only when:

- Each applicable build, test, and other check either passes or, if it cannot be
  run, is reported as not run, together with the reason and remaining
  uncertainty.
- Required documentation, distribution lists, and test expectations match the
  change.
- Temporary files and debug output created by the task are removed, and the
  change introduces no accidental formatting changes or exposed secrets.

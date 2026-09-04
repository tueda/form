# Rationale for FORM C/C++ Coding Style

This document records the source survey, evidence, and judgement underlying
`coding-style.md`. The survey results are evidence for the canonical rules; this
document is intentionally more detailed and less normative than the style guide
itself.

The audit baseline is `v5.0.1`; `v4.3.1` is used only as a historical reference.

The audit treats project-maintained code, primarily under `sources/`, as
normative evidence. Vendored or imported code such as `extern/` retains its
upstream or component-local style and is outside this survey.

Rule IDs such as `CS-BRACES` correspond exactly to the same IDs in
`coding-style.md`. The IDs are **cross-reference labels only**; they do not
indicate priority, severity, or application order.

## Audit goal

The repository is old, large, and multi-author. It therefore contains at least
four different kinds of formatting evidence:

1. **Documented design conventions** — explicitly described in FORM/STedi
   documentation.
2. **Strong house patterns** — repeated broadly in core source and still visible
   in maintained code at the audit baseline.
3. **Local or subsystem styles** — coherent inside a particular C++ subsystem or
   author's code, but not representative of the repository as a whole.
4. **Legacy/noise** — old density, historical syntax, accidental drift, and
   isolated exceptions.

The purpose of the audit is **not** to average all existing formatting. The
purpose is to recover a single canonical FORM style suitable for new code while
preserving reviewability of legacy code.

## Audit method

The audit used several complementary methods:

- inventory and fixed-string file-presence counts for `sources/` at `v5.0.1`;
- direct inspection of representative `v5.0.1` files, including C++ code such
  as `features.cc`;
- inspection of selected `v4.3.1` sources and the same fixed-string queries for
  historical comparison;
- comparison with FORM developer documentation, the developer Wiki as consulted
  for this audit, STedi documentation, and the C23/K&R cleanup discussion in
  PR #764;
- comparison with common formatter concepts only to describe mappings, not to
  redefine FORM style.

The source-tree counts below are **file-presence counts** at `v5.0.1`, not token
counts. The release tag is the sole audit baseline for the table.

A file is counted if it contains the queried pattern at least once. A single
file may therefore appear in both sides of a comparison if it contains both
forms. Fixed-string search cannot reliably classify every occurrence by AST
role, so ambiguous rules were checked against real source excerpts before being
made normative.

The audited `sources/` inventory contains 86 C/C++/header source files: 52 `.c`,
11 `.cc`, and 23 `.h`. The following Bash fragment reproduces the inventory and
all fixed-string counts in the table:

```bash
audit=v5.0.1
source_paths=( 'sources/*.c' 'sources/*.cc' 'sources/*.h' )

git ls-tree -r --name-only "$audit" -- sources |
  awk '
    /\.c$/ { c++ } /\.cc$/ { cc++ } /\.h$/ { h++ }
    END { printf "%d .c, %d .cc, %d .h, %d total\n", c,cc,h,c+cc+h }
  '

patterns=(
  '#[' '#]' 'sizeof(' 'sizeof (' 'WORD *' 'WORD* ' '(WORD *)' '(WORD*)'
  'MesPrint(' 'MesPrint (' 'if ( ' 'if(' 'while ( ' 'while('
  'switch ( ' 'switch(' 'for ( ' 'for(' 'for(;;)' 'return(' 'return ('
)
for pattern in "${patterns[@]}"; do
  count=$(git grep -l -F "$pattern" "$audit" -- \
    "${source_paths[@]}" | wc -l)
  printf '%-12s %s\n' "$pattern" "$count"
done

for fold_audit in v5.0.1 v4.3.1; do
  total=$(git ls-tree -r --name-only "$fold_audit" -- sources |
    awk '/\.(c|cc|h)$/ { n++ } END { print n+0 }')
  both=$(comm -12 \
    <(git grep -l -F '#[' "$fold_audit" -- "${source_paths[@]}" | sort) \
    <(git grep -l -F '#]' "$fold_audit" -- "${source_paths[@]}" | sort) |
    wc -l)
  printf '%s both fold markers: %s/%s files\n' "$fold_audit" "$both" "$total"
done

for pattern in '@file' 'Copyright'; do
  count=$(git grep -l -F "$pattern" "$audit" -- \
    "${source_paths[@]}" | wc -l)
  printf '%-12s %s files\n' "$pattern" "$count"
done
```

## Evidence summary

| Feature | Files at `v5.0.1` | Interpretation |
|---|---:|---|
| opening fold marker `#[` | 86/86 files | source-tree-wide, documented |
| closing fold marker `#]` | 86/86 files | source-tree-wide, documented |
| `sizeof(` | 60 files | strong canonical form |
| `sizeof (` | 0 files | strong negative evidence |
| `WORD *` | 63 files | canonical pointer alignment |
| `WORD* ` | 8 files | local/modern C++ deviations exist |
| `(WORD *)` | 48 files | canonical C pointer-cast form |
| `(WORD*)` | 9 files | minority/local variation |
| `MesPrint(` | 63 files | function name immediately followed by `(` |
| `MesPrint (` | 4 files | concentrated in local C++ style |
| opening-padded `if` | 66 files | strong core rule |
| compact `if(` | 22 files | mostly macros/local; not canonical ordinary C |
| opening-padded `while` | 57 files | strong core rule |
| compact `while(` | 12 files | macros/legacy/local variation |
| opening-padded `switch` | 36 files | strong core rule |
| compact `switch(` | 8 files | minority/local variation |
| opening-padded `for` | 58 files | canonical ordinary loop |
| compact `for(` | 30 files | initially looks inconsistent |
| exact `for(;;)` | 25 files | explains most compact `for` files as an idiom |
| `return(` | 57 files | strongly established historical house form |
| `return (` | 11 files | minority form, not canonical |

These counts are useful primarily for distinguishing broad patterns from
localized exceptions.

### Historical comparison

Setting `audit=v4.3.1` in the fragment above reproduces the historical
comparison. Selected pairs show the same direction at both releases:

| Fixed-string pair | `v4.3.1` | `v5.0.1` |
|---|---:|---:|
| `sizeof(` / `sizeof (` | 55 / 0 files | 60 / 0 files |
| `WORD *` / `WORD* ` | 57 / 3 files | 63 / 8 files |
| opening-padded `if` / compact `if(` | 59 / 21 files | 66 / 22 files |
| `return(` / `return (` | 54 / 8 files | 57 / 11 files |

These values show that the selected patterns predate the audit baseline;
`v4.3.1` is supporting historical evidence, not a second normative baseline.
It contains 78 audited files, of which 75 have both fold markers, so the 86/86
source-tree-wide fold finding is specifically a `v5.0.1` result.

## Indentation and patch hygiene (CS-INDENTATION)

Literal tabs are pervasive in both `v4.3.1` core code and maintained code at the
audit baseline. The `v5.0.1` `features.cc`, for example, uses tabs for block
indentation while preserving the characteristic FORM whitespace inside control
statements and calls.

No `.editorconfig` or `.clang-format` defining a visual tab width is present at
the audit baseline. Therefore the rule specifies **literal tabs for
indentation** but does not invent a tab width. Continuation alignment is less
rigid: maintained code uses combinations of tabs and spaces to align long
conditions and calls.

The FORM developer Wiki as consulted for this audit warns that the source has a
unique coding style and that applying a generic formatter can cause massive
undesired changes. It also recommends disabling trailing-whitespace trimming
because fold state can depend on it.

Preserving LF line endings and the final newline is patch hygiene rather than a
new visual convention: changing either can create whole-file churn or malformed
text without improving the code.

Decision: tabs for blocks; spaces may assist alignment; do not run generic
formatting or create style-only churn in unrelated code.

## Braces (CS-BRACES)

The dominant core pattern is:

```c
int Function(...)
{
	if ( condition ) {
		...
	}
	else {
		...
	}
}
```

This is visible in the `v4.3.1` core (`argument.c`, `pre.c`, `normal.c`,
`sort.c`, `startup.c`, and `tools.c`) and in `v5.0.1` `features.cc`.

Known deviations such as `flintwrap.cc` put function opening braces on the same
line, but the same files also differ in comma spacing and pointer alignment.
They are better understood as coherent subsystem-local style rather than
evidence for two equally canonical FORM brace rules. At `v5.0.1`, struct, class,
and namespace declarations commonly keep `{` on the declaration line, so the
function-specific next-line rule should not be generalized to those constructs.

Decision: next-line function brace; same-line control brace; `else` on a new
line.

## Control parentheses and `sizeof` (CS-PARENTHESES)

The `v4.3.1` core repeatedly shows the visual distinction:

```c
if ( Generator(BHEAD factor,AR.Cnumlhs) ) {
	...
}
```

The outer control parentheses are padded while the inner call is compact.
`v5.0.1` `features.cc` preserves the same control syntax.

`sizeof` is unusually clean evidence: `sizeof(` occurs in 60 audit-baseline
source files while `sizeof (` occurs in 0. The historical comparison is also
55/0, including macro-heavy code.

Decision: padded control parentheses; compact `sizeof(...)`.

## Function and macro calls (CS-CALLS)

Core C routinely uses calls such as:

```c
LocateFile(&name,type)
SeekFile(handle,&scrpos,SEEK_END)
ReadFile(handle,buffer,filesize)
MesPrint("%s",buffer)
Generator(BHEAD factor,AR.Cnumlhs)
```

The audit-baseline `features.cc` preserves compact call commas even for
standard-library calls:

```cpp
std::fill(col_lens.begin(),col_lens.begin()+n_cols,0);
std::max(col_lens[j],lines[k].size());
MesPrint("%s",line.c_str());
```

The audit-baseline comparison using the common `MesPrint` call found `MesPrint(`
in 63 files and `MesPrint (` in only 4, with the minority concentrated in C++
subsystems already known to use local style.

Decision: function names are immediately followed by `(`, call parentheses are
compact, and call arguments use no space after commas.

## Function parameter lists (CS-PARAMETERS)

Parameter lists are less uniform than calls. Examples with a space after the
parameter comma include:

```c
WORD execarg(PHEAD WORD *term, WORD level)
int DoTail(int argc, UBYTE **argv)
static inline int ConsumeOption(UBYTE **s, const char *opt)
```

Older core examples without the space also exist, and old prototypes in
`declare.h` are often very compact.

Decision: normalize **new** parameter lists to `, ` while preserving old compact
declarations when they are not otherwise rewritten. This is a chosen
normalization backed by maintained `v5.0.1` code, not a claim that every
historical declaration followed it.

## Pointers, references, and casts (CS-POINTERS)

Core C declarations use `WORD *term`, `UBYTE *buffer`, and `FILES **filelist`.
Audit-baseline counts are 63 files with `WORD *` versus 8 with `WORD* `.

Core pointer casts use `(WORD *)ptr` and `(UBYTE *)buffer`; representative
counts are 48 files with `(WORD *)` versus 9 with `(WORD*)`.

Audit-baseline C++ pointer and reference alignment is mixed. For references,
`features.cc` uses `const T& name`, while `flintinterface.cc` commonly uses
`const T &name`. The latter file also mixes `WORD*` return types with `WORD *`
parameters. No C++ form is uniform. The chosen normalization keeps C++ pointer
and reference declarators consistent with canonical FORM C rather than
introducing different alignment for `*` and `&`.

Decision: C and C++ pointers use `TYPE *name`; C pointer casts use
`(TYPE *)expr`; C++ references use `T &name`. Existing subsystem-local C++
alignment remains untouched.

## Operators (CS-OPERATORS)

Core code strongly spaces assignment, comparison, and logical operators, but
arithmetic is mixed. Compact arithmetic is frequent in forms such as
`cbuf+AM.rbufnum`, `start+1`, `n*sizeof(WORD)`, and `fleft+FUNHEAD`, while
spaces are also used in larger expressions to clarify grouping.

There is no evidence for a simple universal rule such as "always space `+`" or
"never space `+`".

Decision: make assignment/comparison/logical spacing normative, keep unary
operators attached, and leave arithmetic spacing context-sensitive rather than
letting a formatter impose a blanket rule.

## Control flow (CS-CONTROL-FLOW)

A naive search initially suggests inconsistent `for` spacing: padded
`for ( ... )` occurs in 58 files, while compact `for(` occurs in 30. But the
exact idiom `for(;;)` alone occurs in 25 files. Most compact `for(` evidence
therefore reflects a specific infinite-loop idiom rather than random spacing
drift.

Short one-line constructs such as `if ( condition ) return(0);` are common in
core code, and audit-baseline `features.cc` uses
`if ( n_lines == 0 ) return;`. Old code sometimes packs multiple statements
onto one line, but that density is not necessary to preserve the house style.

Representative core `switch` statements indent `case` and `default` one level
inside the `switch`, with multi-line bodies one level further. This supports the
guide's canonical layout without treating densely packed legacy cases as the
preferred form.

Decision: ordinary loops use `for ( ... )`; infinite loops may use `for(;;)`;
genuinely simple one-line control bodies are acceptable; multi-step logic should
not be packed onto one line.

## Return statements (CS-RETURNS)

Parenthesized return values are widespread in the core:

```c
return(0);
return(-1);
return(value);
```

The audit-baseline search found `return(` in 57 files. The `v5.0.1` `comtool.h`
still contains `return(1);` and `return(0);`, so this is not purely dead
historical syntax.

PR #764 distinguished removed pre-C89/K&R function declaration syntax from other
K&R-derived coding style and mentioned `return(0);` as an example of the latter.

Decision: retain `return(value);` in the canonical guide for now. A change to
`return value;` should be an explicit project-wide policy decision rather than
an incidental modernization.

## Source folds (CS-FOLDS)

Every audited C/C++/header file in `sources/` at `v5.0.1` contains both opening
and closing STedi-style fold markers. This makes folds stronger evidence than
most whitespace rules: they are both source-tree-wide at the audit baseline and
explicitly documented.

The FORM developer reference says that source files are structured by "so-called
folds" and recommends `set foldmarker=#[,#]` for vi-compatible editors. The
developer Wiki as consulted for this audit gives the marker grammar, explains
that a closed fold can be encoded by a trailing space on the closing fold line,
and recommends disabling automatic trailing-whitespace trimming. The STedi
manual documents matching labels, the arbitrary three-character prefix, and
nesting.

Core C files provide the canonical saved-state pattern. For example,
`compiler.c` and `execute.c` leave first-level code sections open while closing
nested function folds. Differing states in newer C++ files are local variation,
not evidence for a second canonical rule. Saved state is not determined by the
comment wrapper or source language; new folds follow the local file and
hierarchy.

Decision: folds are first-class source metadata. Agents must not flatten,
rename, or trim them as incidental formatting. New folds follow the surrounding
wrapper, nesting, and saved-state convention.

## Doxygen and comments (CS-DOCUMENTATION)

The audit-baseline source contains multiple comment eras. Doxygen `@file`
comments occur in 78 of 86 audited files, many maintained functions have Doxygen
documentation, core implementation comments commonly use `/* ... */`, and C++
code also uses `//` for local comments.

There is insufficient evidence for one universal implementation-comment syntax,
so converting old comments would create noise.

Doxygen comments should be immediately adjacent to the declaration or definition
they document. Microsoft's `vscode-cpptools` issue #7033 concerns a macro placed
between a documentation comment and a function. It shows that intervening syntax
can prevent comment association, but does not establish that a blank line alone
has the same effect. Adjacency is adopted as a clear project convention, not as
a workaround for a demonstrated blank-line bug.

Direction tags such as `@param[in]`, `@param[out]`, and `@param[in,out]` are
established in audit-baseline files including `compiler.c`, `execute.c`,
`mpi.c`, and `parallel.c`. They convey information not available from a C
signature and therefore support using a direction only when it is meaningful.

Decision: use useful Doxygen for new files and non-trivial/public functions,
avoid boilerplate, keep the doc comment directly adjacent to its target, and
preserve local implementation-comment syntax.

Reference: https://github.com/microsoft/vscode-cpptools/issues/7033

## Copyright and license metadata (CS-LICENSE)

Copyright/license metadata is unusually uniform at the audit baseline. Searches
found a copyright notice in all 52 `.c`, all 11 `.cc`, and all 23 `.h` files
under `sources/` (86/86 total). Ordinary FORM source files use a `License` fold
containing the standard GPL notice and, at `v5.0.1`, the line
`Copyright (C) 1984-2026 J.A.M. Vermaseren`.

The repository contains `scripts/update-copyright.sh`, which updates recognized
notices project-wide to the year in which it runs. At the audit baseline it
recognizes the standard FORM notice and the separately attributed T. Kaneko
notice. The GRCC sources (`grcc.cc`, `grcc.h`, `grccparam.h`) demonstrate why
copyright should not be normalized blindly: they carry a distinct notice and
publication request.

Decision: new ordinary FORM C/C++ source/header files receive the maintained
standard `License` fold by copying a nearby maintained file; attribution is not
invented or modified by coding agents; distinct notices are preserved; year
updates use the project-wide mechanism rather than incidental patches.

## Preprocessor and macros (CS-PREPROCESSOR)

Historical headers contain very dense macros and compressed syntax. Macro bodies
therefore should not be used as the primary evidence for ordinary statement
formatting.

Decision: use compact function-like macro calls, preserve existing continuation
layout when touching old macros, and avoid formatting-only rewrites of large
macro bodies.

## Line wrapping and continuations (CS-WRAPPING)

No committed formatter or source-wide fixed line-length rule was found at the
audit baseline. The `v4.3.1` reference and `v5.0.1` baseline both use
combinations of tabs and spaces for long conditions and calls; operator-leading
logical continuation is common.

Decision: preserve grouping, use tabs for block indentation and spaces where
useful for alignment, break calls at argument boundaries when necessary, and do
not rewrap unrelated nearby code.

## C++ (CS-CPP)

C++ is the largest source of apparent style fragmentation. Audit-baseline
`features.cc` closely follows core FORM layout: next-line function braces, tab
indentation, padded control parentheses, compact calls and call commas, and
source folds.

`flintwrap.cc` and polynomial C++ files contain coherent subsystem-local
variations such as same-line function braces, spaces after call commas, and
different pointer/reference alignment.

Decision: new C++ follows FORM-wide structural whitespace and declarator
alignment where applicable instead of perpetuating subsystem-specific drift.

## Naming (CS-NAMING)

Naming is not sufficiently uniform to infer a single new scheme without creating
a policy that goes beyond formatting. The repository contains PascalCase core
routines, lower-case historical routines, subsystem-specific C++ names, and
long-established public symbols.

Decision: do not invent a global naming migration. Preserve existing APIs and
match the subsystem when adding a closely related symbol.

## Legacy code and local variation (CS-LEGACY)

The `v4.3.1` historical reference and `v5.0.1` audit baseline both contain
legacy density, subsystem-local choices, and isolated deviations. A style guide
intended for coding agents must not turn those deviations into a second accepted
style, but it must also avoid creating unrelated style-only diffs.

Decision: canonical style applies to new and substantially rewritten code;
untouched legacy formatting is preserved; local deviations do not redefine
repository-wide style.

Here, substantial rewriting means replacing or reorganizing the implementation
or structure of a function or another construct. A several-line edit does not
justify reformatting that construct, and the rule never extends to adjacent
unchanged code. This construct-level boundary preserves reviewability and keeps
style conformance from becoming a pretext for broad cleanup.

## Confidence classification

### Documented / effectively certain

- STedi fold markers and nesting are structural metadata.
- fold labels must match according to STedi syntax.
- intentional trailing whitespace must not be globally trimmed.
- generic auto-formatting is unsafe for FORM source.

### Strong canonical pattern

- tabs for block indentation;
- next-line opening brace for function definitions;
- same-line brace for control statements;
- `else` on a new line;
- padded control parentheses;
- compact call parentheses;
- compact commas in function calls;
- `sizeof(...)`;
- C pointer declarations as `TYPE *name`;
- C pointer casts as `(TYPE *)expr`;
- `case` labels one level inside `switch`;
- common Doxygen `@file` headers;
- ordinary `for ( ... )` plus the special `for(;;)` idiom.

### Chosen normalization where history is mixed

- spaces after commas in new function parameter lists;
- standard multi-line formatting for new functions rather than old single-line
  function bodies;
- declarator-attached C++ pointers and references;
- applying core FORM structural style to new C++ instead of copying local C++
  subsystem drift.

### Deliberately not over-specified

- universal arithmetic-operator spacing;
- one global naming scheme;
- one implementation-comment syntax;
- one exact continuation alignment formula;
- visual tab width.

## Primary references

- FORM repository: https://github.com/form-dev/form
- Audit source baseline:
  https://github.com/form-dev/form/tree/v5.0.1/sources
- Developer reference source overview:
  https://github.com/form-dev/form/blob/v5.0.1/doc/devref/source.tex
- VS Code Tips for FORM Developers:
  https://github.com/form-dev/form/wiki/VS-Code-Tips-for-FORM-Developers
- STedi fold specification:
  https://www.nikhef.nl/~form/maindir/others/stedi2/stedihtml/node30.html
- FORM formatter discussion:
  https://github.com/form-dev/form/discussions/712
- C23/K&R cleanup PR #764:
  https://github.com/form-dev/form/pull/764
- Historical comparison source:
  https://github.com/form-dev/form/tree/v4.3.1/sources
- Audit-baseline `features.cc`:
  https://github.com/form-dev/form/blob/v5.0.1/sources/features.cc
- Audit-baseline `comtool.h`:
  https://github.com/form-dev/form/blob/v5.0.1/sources/comtool.h
- VS Code C/C++ doc-comment adjacency issue #7033:
  https://github.com/microsoft/vscode-cpptools/issues/7033
- Local-style counterexample `flintwrap.cc`:
  https://github.com/form-dev/form/blob/v5.0.1/sources/flintwrap.cc

# FORM C/C++ Coding Style

Canonical style for new and substantially rewritten C/C++ code in FORM. Existing
source contains legacy and subsystem-specific variations; do not copy an
isolated deviation into new code, and do not reformat unrelated old code merely
to make it conform.

This guide applies to project-maintained C/C++ code, primarily under `sources/`.
For vendored or imported code such as `extern/`, preserve and follow the
upstream or component-local style instead.

Rule IDs such as `CS-BRACES` are **cross-reference labels only**. They do not
indicate priority, severity, or application order.

## Indentation and patch hygiene (CS-INDENTATION)

- Indent blocks with **tabs**. Use spaces only where useful for continuation
  alignment.
- Do not run a generic auto-formatter over FORM source files.
- Preserve LF line endings, the final newline, folds, and intentional trailing
  whitespace.
- Keep style-only changes out of unrelated functional patches.

## Braces (CS-BRACES)

Function opening braces go on the next line:

```c
int Function(int a, int b)
{
	...
}
```

Control-statement braces stay on the same line, and `else` starts on a new line:

```c
if ( condition ) {
	...
}
else {
	...
}
```

The same applies to `for`, `while`, and `switch`. For structs/classes/namespaces
and similar non-function constructs, keep the opening brace on the same line
unless the local construct requires otherwise.

## Control parentheses and `sizeof` (CS-PARENTHESES)

Whitespace distinguishes control syntax from calls. Use padded parentheses for
control statements:

```c
if ( condition )
for ( init; condition; increment )
while ( condition )
switch ( expression )
```

Write `sizeof` in compact form:

```c
sizeof(WORD)
n*sizeof(WORD)
```

## Function and macro calls (CS-CALLS)

Use compact parentheses and **no space after commas** in function calls and
function-like macros:

```c
Function(arg)
Function(arg1,arg2,arg3)
MACRO(arg1,arg2)
MesPrint("%d",value)
std::max(a,b)
```

Thus this is canonical FORM style:

```c
if ( Generator(BHEAD factor,AR.Cnumlhs) ) {
	...
}
```

Do not write `Function (arg)`, `Function( arg )`, or ordinary calls as
`Function(a, b)`.

Deliberately aligned tables/initializers may keep their local alignment.

## Function parameter lists (CS-PARAMETERS)

For new function declarations and definitions, use a space after parameter
commas:

```c
int Function(int a, int b)
static inline int ConsumeOption(UBYTE **s, const char *opt)
```

Older declarations sometimes use `Function(int a,int b)`; preserve them when
untouched, but do not use that form for new parameter lists.

## Pointers, references, and casts (CS-POINTERS)

Pointer declarations in canonical FORM C and C++ attach `*` to the declarator:

```c
WORD *p;
UBYTE *buffer;
FILES **filelist;
WORD *p, *q;
```

Prefer this over `WORD* p`.

Use compact C casts with pointer spacing inside the type:

```c
(WORD *)ptr
(UBYTE *)buffer
```

For C++ references, likewise attach `&` to the declarator:

```cpp
const T &value
const auto &item
```

## Operators (CS-OPERATORS)

Use spaces around assignment, comparison, and logical binary operators:

```c
a = b;
if ( a == b && c != d ) {
	...
}
```

Keep unary operators attached to their operand: `*p`, `&x`, `!flag`, `p++`,
`--i`.

Arithmetic spacing is intentionally less rigid. Compact pointer/index arithmetic
is idiomatic:

```c
p+1
n-1
FUNHEAD+1
n*sizeof(WORD)
```

Spaces are also used in longer expressions when they clarify grouping. Do not
mechanically normalize all `+`, `-`, `*`, `/`, or `%` spacing.

## Control flow (CS-CONTROL-FLOW)

Ordinary loops use the padded form:

```c
for ( i = 0; i < n; i++ ) {
	...
}
```

The compact infinite-loop idiom is established:

```c
for(;;) {
	...
}
```

A genuinely simple one-line body is acceptable:

```c
if ( error ) return(-1);
if ( done ) break;
```

Do not pack multi-step logic onto one line merely to save space.

Indent `case` labels one level inside a `switch`, and their bodies one level
further:

```c
switch ( value ) {
	case 0:
		HandleZero();
		break;
	default:
		break;
}
```

## Return statements (CS-RETURNS)

The established FORM form for a non-void return is:

```c
return(0);
return(-1);
return(value);
```

Use `return;` for a void return. Do not write `return (value);`.

A project-wide change to modernize this convention should be made explicitly,
not piecemeal inside unrelated patches.

## Source folds (CS-FOLDS)

STedi-style folds are **source structure**, not decorative comments. Preserve
`#[` / `#]` markers, labels, nesting, boundaries, and intentional trailing
whitespace.

Typical function fold in C, saved closed:

```c
/*
  	#[ FunctionName :
*/

int FunctionName(int arg)
{
	...
}

/*
  	#] FunctionName : 
*/
```

Some C++ files use line-comment wrappers; this example is saved closed:

```cpp
//	#[ Section :
...
//	#] Section : 
```

Saved fold state is independent of the comment wrapper and source language.

Rules:

- Opening and closing labels must match through the terminating `:`, except for
  `[` versus `]`.
- Never remove, flatten, or rename folds as incidental cleanup.
- When adding a substantial function/section to a file whose peers are folded,
  add the corresponding fold using that file's wrapper and nesting style.
- Match the saved fold state to the surrounding hierarchy. First-level code or
  section folds are commonly open, while inner function folds are commonly
  closed; preserve local convention rather than closing every new fold.
- The three characters before `#` are deliberately not fixed by STedi. Preserve
  an existing fold marker's prefix byte for byte, including its exact mixture
  of spaces and tabs, unless the task explicitly changes the fold structure.
- Preserve trailing whitespace on an existing closing fold marker byte for
  byte unless the task explicitly changes its saved fold state. A trailing
  blank can encode the saved closed state of the fold.
- A whitespace warning on a fold-marker line is not authorization to alter it.

## Doxygen and comments (CS-DOCUMENTATION)

New files should normally begin with a useful Doxygen `@file` comment. Document
public or non-trivial functions when the documentation adds information beyond
the signature.

**Place a Doxygen comment immediately before the declaration or definition it
documents, with no blank line between them.** This keeps the association
visually unambiguous.

```c
/**
 * Brief description.
 *
 * @param[in]  arg  Description.
 * @return          Description.
 */
int Function(int arg)
{
	...
}
```

Do not write:

```c
/**
 * Brief description.
 */

int Function(int arg)
```

Use `@param[in]`, `@param[out]`, or `@param[in,out]` when that direction is
meaningful. Do not add boilerplate Doxygen to trivial helpers merely to restate
their names or signatures.

For implementation comments, preserve the file's established syntax. Core C
commonly uses `/* ... */`; newer C++ also uses `// ...`. Do not convert comment
syntax without a reason.

Do not merge descriptive comments with fold markers in a way that damages fold
recognition.

## Copyright and license metadata (CS-LICENSE)

Every C/C++ source and header in `sources/` carries a `License` fold with a
copyright notice. Treat that block as legal/project metadata, not ordinary prose
or formatting.

- For a new ordinary FORM source/header, copy the current standard `License`
  fold from a nearby maintained FORM source file; do not recreate the legal text
  from memory.
- Do not add, remove, or replace copyright holders merely because a contributor
  created or modified code. Change copyright attribution only as an explicit
  project/legal decision.
- Preserve distinct notices and attribution for imported or separately
  copyrighted code.
- Do not bump copyright years as incidental cleanup. The repository provides
  `scripts/update-copyright.sh` for the project-wide year update.
- Do not reformat the license block or its fold markers.

## Preprocessor and macros (CS-PREPROCESSOR)

Macro-heavy legacy code is often denser than ordinary C/C++. Do not treat old
macro bodies as the primary precedent for normal code formatting.

Use compact function-like macro calls (`MACRO(a,b)`) and preserve existing
preprocessor continuation layout when modifying old macros. Avoid
formatting-only rewrites of large macros.

## Line wrapping and continuations (CS-WRAPPING)

No single fixed source line length is established. When wrapping:

- preserve syntactic grouping;
- keep tabs for block indentation;
- use spaces where useful for continuation alignment;
- operator-leading continuation is a good default for long logical conditions;
- break long calls at argument boundaries without changing compact call-comma
  spacing;
- do not rewrap unrelated nearby code.

Example:

```c
if ( first_condition
	 && second_condition
	 && third_condition ) {
	...
}
```

## C++ (CS-CPP)

New C++ code follows the same FORM-wide structural rules where applicable:

- tabs for block indentation;
- function braces on the next line;
- control braces on the same line;
- padded control parentheses;
- compact calls and compact call commas;
- declarator-attached pointers and references;
- source folds;
- no unrelated auto-formatting.

Existing C++ subsystems contain local variations (same-line function braces,
spaces after call commas, different pointer alignment). Treat these as
historical/subsystem-local unless a technical reason requires them.

## Naming (CS-NAMING)

The repository has several naming eras. Do not invent a repository-wide renaming
scheme as part of formatting work.

- Preserve existing API names.
- Match the subsystem when adding a closely related symbol.
- Do not rename symbols solely for stylistic consistency in an unrelated change.

## Legacy code and local variation (CS-LEGACY)

When canonical style conflicts with nearby legacy formatting:

- use canonical style for new or substantially rewritten code;
- leave untouched legacy formatting alone;
- do not create broad style-only diffs;
- do not infer a second accepted style from an isolated local deviation.

Here, "substantially rewritten" means that the implementation or structure of a
function or other construct is replaced or reorganized. Editing several lines
does not qualify, and adjacent unchanged code must not be reformatted.

## Canonical function excerpt

This is a function-level excerpt. A new file should also include the `@file`
comment and must include the `License` fold described above.

```c
/*
  	#[ ProcessItems :
*/

/**
 * Processes an array of items.
 *
 * @param[in]  items  Items to process.
 * @param[in]  n      Number of items.
 * @return            0 on success, or -1 if the input is invalid.
 */
int ProcessItems(WORD *items, WORD n)
{
	WORD i;
	WORD *buffer;

	if ( !items || n <= 0 ) return(-1);

	buffer = (WORD *)Malloc1(n*sizeof(WORD),"ProcessItems");

	for ( i = 0; i < n; i++ ) {
		if ( CheckItem(items+i) ) {
			CopyItem(buffer+i,items+i);
		}
		else {
			ClearItem(buffer+i);
		}
	}

	M_free(buffer,"ProcessItems");
	return(0);
}

/*
  	#] ProcessItems : 
*/
```

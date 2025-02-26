import io
import re
import sys
from pathlib import Path
from typing import Dict, Optional, Union

import pcpp
import pycparser

FORM3_INCLUDES = """

#if defined(ILP32)

typedef short WORD;
typedef long LONG;
typedef unsigned short UWORD;
typedef unsigned long ULONG;
#define BITSINWORD 16
#define BITSINLONG 32
#define INT16 short
#define INT32 int
#define INT64 long long
#undef INT128

typedef off_t INT32

#elif defined(LLP64)

typedef int WORD;
typedef long long LONG;
typedef unsigned int UWORD;
typedef unsigned long long ULONG;
#define BITSINWORD 32
#define BITSINLONG 64
#define INT16 short
#define INT32 int
#define INT64 long long
#undef INT128

typedef off_t INT64

#elif defined(LP64)

typedef int WORD;
typedef long LONG;
typedef unsigned int UWORD;
typedef unsigned long ULONG;
#define BITSINWORD 32
#define BITSINLONG 64
#define INT16 short
#define INT32 int
#define INT64 long
#undef INT128

typedef off_t INT64

#else
#error ILP32 or LLP64 or LP64 must be defined!
#endif

"""


def preprocess(
    filename: str, content: str, defs: Optional[Dict[str, Union[str, int]]] = None
) -> str:
    header = ""
    if defs:
        for name, value in defs.items():
            header += f"#define {name} {value}\n"
    content = header + content

    cpp = pcpp.preprocessor.Preprocessor()

    buf = io.StringIO()
    cpp.parse(content, filename)
    cpp.write(buf)
    return buf.getvalue()


def process_file(filename: str) -> None:
    content = Path(filename).read_text()

    # Check if it contains PADXXX macros.
    if not re.search(r"\bPAD", content):
        return

    # debug
    if filename != "sources/structs.h":
        return

    content = preprocess(filename, content, {"UNIX": 1, "LP64": 1})
    print(content)

    parser = pycparser.c_parser.CParser()
    ast = parser.parse(content, filename=filename)
    generator = pycparser.c_generator.CGenerator()
    print(generator.visit(ast))


def main() -> None:
    for f in sys.argv[1:]:
        process_file(f)


if __name__ == "__main__":
    main()

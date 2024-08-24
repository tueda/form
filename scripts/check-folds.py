import argparse
from dataclasses import dataclass

@dataclass(frozen=True)
class Fold:
    lineno: int
    label: str

def check_file(filename: str, fix: bool) -> None:
    fixed = False
    lineno = 0
    folds = []  # as a stack
    pass

def main() -> None:
    """Main entry point."""
    parser = argparse.ArgumentParser(usage='%(prog)s [options] files..')
    parser.add_argument(
        '-f', '--fix', action='store_true', default=False,
        help='close all folds'
    )
    parser.add_argument('filenames', nargs='*', help=argparse.SUPPRESS)
    args = parser.parse_args()
    files = args.filenames
    if not files:
        parser.print_help()
    else:
        for f in files:
            check_file(f, args.fix)

if __name__ == '__main__':
    main()

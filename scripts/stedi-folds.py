import argparse

try:
    from typing import TYPE_CHECKING
except ImportError:
    TYPE_CHECKING = False
if TYPE_CHECKING:
    from typing import Optional, Sequence  # noqa: F401


def main(args=None):
    # type: (Optional[Sequence[str]]) -> None

    parser = argparse.ArgumentParser(
        usage=("%(prog)s [options] [--] " "[par=val].. [par+=int].. [par*=float].."),
        add_help=False,
    )
    opts = parser.parse_args(args=args)
    print(opts)

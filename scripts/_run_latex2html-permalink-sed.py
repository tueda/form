import logging as _logging
import re
import sys

_logging.basicConfig(level=_logging.INFO, format="%(name)s %(levelname)7s %(message)s")
logger = _logging.getLogger("latex2html-permalink")

with open(sys.argv[1], "r") as f:
    s = f.read()

d = {}
d_inv = {}

# TODO: existing IDs???

a = re.findall("<A .*?</A>", s, re.DOTALL)
a = [s for s in a if "SECTION" in s]
for s in a:
    s = " ".join(ss.strip() for ss in s.splitlines())

    m = re.search(r"HREF=\"#(SECTION.*?)\"", s)
    if m:
        name = m.group(1)
    else:
        m = re.search(r"ID=\"(SECTION.*?)\"", s)
        if m:
            name = m.group(1)
        else:
            continue

    s = re.sub("<A .*?>", "", s)
    s = re.sub("</A>", "", s)
    s = re.sub("<SPAN .*?>", "", s)
    s = re.sub("</SPAN>", "", s)
    s = re.sub(r"^\s*\d+\.(?:\d+)?(?:\.\d+)?(?:\.\d+)?\s+", "", s)
    for k, v in {
        "&nbsp;": "",
        "#": "num",  # &#35;
        "'": "apos",  # &#39;
        "(": "lpar",  # &#40;
        ")": "rpar",  # &#41;
        ",": "",  # &#44;
        "/": "sol",  # &#47;
        ":": "",  # &#58;
        ";": "",  # &#59;
        "<": "lt",  # &#60;
        ">": "gt",  # &#62;
        "[": "lbrack",  # &#91;
        "]": "rbrack",  # &#93;
    }.items():
        s = s.replace(k, f" {v} ")
    s = re.sub(r"\s+", " ", s)
    s = s.strip()
    s = s.replace(" ", "_")
    s = s.lower()
    for v in ["a", "an", "the"]:
        s = re.sub(f"^{v}_", "", s)
        s = s.replace(f"_{v}_", "_")
    if not s:
        continue

    if re.search("[^0-9A-Za-z_]", s):
        logger.warning(f'section "{s}" not alphanumeric')

    if name in d.keys():
        if s != d[name]:
            logger.warning(f'id "{name}" found more than once')
            logger.warning(f"previous: {name}={d[name]}")
            logger.warning(f"ignore: {name}={s}")
    else:
        if s in d.values():
            s0 = s
            i = 1
            while True:
                s = f"{s0}_{i}"
                if s not in d.values():
                    break
                i += 1
            logger.info(f'section "{s0}" found more than once')
            logger.info(f"previous: {d_inv[s0]}={s0}")
            logger.info(f"renamed: {name}={s}")
        d[name] = s
        d_inv[s] = name

for k, v in d.items():
    print(f's/{k}"/{v}"/g')

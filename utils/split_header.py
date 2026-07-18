#!/usr/bin/env python3
"""
split_headers.py — splits C++ .h files into declarations (.h) and implementations (.cpp).

Usage:
    python3 split_headers.py <directory>          # dry-run
    python3 split_headers.py <directory> --apply  # apply changes

Rules:
    - Moves method bodies out of .h into .cpp
    - Leaves only clean declarations in .h
    - Constructor init-list stays in .cpp only (removed from .h declaration)
    - Strips 'override'/'final' from .cpp signatures
    - Normalizes body indentation to 4 spaces
    - Skips inline/template/constexpr — they must stay in .h
    - Skips methods already defined in .cpp (no redefinition errors)
    - Only processes content up to the first #include after the class (guards against
      concatenated .h + .cpp files)
    - Analyses #includes and moves cpp-only ones to .cpp, keeps shared ones in .h
"""

import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional

# ── Config ────────────────────────────────────────────────────────────────────

KEEP_IN_HEADER = {"inline", "template", "constexpr", "consteval", "constinit"}

CONTROL_FLOW = {
    "if", "else", "for", "while", "do", "switch",
    "try", "catch", "namespace", "extern", "typedef", "using",
}


# ── Tokenizer ─────────────────────────────────────────────────────────────────

@dataclass
class Token:
    kind: str  # 'word', 'punct', 'string', 'comment', 'space', 'newline', 'pp'
    text: str
    pos: int  # byte offset in original source


def tokenize(src: str) -> list[Token]:
    tokens: list[Token] = []
    i = 0
    n = len(src)
    while i < n:
        c = src[i]

        # Block comment
        if src[i:i + 2] == "/*":
            j = src.find("*/", i + 2)
            end = j + 2 if j != -1 else n
            tokens.append(Token("comment", src[i:end], i));
            i = end;
            continue

        # Line comment
        if src[i:i + 2] == "//":
            j = src.find("\n", i + 2)
            end = j if j != -1 else n
            tokens.append(Token("comment", src[i:end], i));
            i = end;
            continue

        # Preprocessor
        if c == "#":
            j = i
            while j < n:
                if src[j] == "\n" and (j == 0 or src[j - 1] != "\\"):
                    break
                j += 1
            tokens.append(Token("pp", src[i:j], i));
            i = j;
            continue

        # String/char literal
        if c in ('"', "'"):
            j = i + 1
            while j < n:
                if src[j] == "\\": j += 2; continue
                if src[j] == c:    j += 1; break
                j += 1
            tokens.append(Token("string", src[i:j], i));
            i = j;
            continue

        # Newline
        if c == "\n":
            tokens.append(Token("newline", "\n", i));
            i += 1;
            continue

        # Whitespace
        if c in (" ", "\t", "\r"):
            j = i
            while j < n and src[j] in (" ", "\t", "\r"): j += 1
            tokens.append(Token("space", src[i:j], i));
            i = j;
            continue

        # Identifier / keyword
        if c.isalpha() or c == "_":
            j = i
            while j < n and (src[j].isalnum() or src[j] == "_"): j += 1
            tokens.append(Token("word", src[i:j], i));
            i = j;
            continue

        # Number
        if c.isdigit():
            j = i
            while j < n and (src[j].isalnum() or src[j] in (".", "_")): j += 1
            tokens.append(Token("punct", src[i:j], i));
            i = j;
            continue

        # Two-char operators
        two = src[i:i + 2]
        if two in ("::", "->", "&&", "||", "==", "!=", "<=", ">=",
                   "<<", ">>", "++", "--", "+=", "-=", "*=", "/="):
            tokens.append(Token("punct", two, i));
            i += 2;
            continue

        # Single char
        tokens.append(Token("punct", c, i));
        i += 1

    return tokens


def skip_ws(tokens: list[Token], idx: int) -> int:
    while idx < len(tokens) and tokens[idx].kind in ("space", "newline", "comment"):
        idx += 1
    return idx


def find_matching(tokens: list[Token], open_idx: int, open_ch: str, close_ch: str) -> int:
    depth = 0
    i = open_idx
    while i < len(tokens):
        k = tokens[i].kind
        if k in ("string", "comment", "pp"):
            i += 1;
            continue
        if tokens[i].text == open_ch:
            depth += 1
        elif tokens[i].text == close_ch:
            depth -= 1
            if depth == 0: return i
        i += 1
    return -1


# ── Data ──────────────────────────────────────────────────────────────────────

@dataclass
class Method:
    # Pure declaration signature (no init-list, no body) — what stays in .h
    pure_decl: str
    # Full definition signature for .cpp (Class:: added, override/final stripped)
    defn_sig: str
    # Constructor init-list, e.g. ": m_ecs(ecs), m_x(0)" or ""
    init_list: str
    # Body text "{...}"
    body: str
    class_name: str
    keep: bool  # True → inline/template, leave in .h as-is
    src_start: int  # offset in source where the method sig begins
    src_end: int  # offset just past closing }


@dataclass
class ParseResult:
    header: str  # cleaned .h content
    methods: list[Method] = field(default_factory=list)
    keep_includes: list[str] = field(default_factory=list)  # stays in .h
    cpp_includes: list[str] = field(default_factory=list)  # moves to .cpp


# ── Helpers ───────────────────────────────────────────────────────────────────

def src_of(tokens: list[Token]) -> str:
    return "".join(t.text for t in tokens)


def no_override(sig: str) -> str:
    s = re.sub(r"\b(override|final)\b\s*", "", sig)
    return re.sub(r"  +", " ", s).rstrip()


def add_class_prefix(sig: str, cls: str) -> str:
    if not cls or f"{cls}::" in sig:
        return sig
    return re.sub(
        r"(\b)(\w+)(\s*\()",
        lambda m: m.group(1) + cls + "::" + m.group(2) + m.group(3),
        sig, count=1,
    )


def fix_sig_indent(sig: str) -> str:
    """Align continuation lines to one column past the opening ("""
    paren = sig.find("(")
    if paren == -1:
        return re.sub(r"[ \t]*\n[ \t]*", "\n    ", sig)
    indent = " " * (paren + 1)
    return re.sub(r"[ \t]*\n[ \t]*", "\n" + indent, sig)


def dedent_body(body: str) -> str:
    inner = body[1:-1].split("\n")
    non_empty = [l for l in inner if l.strip()]
    if not non_empty:
        return "{\n}"
    min_i = min(len(l) - len(l.lstrip()) for l in non_empty)
    out = [("    " + l[min_i:]) if l.strip() else "" for l in inner]
    return "{\n" + "\n".join(out).strip("\n") + "\n}"


def method_name_from(sig: str) -> str:
    m = re.search(r"(\w+)\s*\(", sig)
    return m.group(1) if m else ""


def split_init_list(sig_tokens: list[Token]) -> tuple[list[Token], list[Token]]:
    """
    Given all tokens between method start and the opening {,
    split into (decl_tokens, init_list_tokens).

    Finds the closing ) of the PARAMETER LIST (not any nested parens),
    then looks for a bare ':' (not '::') after it.
    e.g.  "Foo(int x) : m_x(x), m_y(0)"
          decl_tokens = tokens for "Foo(int x)"
          init_tokens = tokens for ": m_x(x), m_y(0)"
    """
    # Find the closing ) of the parameter list.
    # The parameter list opens at the FIRST '(' at paren-depth 0.
    params_close = -1
    depth = 0
    for idx, t in enumerate(sig_tokens):
        if t.kind in ("string", "comment"):
            continue
        if t.text == "(":
            depth += 1
        elif t.text == ")":
            depth -= 1
            if depth == 0:
                params_close = idx
                break  # stop at the FIRST matching close-paren

    if params_close == -1:
        return sig_tokens, []

    after = sig_tokens[params_close + 1:]

    # Collect optional qualifiers (const, noexcept, &, && …) then look for ':'
    # but not '::'
    colon_idx = -1
    for idx, t in enumerate(after):
        if t.kind in ("space", "newline", "comment"):
            continue
        if t.text == ":":
            colon_idx = idx
            break
        # Any other real token before ':' means this is a qualifier (const, noexcept…)
        # keep scanning

    if colon_idx == -1:
        return sig_tokens, []

    decl_tokens = sig_tokens[:params_close + 1]
    # Grab qualifiers between ) and : (e.g. "const noexcept")
    qual = src_of(after[:colon_idx]).strip()
    if qual:
        fake = Token("space", " " + qual, -1)
        decl_tokens = decl_tokens + [fake]

    init_tokens = after[colon_idx:]  # starts at ':'
    return decl_tokens, init_tokens


# ── Parser ────────────────────────────────────────────────────────────────────

def find_class_body(tokens: list[Token]) -> tuple[int, int, str]:
    """
    Returns (open_brace_idx, close_brace_idx, class_name) for the first
    top-level class/struct that should have its methods split out.

    Rules:
      - Forward declarations (Foo;) are always skipped.
      - Plain 'struct' without a base class is treated as a data structure
        and skipped — its methods stay in the header.
      - 'class' is always a candidate.
      - 'struct' with a base class (struct Foo : Bar) is also a candidate.
    """
    i = 0
    while i < len(tokens):
        t = tokens[i]
        if t.kind != "word" or t.text not in ("class", "struct"):
            i += 1;
            continue

        keyword = t.text
        j = skip_ws(tokens, i + 1)
        if j >= len(tokens) or tokens[j].kind != "word":
            i += 1;
            continue
        name = tokens[j].text

        # Scan between name and { or ; to detect base class and forward decls
        has_base = False
        k = j + 1
        brace_k = -1
        while k < len(tokens):
            if tokens[k].kind in ("space", "newline", "comment"):
                k += 1;
                continue
            if tokens[k].text == ";":
                # forward declaration
                i = k + 1;
                break
            if tokens[k].text == ":":
                # could be base class or bit-field; if next non-ws is not ":"
                # then it IS a base class
                nxt = skip_ws(tokens, k + 1)
                if nxt < len(tokens) and tokens[nxt].text != ":":
                    has_base = True
                k += 1;
                continue
            if tokens[k].text == "{":
                brace_k = k;
                break
            k += 1
        else:
            break  # ran out of tokens

        if brace_k == -1:
            # hit ; → forward decl, already advanced i
            continue

        # struct without base class → data structure, leave it alone
        if keyword == "struct" and not has_base:
            close = find_matching(tokens, brace_k, "{", "}")
            i = close + 1 if close != -1 else brace_k + 1
            continue

        close = find_matching(tokens, brace_k, "{", "}")
        if close == -1:
            i = brace_k + 1;
            continue
        return brace_k, close, name

    return -1, -1, ""


def parse_methods_in_class(
        tokens: list[Token],
        class_open: int,
        class_close: int,
        class_name: str,
) -> list[Method]:
    methods: list[Method] = []
    i = class_open + 1

    while i < class_close:
        t = tokens[i]

        # Skip whitespace, comments, preprocessor
        if t.kind in ("space", "newline", "comment", "pp", "string"):
            i += 1;
            continue

        # Access specifier: public: / private: / protected:
        if t.kind == "word" and t.text in ("public", "private", "protected"):
            j = skip_ws(tokens, i + 1)
            if j < len(tokens) and tokens[j].text == ":":
                i = j + 1;
                continue

        # Skip nested class/struct/enum entirely
        if t.kind == "word" and t.text in ("class", "struct", "enum", "namespace"):
            j = i + 1
            while j < class_close and tokens[j].text not in ("{", ";"):
                j += 1
            if j < class_close and tokens[j].text == "{":
                close = find_matching(tokens, j, "{", "}")
                i = close + 1 if close != -1 else j + 1
            else:
                i = j + 1
            continue

        # Collect tokens until { or ; (the signature + optional init-list)
        sig_tokens: list[Token] = []
        j = i
        while j < class_close:
            tj = tokens[j]
            if tj.kind in ("comment", "pp"):
                j += 1;
                continue
            if tj.text in (";", "{"):
                break
            sig_tokens.append(tj)
            j += 1

        if j >= class_close or tokens[j].text == ";":
            # Declaration only — no body, skip
            i = j + 1;
            continue

        # tokens[j].text == "{"
        brace_open_idx = j

        # Must look like a function: has ( in sig
        if not any(t.text == "(" for t in sig_tokens):
            close = find_matching(tokens, brace_open_idx, "{", "}")
            i = close + 1 if close != -1 else brace_open_idx + 1
            continue

        # Must not start with control-flow keyword
        words = [t.text for t in sig_tokens if t.kind == "word"]
        if not words or words[0] in CONTROL_FLOW:
            close = find_matching(tokens, brace_open_idx, "{", "}")
            i = close + 1 if close != -1 else brace_open_idx + 1
            continue

        brace_close_idx = find_matching(tokens, brace_open_idx, "{", "}")
        if brace_close_idx == -1:
            i = brace_open_idx + 1;
            continue

        # Trim trailing whitespace from sig_tokens
        while sig_tokens and sig_tokens[-1].kind in ("space", "newline"):
            sig_tokens.pop()

        # Split off constructor init-list
        decl_tokens, init_tokens = split_init_list(sig_tokens)
        pure_decl = src_of(decl_tokens).strip()
        init_list = src_of(init_tokens).strip()

        # Build .cpp definition signature
        defn_sig = no_override(pure_decl)
        defn_sig = add_class_prefix(defn_sig, class_name)
        defn_sig = fix_sig_indent(defn_sig.strip())

        body_toks = tokens[brace_open_idx:brace_close_idx + 1]
        body = src_of(body_toks)

        keep = bool({t.text for t in sig_tokens if t.kind == "word"} & KEEP_IN_HEADER)
        src_start = sig_tokens[0].pos if sig_tokens else tokens[brace_open_idx].pos
        src_end = tokens[brace_close_idx].pos + len(tokens[brace_close_idx].text)

        methods.append(Method(
            pure_decl=pure_decl,
            defn_sig=defn_sig,
            init_list=init_list,
            body=body,
            class_name=class_name,
            keep=keep,
            src_start=src_start,
            src_end=src_end,
        ))

        i = brace_close_idx + 1

    return methods


# ── Include analysis ──────────────────────────────────────────────────────────

# Standard-library / well-known headers that are almost always needed only in
# .cpp (implementation details).  Purely heuristic — the identifier scan below
# is the real logic; this list only helps when a header appears but its
# identifiers aren't directly mentioned (e.g. pulled in for side-effects).
CPP_ONLY_HINTS: set[str] = {
    "algorithm", "cassert", "chrono", "cmath", "cstdio", "cstdlib",
    "cstring", "fstream", "functional", "iomanip", "iostream", "numeric",
    "random", "sstream", "stdexcept", "thread",
}


def extract_includes(src: str) -> list[tuple[str, str]]:
    """
    Return list of (raw_line, header_name) for every #include in src.
    header_name is the bare name without <>, "" or path prefix.
    e.g. '#include <vector>' → ('#include <vector>', 'vector')
         '#include "ECS/World.h"' → ('#include "ECS/World.h"', 'World')
    """
    result = []
    for line in src.splitlines():
        m = re.match(r'\s*#include\s*[<"]([^>"]+)[>"]', line)
        if m:
            raw = line.rstrip()
            path = m.group(1)
            # bare name = last path component without extension
            name = re.sub(r'\.h(?:pp)?$', '', path.split('/')[-1].split('\\\\')[-1])
            result.append((raw, name))
    return result


def identifiers_in(text: str) -> set[str]:
    """Return all C++ identifiers (word tokens) found in text."""
    return set(re.findall(r'\b[A-Za-z_][A-Za-z0-9_]*\b', text))


def classify_includes(
        src: str,
        methods: list["Method"],
        class_open_pos: int,
        class_close_pos: int,
) -> tuple[list[str], list[str]]:
    """
    Classify every #include in src as belonging to .h or .cpp only.

    Returns (keep_in_h, move_to_cpp) — each is a list of raw #include lines.

    Strategy
    --------
    For each include, extract its bare header name and use it to look up
    identifiers it likely provides (we don't actually parse it; instead we use
    the header name itself as a proxy identifier plus any identifiers embedded
    in the path).

    Then check:
      - If any identifier from the include name/path appears in the class
        *declaration* zone (outside method bodies) → keep in .h
      - Else if it appears only in method bodies → move to .cpp
      - Else (unused / ambiguous) → keep in .h (safe default)
    """
    all_includes = extract_includes(src)
    if not all_includes:
        return [], []

    # Text of the class declaration zone = class body minus all method bodies
    decl_zone = src[class_open_pos:class_close_pos]
    for m in methods:
        # Blank out method bodies so they don't pollute the decl scan
        body_start = m.src_end - len(m.body)
        rel_start = body_start - class_open_pos
        rel_end = m.src_end - class_open_pos
        if 0 <= rel_start < len(decl_zone):
            decl_zone = decl_zone[:rel_start] + " " * (rel_end - rel_start) + decl_zone[rel_end:]

    decl_ids = identifiers_in(decl_zone)

    # Collect identifiers from all method bodies
    body_ids: set[str] = set()
    for m in methods:
        body_ids |= identifiers_in(m.body)

    # Text before the class (preamble: global includes, typedefs, etc.)
    preamble = src[:class_open_pos]
    preamble_ids = identifiers_in(preamble)

    keep_in_h: list[str] = []
    move_to_cpp: list[str] = []

    for raw, name in all_includes:
        # Identifiers we associate with this include:
        # the name itself + any CamelCase/UPPER sub-words extracted from it
        assoc = {name} | set(re.findall(r'[A-Z][a-z]+|[A-Z]{2,}|[a-z]+', name))
        assoc = {a for a in assoc if len(a) > 1}

        used_in_decl = bool(assoc & decl_ids)
        used_in_bodies = bool(assoc & body_ids)
        hint_cpp_only = name.lower() in CPP_ONLY_HINTS

        if used_in_decl:
            # Needed in the class declaration → must stay in .h
            keep_in_h.append(raw)
        elif used_in_bodies and not used_in_decl:
            # Only used in method implementations → move to .cpp
            move_to_cpp.append(raw)
        elif hint_cpp_only and not used_in_decl:
            move_to_cpp.append(raw)
        else:
            # Ambiguous or unused — keep in .h (conservative)
            keep_in_h.append(raw)

    return keep_in_h, move_to_cpp


def rewrite_header_includes(src: str, move_to_cpp: list[str]) -> str:
    """Remove the lines in move_to_cpp from src."""
    if not move_to_cpp:
        return src
    to_remove = set(move_to_cpp)
    lines = src.splitlines(keepends=True)
    out = []
    for line in lines:
        if line.rstrip() in to_remove:
            continue
        out.append(line)
    # Collapse runs of 3+ blank lines into 2
    result = re.sub(r'(\n\s*){3,}', '\n\n', "".join(out))
    return result


def parse(src: str) -> ParseResult:
    # Guard: if the file contains both a class AND a bare #include after the
    # class closing }, it was probably a concatenated .h+.cpp.
    # Only tokenize up to the end of the first top-level class.
    tokens = tokenize(src)
    class_open, class_close, class_name = find_class_body(tokens)

    if class_open == -1:
        return ParseResult(header=src)

    # Determine the end of "safe" source: position just past the class closing }
    class_end_pos = tokens[class_close].pos + 1  # +1 for the '}'

    methods = parse_methods_in_class(tokens, class_open, class_close, class_name)

    # Build clean header: replace each non-kept method with just its declaration + ;
    # Also truncate anything after the class (handles .h + .cpp concatenation)
    header = src[:class_end_pos]
    # Process replacements in reverse order
    to_replace = [m for m in methods if not m.keep]
    for m in sorted(to_replace, key=lambda x: -x.src_start):
        decl = m.pure_decl.rstrip()
        # Preserve the original indentation before the method
        replacement = decl + ";\n"
        header = header[:m.src_start] + replacement + header[m.src_end:]

    # Restore any trailing content after class (like \n or ;)
    # Find the ; or \n right after class_end_pos in original
    tail = src[class_end_pos:]
    # Grab up to and including the ; that closes the class definition
    semi_match = re.match(r"([^#\n]*\n?)", tail)
    if semi_match:
        header = header + semi_match.group(1)

    # Analyse includes
    class_open_pos = tokens[class_open].pos
    class_close_pos = tokens[class_close].pos
    keep_inc, cpp_inc = classify_includes(
        src, [m for m in methods if not m.keep], class_open_pos, class_close_pos
    )
    # Rewrite header: remove cpp-only includes
    header = rewrite_header_includes(header, cpp_inc)
    return ParseResult(
        header=header,
        methods=[m for m in methods if not m.keep],
        keep_includes=keep_inc,
        cpp_includes=cpp_inc,
    )


# ── .cpp builder ──────────────────────────────────────────────────────────────

def is_already_defined(m: Method, cpp: str) -> bool:
    name = method_name_from(m.defn_sig)
    if not name:
        return False
    prefix = f"{m.class_name}::" if m.class_name else ""
    pat = re.compile(
        rf"\b{re.escape(prefix)}{re.escape(name)}\s*\([^)]*\)[^;{{]*\{{",
        re.DOTALL,
    )
    return bool(pat.search(cpp))


def fmt_method(m: Method) -> str:
    body = dedent_body(m.body)
    if m.init_list:
        # Format init-list: each initializer on its own line, indented
        init = m.init_list
        if not init.startswith(":"):
            init = ": " + init
        return f"{m.defn_sig}\n    {init}\n{body}"
    return f"{m.defn_sig}\n{body}"


def build_cpp(
        methods: list[Method],
        header_name: str,
        existing: Optional[str],
        cpp_includes: Optional[list[str]] = None,
) -> tuple[str, int, int]:
    to_add, skipped = [], 0
    for m in methods:
        if existing and is_already_defined(m, existing):
            skipped += 1
        else:
            to_add.append(fmt_method(m))

    if not to_add:
        return (existing or "", 0, skipped)

    additions = "\n\n".join(to_add)
    if existing is not None:
        content = (
                existing.rstrip()
                + "\n\n// --- split_headers: auto-generated ---\n\n"
                + additions + "\n"
        )
    else:
        inc_block = "\n".join(cpp_includes) + "\n" if cpp_includes else ""
        content = f'#include "{header_name}"\n{inc_block}\n{additions}\n'
    return content, len(to_add), skipped


# ── Runner ────────────────────────────────────────────────────────────────────

def process(h: Path, apply: bool) -> bool:
    src = h.read_text(encoding="utf-8", errors="replace")
    r = parse(src)

    if not r.methods:
        print(f"\n  {h.name}: no method bodies found, skipping.")
        return False

    cpp_path = h.with_suffix(".cpp")
    existing = cpp_path.read_text(encoding="utf-8", errors="replace") if cpp_path.exists() else None
    new_cpp, added, skipped = build_cpp(r.methods, h.name, existing, r.cpp_includes)

    print(f"\n{'=' * 60}")
    print(f"File: {h}")
    print(f"  Methods in header        : {len(r.methods)}")
    print(f"  To move to .cpp          : {added}")
    if skipped:
        print(f"  Already in .cpp (skipped): {skipped}")
    for m in r.methods:
        tag = " [skipped]" if (existing and is_already_defined(m, existing)) else ""
        nm = method_name_from(m.defn_sig) or "?"
        print(f"    - {m.class_name + '::' if m.class_name else ''}{nm}(...){tag}")

    if r.cpp_includes:
        print(f"  Includes moved to .cpp    : {len(r.cpp_includes)}")
        for inc in r.cpp_includes:
            print(f"    → {inc}")
    if r.keep_includes:
        print(f"  Includes kept in .h       : {len(r.keep_includes)}")

    if added == 0:
        print("  Nothing new to add.")
        return False

    if apply:
        h.write_text(r.header, encoding="utf-8")
        cpp_path.write_text(new_cpp, encoding="utf-8")
        action = "created" if existing is None else "updated"
        print(f"  ✓ {h.name} cleaned, {cpp_path.name} {action}")
    else:
        print("  [dry-run] Pass --apply to apply.")

    return True


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 split_headers.py <directory> [--apply]")
        sys.exit(1)

    d = Path(sys.argv[1])
    apply = "--apply" in sys.argv

    if not d.is_dir():
        print(f"Error: '{d}' is not a directory")
        sys.exit(1)

    if not apply:
        print("⚠️  DRY-RUN — no files changed. Pass --apply to apply.")

    files = sorted(d.rglob("*.h"))
    if not files:
        print("No .h files found.")
        return

    print(f"Found {len(files)} .h file(s) in '{d}'")
    changed = sum(1 for f in files if process(f, apply))

    print(f"\n{'=' * 60}")
    if apply:
        print(f"Done! Processed {changed}/{len(files)} file(s).")
    else:
        print(f"Dry-run: {changed} file(s) have methods to move.")
        print("Run with --apply to apply.")


if __name__ == "__main__":
    main()
# This will be replaced — just checking line count

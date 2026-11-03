"""Format owned C++ headers using whitespace edits; never build or run tests."""
from pathlib import Path
import difflib
import hashlib
import json
import re
import subprocess
from tree_sitter import Language, Parser
import tree_sitter_cpp

ROOT = Path(__file__).resolve().parents[1]
PARSER = Parser(Language(tree_sitter_cpp.language()))
OUT = ROOT / 'framework-analysis/api-reference'


def write(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding='utf-8-sig', newline='\n')


def functions(node):
    if node.type in ('function_declarator', 'operator_cast'):
        return True
    return any(functions(c) for c in node.named_children
               if c.type not in ('compound_statement', 'parameter_list', 'template_argument_list'))


def format_header(source):
    data = source.encode('utf-8')
    tree = PARSER.parse(data)
    edits = {}

    def gap(a, b, value, priority=0):
        if a <= b and not data[a:b].strip():
            old = edits.get((a, b))
            if old is None or priority >= old[0]:
                edits[a, b] = priority, value.encode('utf-8')

    def indent(node):
        start = data.rfind(b'\n', 0, node.start_byte) + 1
        return re.match(rb'[ \t]*', data[start:node.start_byte]).group().decode('utf-8')

    def walk(node):
        # Raw GLSL, strings, preprocessor lines and comments retain their bytes.
        if node.type in ('raw_string_literal', 'string_literal', 'comment') or node.type.startswith('preproc'):
            return
        children = node.named_children
        if node.type == 'compound_statement' and children:
            pad = indent(node.parent) if node.parent is not None else indent(node)
            gap(node.start_byte + 1, children[0].start_byte, '\n' + pad + '    ')
            for a, b in zip(children, children[1:]):
                if b.start_point.row == a.end_point.row:
                    gap(a.end_byte, b.start_byte, '\n' + pad + '    ')
            gap(children[-1].end_byte, node.end_byte - 1, '\n' + pad)
        if node.type == 'if_statement':
            consequence = node.child_by_field_name('consequence')
            condition = node.child_by_field_name('condition')
            if consequence and condition and consequence.type != 'compound_statement':
                if consequence.start_point.row == condition.end_point.row:
                    gap(condition.end_byte, consequence.start_byte, '\n' + indent(node) + '    ')
        if node.type in ('field_declaration_list', 'declaration_list', 'translation_unit'):
            for i, child in enumerate(children):
                if child.type not in ('declaration', 'field_declaration', 'function_definition', 'template_declaration'):
                    continue
                if not functions(child):
                    continue
                first = i
                while first and children[first - 1].type == 'comment':
                    comment = children[first - 1]
                    # A trailing comment belongs to the preceding declaration.
                    if first > 1 and children[first - 2].end_point.row == comment.start_point.row:
                        break
                    first -= 1
                for a, b in zip(children[first:i], children[first + 1:i + 1]):
                    gap(a.end_byte, b.start_byte, '\n' + indent(child), 2)
                if first:
                    previous = children[first - 1]
                    if previous.type != 'access_specifier':
                        gap(previous.end_byte, children[first].start_byte,
                            '\n\n\n' + indent(child), 3)
                last = i
                if i + 1 < len(children) and children[i + 1].type == 'comment':
                    if children[i + 1].start_point.row == child.end_point.row:
                        last += 1
                if last + 1 < len(children) and children[last + 1].type != 'comment':
                    gap(children[last].end_byte, children[last + 1].start_byte,
                        '\n\n\n' + indent(children[last + 1]), 1)
        for child in children:
            walk(child)

    walk(tree.root_node)
    result = data
    for (a, b), (_, value) in sorted(edits.items(), reverse=True):
        result = result[:a] + value + result[b:]
    return result.decode('utf-8')


def main():
    paths = [ROOT / 'standard/Console.h']
    for base in ('framework', 'standard/framework'):
        paths.extend(sorted(p for p in (ROOT / base).rglob('*.h') if 'dependencies' not in p.parts))
    inventory, report = [], ['# 헤더 변경 전후\n',
        '공백·줄바꿈만 변경했다. 주석은 선언에 붙여 유지했다. 빌드·테스트·IDE hover 확인은 사용자에게 맡긴다.\n']
    for path in paths:
        raw = path.read_bytes()
        before = subprocess.check_output(
            ['git', 'show', 'HEAD:' + path.relative_to(ROOT).as_posix()], cwd=ROOT).decode('utf-8-sig')
        before = before.replace('\r\n', '\n')
        # This formatter is a one-time migration against the approved clean HEAD.
        # Preserve all non-whitespace bytes, including complete literal contents.
        def tokens(text):
            data = text.encode('utf-8')
            def leaves(node):
                if not node.children or node.type in ('raw_string_literal', 'string_literal', 'char_literal', 'comment'):
                    return [data[node.start_byte:node.end_byte]]
                return [token for child in node.children for token in leaves(child)]
            return leaves(PARSER.parse(data).root_node)
        if tokens(before) != tokens(raw.decode('utf-8-sig').replace('\r\n', '\n')):
            raise RuntimeError('Existing non-format source change: ' + str(path))
        after = format_header(before)
        changed = after != before or not raw.startswith(b'\xef\xbb\xbf')
        inventory.append({'file': path.relative_to(ROOT).as_posix(), 'changed': changed,
                          'before_sha256': hashlib.sha256(raw).hexdigest()})
        if not changed:
            continue
        # Refuse to overwrite a file changed by another writer during generation.
        if path.read_bytes() != raw:
            raise RuntimeError('Concurrent source change: ' + str(path))
        write(path, after)
        report.append('\n## ' + path.relative_to(ROOT).as_posix() + '\n')
        a, b = before.splitlines(), after.splitlines()
        for group in difflib.SequenceMatcher(None, a, b, autojunk=False).get_grouped_opcodes(2):
            report.append('\n변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).\n')
            report.append('\n### before\n```cpp\n' + '\n'.join(a[group[0][1]:group[-1][2]]) + '\n```\n')
            report.append('\n### after\n```cpp\n' + '\n'.join(b[group[0][3]:group[-1][4]]) + '\n```\n')
    write(OUT / 'HEADERS_BEFORE_AFTER.md', ''.join(report))
    write(OUT / 'HEADER_INVENTORY.json', json.dumps(inventory, ensure_ascii=False, indent=2) + '\n')
    print('Owned headers:', len(paths), '; changed:', sum(r['changed'] for r in inventory))


if __name__ == '__main__':
    main()

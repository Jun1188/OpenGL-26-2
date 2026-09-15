"""Black-box regression checks; uses only the Python standard library."""
import argparse
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--exe', type=Path, default=ROOT / 'x64/Debug/Computer-Graphics_WarmmingUp2.exe')
args = parser.parse_args()
EXE = args.exe.resolve()
OUTPUT_DIR = ROOT / 'outout-6'
checks = 0

def check(condition, description):
    global checks
    if not condition:
        raise AssertionError(description)
    checks += 1

def run(*arguments, input_text='', cwd=ROOT):
    return subprocess.run([str(EXE), *map(str, arguments)], input=input_text,
                          capture_output=True, text=True, encoding='utf-8', timeout=15, cwd=cwd)

def run_with_output(*arguments, input_text='', cwd=ROOT):
    before = set(OUTPUT_DIR.glob('*.txt'))
    result = run(*arguments, input_text=input_text, cwd=cwd)
    created = sorted(set(OUTPUT_DIR.glob('*.txt')) - before)
    captured = [(path.name, path.read_text(encoding='utf-8')) for path in created]
    for path in created:
        path.unlink()
    return result, captured

def frames(text):
    lines = text.splitlines()
    result = []
    for index, line in enumerate(lines):
        match = re.search(r'Board (\d+)x\d+', line)
        if match:
            size = int(match[1])
            result.append([row.split() for row in lines[index + 1:index + 1 + size]])
    return result

def board(coordinates, commands):
    result = run('--board', input_text=coordinates + '\n' + '\n'.join(commands) + '\nq\n')
    check(result.returncode == 0, 'board process exits successfully')
    return result.stdout, frames(result.stdout)

with tempfile.TemporaryDirectory(prefix='warmup-cli-') as directory:
    temporary = Path(directory)
    for name, count in [('cube', 12), ('pyramid', 6), ('tetrahedron', 4)]:
        source = ROOT / 'data' / f'{name}.txt'
        before = source.read_bytes()
        result, outputs = run_with_output('--parse', source, cwd=temporary)
        check(result.returncode == 0, name + ' exit')
        check(len(re.findall(r'^Face ', result.stdout, re.M)) == count, name + ' faces')
        check('Validation succeeded' in result.stdout, name + ' valid summary')
        check(len(outputs) == 1 and outputs[0][1] in result.stdout, name + ' output file mirrors console report')
        check(source.read_bytes() == before and not list(temporary.iterdir()), name + ' no file writes')
    source = temporary / '한글 입력.txt'
    source.write_text('v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\nf 1 2\nf 1 8 3\n', encoding='utf-8')
    before = source.read_bytes()
    result, outputs = run_with_output('--parse', source, cwd=temporary)
    check(result.returncode == 2, 'invalid faces return 2')
    check('Face 2:' in result.stdout and 'Face 3:' in result.stdout, 'all failing faces identified')
    check('Faces: 1/3 valid' in result.stdout, 'partial console result with explicit failure')
    check(len(outputs) == 1 and 'Faces: 1/3 valid' in outputs[0][1], 'invalid input output file retained')
    check(source.read_bytes() == before and list(temporary.iterdir()) == [source], 'invalid input also writes nothing')
    check(run('--parse', temporary / 'missing.txt').returncode == 1, 'missing file')
    check(run('--parse', source, temporary / 'not-created.txt').returncode == 1, 'old output-path syntax rejected')
    check(not (temporary / 'not-created.txt').exists(), 'no output file from obsolete syntax')

result, outputs = run_with_output(input_text='6\ndata/pyramid.txt\nq\n')
check(result.returncode == 0 and 'Faces: 6/6 valid' in result.stdout, 'interactive problem 6')
check(len(outputs) == 1 and outputs[0][0].startswith('pyramid-'), 'interactive problem 6 creates numbered output')
check('Output file: ' in result.stdout, 'created output name reported')

result, outputs = run_with_output('--parse', ROOT / 'data' / 'err.txt')
check(result.returncode == 2, 'err.txt intentionally fails validation')
for expected in [
    'A vertex requires three coordinate components.',
    'Unsupported record: vn',
    'A face requires exactly three corners.',
    'Duplicate vertex index within this face.',
    'Vertex index out of range',
    'Mixed face formats',
    'Face references an invalid vertex declaration.',
    'Face references an invalid texture declaration.',
    'Duplicate texture coordinates within this face.',
    'Collinear vertices do not form a triangle.',
    'Faces: 1/10 valid',
]:
    check(expected in result.stdout, 'err.txt covers ' + expected)
check(len(outputs) == 1 and outputs[0][0].startswith('err-'), 'err.txt creates numbered output')
check(outputs[0][1] in result.stdout, 'err.txt file mirrors console report')
result = run('--board', input_text='1+1 2 2\n1 1 2 2\n10 10 11 11\nq\n')
check('Invalid coordinates.' in result.stdout, 'coordinate tokens must be whitespace-separated integers')

# Both rectangles: one blocked axis rejects the whole compound command.
for target in [1, 2]:
    coordinates = '1 1 2 2\n10 10 11 11' if target == 1 else '10 10 11 11\n1 1 2 2'
    text, images = board(coordinates, [f'{target}X', f'{target}S', f'{target}b'])
    check(f'Area {target}: 2 x 2 = 4' in text, 'x-wrapped S rejects both axes')
    check(images[1] == images[2], 'x-wrapped S preserves display')
    check('entire command was rejected' in text, 'x-wrapped S reports rejection')
    text, images = board(coordinates, [f'{target}X', f'{target}Y'] + [f'{target}S'] * 10 + [f'{target}b'])
    check(f'Area {target}: 2 x 2 = 4' in text, 'dual-wrapped repeated S preserves dimensions')
    check(all(image == images[2] for image in images[3:]), 'dual-wrapped repeated S preserves display')
    text, _ = board(coordinates, [f'{target}X', f'{target}Y', f'{target}s', f'{target}b'])
    check(f'Area {target}: 1 x 1 = 1' in text, 'dual-wrapped s can shrink both axes')

for coordinates, command in [
    ('2 2 3 2\n10 10 11 11', 'a'),
    ('2 2 2 3\n10 10 11 11', 'A'),
    ('2 2 2 3\n10 10 11 11', 's'),
    ('29 2 30 3\n10 10 11 11', 'S'),
    ('29 2 30 3\n10 10 11 11', 'a'),
    ('2 29 3 30\n10 10 11 11', 'A'),
]:
    text, images = board(coordinates, [command] * 5)
    check(images[0] == images[-1], 'repeated rejected command preserves whole rectangle')
    check('entire command was rejected' in text, 'reason visible')

text, images = board('2 2 5 6\n6 6 9 9', ['x'])
check(images[-1][5][5] == '#', 'PDF collision example')
text, images = board('1 1 2 2\n10 10 11 11', ['X', 's', 'S', 'b', 'q', 'x'])
check(len(images) == 5 and 'Area 1: 1 x 1 = 1' in text, 'q directly after area exits')
for key in 'xXyY':
    _, images = board('1 1 2 2\n10 10 11 11', [key] * 60)
    check(images[0] == images[-1], 'two complete circuits ' + key)
_, images = board('30 30 30 30\n10 10 11 11', ['d'])
check(len(images[-1]) == 30 and images[0] == images[-1], 'board shrink does not relocate')
_, images = board('1 1 2 2\n10 10 11 11', ['X', 'd'])
check(len(images[-1]) == 30, 'negative origin blocks shrink')
_, images = board('1 1 2 2\n5 5 5 5', ['c'] * 11 + ['d'] * 31)
check(len(images[11]) == 40 and len(images[-1]) == 10, 'board size bounds')
_, images = board('1 1 2 2\n10 10 11 11', ['X', 's', 'r', '2 2 3 3', '10 10 11 11', 'S', 'b'])
check(len(images[-1]) == 30, 'reset board')
print(f'PASS {checks} CLI checks')

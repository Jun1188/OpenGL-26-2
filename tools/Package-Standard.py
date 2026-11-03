"""Create the source-only standard ZIP; no build, app run, tests or validators."""
from pathlib import Path
import argparse
import datetime
import hashlib
import json
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / 'standard'
LIMIT = 30_000_000


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', default='dist/standard-composition-20261103.zip')
    args = parser.parse_args()
    output = (ROOT / args.output).resolve()
    output.relative_to(ROOT)
    if output.is_relative_to(SOURCE.resolve()):
        raise SystemExit('Output must be outside the source tree.')
    if output.exists():
        raise SystemExit('Output already exists. Choose a new --output path; existing artifacts are preserved.')
    included, excluded = [], []
    for path in sorted(SOURCE.rglob('*')):
        if not path.is_file():
            continue
        relative = path.relative_to(SOURCE)
        reason = None
        if relative.parts[0] in {'.vs', 'x64', '.git', '__pycache__'}:
            reason = 'IDE cache/build output; source, bundled SDK and licences remain included'
        elif path.name.endswith('.vcxproj.user') or path.suffix in {'.pyc', '.tmp'}:
            reason = 'Device-local or temporary file'
        elif not path.resolve().is_relative_to(SOURCE.resolve()):
            reason = 'Outside source tree link'
        entry = {'path': relative.as_posix(), 'bytes': path.stat().st_size}
        if reason:
            excluded.append({**entry, 'reason': reason})
        else:
            entry['sha256'] = hashlib.sha256(path.read_bytes()).hexdigest()
            included.append(entry)
    raw = sum(p['bytes'] for p in included)
    if raw >= LIMIT:
        raise SystemExit(f'Source exceeds 30MB before compression: {raw}. Review packaging scope; no files were deleted.')
    output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(output, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for entry in included:
            archive.write(SOURCE / entry['path'], 'standard/' + entry['path'])
    size = output.stat().st_size
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    result = {
        'date': datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).isoformat(),
        'source_commit': commit, 'source': str(SOURCE), 'archive': str(output),
        'raw_bytes': raw, 'zip_bytes': size, 'limit_bytes': LIMIT,
        'archive_sha256': hashlib.sha256(output.read_bytes()).hexdigest(),
        'included': included, 'excluded': excluded,
        'notes': ['Current working source includes preserved user changes.',
                  'Bundled SDK x64 libraries are included; only standard/x64 build output is excluded.',
                  'No source, cache, output, backup or user file was deleted.',
                  'No build, app run, tests, archive testzip, or validators were run.']}
    output.with_suffix('.manifest.json').write_text(json.dumps(result, ensure_ascii=False, indent=2)+'\n', encoding='utf-8-sig')
    report = ['# standard source package size', '', f'Source commit: `{commit}`. Current working source, including preserved user changes.', '',
              f'- Included files: {len(included)}', f'- Uncompressed: {raw:,} bytes ({raw / 1_000_000:.2f} MB)',
              f'- ZIP: {size:,} bytes ({size / 1_000_000:.2f} MB)', f'- Limit: {LIMIT:,} bytes',
              f'- Excluded cache/build bytes: {sum(e["bytes"] for e in excluded):,}',
              '- Deleted bytes: 0', '- Build/runtime/tests: delegated to user', '',
              'The adjacent manifest lists every included/excluded path, size and included-file SHA-256. The ZIP is a C++ source project; the old executable is not included.']
    output.with_suffix('.SIZE.md').write_text('\n'.join(report)+'\n',encoding='utf-8-sig')
    print(json.dumps({k:result[k] for k in ('archive','raw_bytes','zip_bytes','limit_bytes','archive_sha256')}, ensure_ascii=False))
    if size >= LIMIT:
        raise SystemExit('ZIP exceeds 30MB. Artifact retained for review; do not report delivery success.')


if __name__ == '__main__':
    main()

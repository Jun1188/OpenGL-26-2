"""Shared source packaging policy; preserve bundled SDKs, DLLs and licenses."""
from pathlib import Path
LIMIT=30_000_000

def exclusion(relative: Path, include_learning=False):
    parts=relative.parts
    if not parts: return 'Empty path'
    # Vendor x64 libraries are source inputs, not project build outputs.
    if parts[:2]==('framework','dependencies'): return None
    if parts[0] in {'.vs','x64','.git','__pycache__'}: return 'Project build output or IDE cache'
    if parts[:2] in {('debugger','x64'),('debugger','.vs')}: return 'Optional tool build output or IDE cache'
    if relative.name.endswith('.vcxproj.user') or relative.suffix in {'.pyc','.tmp'}: return 'Device-local or temporary file'
    if include_learning: return None
    if parts[0]=='debugger': return 'Optional source guidance project; not a Standard dependency'
    if parts[0] in {'docs','examples'} or parts[:2] in {('framework','docs'),('framework','examples')}:
        return 'Learning, tutorials or reference examples; not a default build input'
    if relative.suffix.lower() in {'.html','.htm','.md'}: return 'Learning/navigation documentation; vendor licenses are preserved separately'
    return None

def inventory(source: Path, include_learning=False):
    included=[];excluded=[];source=source.resolve()
    for p in sorted(source.rglob('*')):
        if not p.is_file(): continue
        relative=p.relative_to(source)
        reason=exclusion(relative,include_learning)
        if p.is_symlink() or not p.resolve().is_relative_to(source): reason='Link/reparse target outside distribution or symbolic link'
        entry={'path':relative.as_posix(),'bytes':p.stat().st_size}
        (excluded if reason else included).append({**entry,'reason':reason} if reason else entry)
    return included,excluded
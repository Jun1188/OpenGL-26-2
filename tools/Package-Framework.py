"""Create independent source ZIPs below 3,000,000 bytes; never build or run projects."""
from pathlib import Path
import argparse
import hashlib
import json
import zipfile

ROOT = Path(__file__).resolve().parent.parent
LIMIT = 3_000_000
PROJECTS = ["standard"] + [f"openGL_3dAssign_{n}" for n in range(15, 23)]
EXCLUDED_DIRS = {".git", ".vs", "__pycache__", "x64", "Debug", "Release", "packages",
                 "graphify-out", "framework-analysis", ".obj-work", "dist"}
BUILD_SUFFIXES = {".exe", ".pdb", ".idb", ".ipdb", ".iobj", ".ilk", ".tlog", ".pch",
                  ".sdf", ".opensdf", ".db", ".log", ".lastbuildstate", ".recipe", ".partial"}

def include(path, base):
    relative = path.relative_to(base)
    if path.is_symlink() or not path.resolve().is_relative_to(base.resolve()):
        raise RuntimeError(f"Unexpected link outside project: {path}")
    if "dependencies" in relative.parts:
        index = relative.parts.index("dependencies")
        return not any(p in EXCLUDED_DIRS for p in relative.parts[:index]) and "assimp" not in relative.parts
    return not (any(p in EXCLUDED_DIRS for p in relative.parts)
                or path.name.endswith(".vcxproj.user") or path.suffix.lower() in BUILD_SUFFIXES)

def package(project, destination):
    base = ROOT / project
    if not base.is_dir():
        raise RuntimeError(f"Project is absent: {base}")
    destination.mkdir(parents=True, exist_ok=True)
    output = destination / f"{project}.zip"
    temporary = output.with_suffix(".zip.partial")
    if output.exists() or temporary.exists():
        raise RuntimeError(f"Preserve existing output; choose another --output-dir: {output}")
    records = []
    with zipfile.ZipFile(temporary, "x", zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for path in sorted(base.rglob("*")):
            if not path.is_file() or not include(path, base):
                continue
            data = path.read_bytes()
            name = path.relative_to(ROOT).as_posix()
            archive.writestr(name, data)
            records.append({"path": name, "bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()})
        manifest = {"kind": "independent-source-project", "project": project,
                    "size_limit_bytes": LIMIT, "source_files": records,
                    "build_execution_tests_validators": "not_run; delegated to user",
                    "dependencies": "bundled SDKs and licenses; Windows, GPU driver, Visual C++ tools and Windows SDK required",
                    "practice_23": "deferred for joint development"}
        archive.writestr(f"{project}/DISTRIBUTION_MANIFEST.json",
                         json.dumps(manifest, ensure_ascii=False, indent=2).encode("utf-8-sig"))
        notice = f"""# {project} source distribution

Extract the complete folder. Open .sln when provided, otherwise .vcxproj.
Use x64 Debug/Release. Keep framework and its SDKs, DLLs and licenses together.
No external SDK download or package restore is needed. The receiving PC needs
Visual C++ build tools, a Windows SDK, Windows x64 and an OpenGL 3.3 capable driver.
standard and practices 20-22 select an installed toolset rather than fixing v145.
Older practices retain their original project toolset; retarget to v143 for VS 2022.
Read framework/docs/APPLICATION_API.md and the project's README/USER_CHECKS.
Source includes existing workspace documentation. IDE state and compiler output
are excluded. This is source, not a prebuilt application. Builds, execution,
tests and validators were not run by the agent. Practice 23 is deferred.
"""
        archive.writestr(f"{project}/README_PACKAGE.md", notice.encode("utf-8-sig"))
    size = temporary.stat().st_size
    if size >= LIMIT:
        raise RuntimeError(f"ZIP too large: {size:,} >= {LIMIT:,}; retained {temporary}")
    temporary.rename(output)
    return {"project": project, "archive": str(output.relative_to(ROOT)) if output.is_relative_to(ROOT) else str(output),
            "zip_bytes": size, "limit_bytes": LIMIT, "source_files": len(records),
            "sha256": hashlib.sha256(output.read_bytes()).hexdigest(), "build_execution_tests_validators": "not_run"}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project", choices=PROJECTS, action="append", help="Repeat for a subset; default is all present projects.")
    parser.add_argument("--output-dir", type=Path, default=ROOT / "dist/portable-20261101")
    args = parser.parse_args()
    destination = args.output_dir.resolve()
    summary = destination / "PACKAGES.json"
    if summary.exists():
        raise SystemExit(f"Preserve existing package inventory: {summary}")
    selected = args.project or [p for p in PROJECTS if (ROOT / p).is_dir()]
    results = [package(p, destination) for p in dict.fromkeys(selected)]
    summary.write_text(json.dumps(results, ensure_ascii=False, indent=2) + "\n", encoding="utf-8-sig")
    for result in results:
        print(f"{result['project']}: {result['zip_bytes']:,} bytes (< {LIMIT:,}); {result['archive']}")

if __name__ == "__main__":
    main()

"""Create a source distribution without building or running the applications."""
from pathlib import Path
import argparse
import hashlib
import json
import zipfile

ROOT = Path(__file__).resolve().parent.parent
LIMIT = 30_000_000  # Decimal MB: stricter than 30 MiB.
PROJECTS = ["standard", "openGL_sample0"] + [f"openGL_3dAssign_{n}" for n in range(15, 21)]
EXCLUDED_DIRS = {".git", ".vs", "__pycache__", "x64", "Debug", "Release", "packages",
                 "graphify-out", "framework-analysis", ".obj-work", "dist"}
BUILD_SUFFIXES = {".exe", ".pdb", ".idb", ".ipdb", ".iobj", ".ilk", ".tlog", ".pch",
                  ".sdf", ".opensdf", ".db", ".log", ".lastbuildstate", ".recipe"}
NOTICE = """# Framework and samples source package

Includes the root framework, standard, openGL_sample0 and practices 15-20 present in
the workspace. Each practice retains its own complete framework. Open the desired
.slnx file in Visual Studio and select x64, Debug or Release. C++17 and the Windows
SDK / Visual C++ toolset are prerequisites. No NuGet restore or Assimp is required.

The package contains current workspace source, including existing user edits.
Executables, compiler intermediates, IDE state, NuGet caches and analysis reports
are excluded. All remaining framework SDK files, DLLs, link libraries, headers,
licenses, model data and shaders are retained. Build and runtime tests were not
performed by the agent. openGL_sample0 uses ../OpenGL.props and the root framework;
keep these together. Practices and standard retain independent bundled frameworks.

See framework/docs/OBJ_LOADING.md for the OBJ API, supported syntax, fallback
diagnostics, explicit texture setup and user-run checks. MTL, glTF and FBX are not
supported. See DISTRIBUTION_MANIFEST.json for exact source-byte hashes.

To recreate the package from this folder:
    python tools/Package-Framework.py
The script requires only Python's standard library. It refuses to replace an
existing archive; pass --output with a new destination when needed.
"""


def include(path, base):
    rel = path.relative_to(base)
    if path.is_symlink() or not path.resolve().is_relative_to(ROOT):
        raise RuntimeError(f"Unexpected link outside distribution: {path}")
    # Keep complete SDKs, including their Release/x64 DLL and LIB directories.
    if "dependencies" in rel.parts:
        sdk_index = rel.parts.index("dependencies")
        if any(part in EXCLUDED_DIRS for part in rel.parts[:sdk_index]):
            return False
        return "assimp" not in rel.parts
    if any(part in EXCLUDED_DIRS for part in rel.parts):
        return False
    if path.name.endswith(".vcxproj.user") or path.suffix.lower() in BUILD_SUFFIXES:
        return False
    # Never filter OBJ model text by suffix. Compiler .obj files live in x64/Debug/Release.
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT / "dist/framework-samples.zip")
    args = parser.parse_args()
    output = args.output.resolve()
    if output.exists():
        raise SystemExit(f"Archive already exists; choose a new --output: {output}")
    output.parent.mkdir(parents=True, exist_ok=True)
    temporary = output.with_name(output.name + ".partial")
    if temporary.exists():
        raise SystemExit(f"Previous partial archive exists: {temporary}")
    roots = [ROOT / "framework"] + [ROOT / name for name in PROJECTS if (ROOT / name).is_dir()]
    files = set()
    for base in roots:
        for path in base.rglob("*"):
            if path.is_file() and include(path, base):
                files.add(path)
    for name in ["OpenGL.props", "OPENGL_SETUP.md", "tools/Package-Framework.py",
                 "docs/obj-loader/PACKAGING.md"]:
        path = ROOT / name
        if path.is_file():
            files.add(path)
    records = []
    with zipfile.ZipFile(temporary, "x", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for path in sorted(files):
            data = path.read_bytes()
            name = path.relative_to(ROOT).as_posix()
            archive.writestr(name, data)
            records.append({"path": name, "bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()})
        archive.writestr("README_PACKAGE.md", NOTICE.encode("utf-8-sig"))
        manifest = {"kind": "source-distribution", "build_and_runtime_tests": "not_run",
                    "size_limit_bytes": LIMIT, "source_files": records,
                    "excluded": sorted(EXCLUDED_DIRS), "sdk_files": "preserved except removed Assimp"}
        archive.writestr("DISTRIBUTION_MANIFEST.json",
                         json.dumps(manifest, ensure_ascii=False, indent=2).encode("utf-8-sig"))
    size = temporary.stat().st_size
    if size >= LIMIT:
        raise SystemExit(f"Archive exceeds the limit ({size:,} >= {LIMIT:,} bytes). "
                         f"Kept the partial artifact for inspection: {temporary}")
    temporary.rename(output)
    result = {"archive": str(output), "zip_bytes": size, "decimal_MB": round(size / 1_000_000, 3),
              "MiB": round(size / 1_048_576, 3), "limit_bytes": LIMIT, "source_files": len(records),
              "uncompressed_source_bytes": sum(item["bytes"] for item in records),
              "sha256": hashlib.sha256(output.read_bytes()).hexdigest(), "build_and_runtime_tests": "not_run"}
    output.with_suffix(".manifest.json").write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n",
                                                  encoding="utf-8-sig")
    print(json.dumps(result, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()

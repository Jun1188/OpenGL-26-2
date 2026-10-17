"""Rebuild the repository's code-only graph with the installed graphify runtime."""
from pathlib import Path
import runpy
import sys
sys.argv = [sys.argv[0], '--scope', 'repository']
runpy.run_path(str(Path(__file__).resolve().parent.parent / 'framework-analysis/update_graph.py'), run_name='__main__')

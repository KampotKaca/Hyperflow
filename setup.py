#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

def run(cmd, cwd=None):
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd)
    if result.returncode != 0:
        sys.exit(result.returncode)

def main():
    root = Path(__file__).resolve().parent
    shaderc_dir = root / "external" / "shaderc"

    # Update and initialize all submodules recursively
    run(["git", "submodule", "update", "--init", "--recursive"], cwd=root)

    # Run Shaderc dependency sync script
    script = shaderc_dir / "utils" / "git-sync-deps"
    if not script.exists():
        sys.exit("Error: git-sync-deps not found at expected path.")
    run([str(script)], cwd=shaderc_dir)

if __name__ == "__main__":
    main()

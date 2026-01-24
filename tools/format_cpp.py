#!/usr/bin/env python3
import argparse
import os
import subprocess
import sys

CPP_EXTENSIONS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hpp", ".hh"}

EXCLUDE_DIRS = {
    ".git",
    "build",
    "bazel-bin",
    "bazel-out",
    "bazel-testlogs",
    ".idea",
    ".vscode",
    "third_party",
    "RoadManager",
    "CommonMini",
}

def is_cpp_file(filename: str) -> bool:
    return os.path.splitext(filename)[1] in CPP_EXTENSIONS


def format_file(file_path: str, clang_format: str, dry_run: bool):
    # Skip formatting if the file path contains any excluded directory
    components = file_path.split(os.sep)
    if any(excluded in components for excluded in EXCLUDE_DIRS):
        print("[SKIP]", file_path)
        return

    cmd = [clang_format, "-i", file_path]
    if dry_run:
        print("[DRY-RUN]", " ".join(cmd))
        return

    try:
        subprocess.check_call(cmd)
        print("[OK]", file_path)
    except subprocess.CalledProcessError as e:
        print("[ERROR]", file_path, e)


def format_directory(root: str, clang_format: str, dry_run: bool):
    for dirpath, dirnames, filenames in os.walk(root):
        # 过滤目录
        dirnames[:] = [d for d in dirnames if d not in EXCLUDE_DIRS]

        for filename in filenames:
            if is_cpp_file(filename):
                full_path = os.path.join(dirpath, filename)
                format_file(full_path, clang_format, dry_run)


def main():
    parser = argparse.ArgumentParser(
        description="Format all C/C++ files in a directory using clang-format"
    )
    parser.add_argument(
        "path", help="Root directory to format"
    )
    parser.add_argument(
        "--clang-format",
        default="clang-format",
        help="Path to clang-format binary",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Only print files to be formatted",
    )

    args = parser.parse_args()

    if not os.path.exists(args.path):
        print("Error: path does not exist:", args.path)
        sys.exit(1)

    format_directory(args.path, args.clang_format, args.dry_run)


if __name__ == "__main__":
    main()

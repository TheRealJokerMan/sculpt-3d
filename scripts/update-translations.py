#
# Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
#

import argparse
import pathlib
import subprocess


def main(preset: str) -> None:
    args = ['cmake', '--build', '--preset', preset, '--target', 'update_translations']
    cwd = pathlib.Path(__file__).resolve().parent.parent
    print(subprocess.check_output(args, cwd=cwd).decode('ascii'))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='update-translations',
                                     description='Update the Qt translation files.')
    parser.add_argument('preset', help='The name of the CMake preset to use.')
    args = parser.parse_args()
    main(args.preset)

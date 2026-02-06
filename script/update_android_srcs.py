#!/usr/bin/env python3
#
#  Copyright (c) 2026, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

#
# This script automatically updates the 'srcs' list for core and cli filegroups in the Android.bp file.
# It scans the 'src/core' and 'src/cli' directories for all .cpp files
# and then replaces the 'srcs' list within the corresponding filegroup blocks in Android.bp.
#
# Usage:
# Run this script from the root directory of the openthread project:
#   python3 script/update_android_srcs.py
#

import os
import re

EXCLUDE_CORE_SRCS = [
    "src/core/instance/extension_example.cpp",
]

def get_cpp_files(src_dir, exclude_list=None):
    """Finds all .cpp files under the src_dir, optionally excluding some."""
    if exclude_list is None:
        exclude_list = []
    cpp_files = []
    for root, dirs, files in os.walk(src_dir):
        for file in files:
            if file.endswith(".cpp"):
                full_path = os.path.join(root, file)
                if full_path not in exclude_list:
                    cpp_files.append(full_path)
    cpp_files.sort()
    return cpp_files

def update_srcs_list(bp_content, filegroup_name, src_files):
    """Updates the srcs list in the Android.bp content for the given filegroup."""
    srcs_items = []
    for src_file in src_files:
        srcs_items.append(f'        "{src_file}",')
    srcs_items_str = "\n".join(srcs_items)

    # Regex to find the srcs list within the specified filegroup
    pattern = re.compile(
        r"(filegroup\s*{{\s*name:\s*\"{}\",[\s\S]*?srcs:\s*\[\n)[\s\S]*?(\s*\],\s*}})".format(filegroup_name),
        re.MULTILINE
    )

    replacement = r"\1" + srcs_items_str + r"\2"
    new_bp_content = pattern.sub(replacement, bp_content)
    return new_bp_content

def update_android_bp_srcs(bp_file="Android.bp"):
    # Read the content of Android.bp
    with open(bp_file, "r") as f:
        bp_content = f.read()

    original_content = bp_content

    # Update for openthread_core_srcs
    core_srcs = get_cpp_files("src/core", EXCLUDE_CORE_SRCS)
    bp_content = update_srcs_list(bp_content, "openthread_core_srcs", core_srcs)
    print(f"Found {len(core_srcs)} source files for openthread_core_srcs.")

    # Update for openthread_cli_srcs
    cli_srcs = get_cpp_files("src/cli")
    bp_content = update_srcs_list(bp_content, "openthread_cli_srcs", cli_srcs)
    print(f"Found {len(cli_srcs)} source files for openthread_cli_srcs.")

    if bp_content != original_content:
        # Write the updated content back to Android.bp
        with open(bp_file, "w") as f:
            f.write(bp_content)
        print(f"Successfully updated {bp_file}.")
    else:
        print(f"No changes needed for {bp_file}.")

if __name__ == "__main__":
    update_android_bp_srcs()

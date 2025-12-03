#!/usr/bin/env python3
# Script to extract compile_time_request sections from object files
# Copyright (C) 2025  Your Name <your.email@example.com>
# This file may be distributed under the terms of the GNU GPLv3 license.

import os
import sys
import glob
import subprocess
import tempfile

def find_object_files(build_dir):
    """Find all .o files in build directory"""
    pattern = os.path.join(build_dir, "**", "*.o")
    return glob.glob(pattern, recursive=True)

def extract_ctr_section(objcopy, obj_file):
    """Extract .compile_time_request section from object file"""
    try:
        # Create temporary file for extracted section
        with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as tmp:
            tmp_path = tmp.name
        
        # Extract section using objcopy
        cmd = [
            objcopy,
            '--dump-section', '.compile_time_request=' + tmp_path,
            obj_file
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        # Read extracted data
        if os.path.exists(tmp_path) and os.path.getsize(tmp_path) > 0:
            with open(tmp_path, 'rb') as f:
                data = f.read()
            os.unlink(tmp_path)
            return data
        else:
            if os.path.exists(tmp_path):
                os.unlink(tmp_path)
            return None
            
    except Exception as e:
        print(f"Warning: Failed to extract CTR from {obj_file}: {e}", file=sys.stderr)
        return None

def main():
    if len(sys.argv) < 4:
        print("Usage: extract_compile_time_requests.py <objcopy> <build_dir> <output_file>")
        sys.exit(1)
    
    objcopy = sys.argv[1]
    build_dir = sys.argv[2]
    output_file = sys.argv[3]
    
    print(f"Extracting compile_time_requests from {build_dir}")
    
    # Find all object files
    obj_files = find_object_files(build_dir)
    print(f"Found {len(obj_files)} object files")
    
    # Extract CTR sections
    all_ctr_data = b''
    for obj_file in obj_files:
        data = extract_ctr_section(objcopy, obj_file)
        if data:
            all_ctr_data += data
    
    # Write concatenated data
    with open(output_file, 'wb') as f:
        f.write(all_ctr_data)
    
    print(f"Extracted {len(all_ctr_data)} bytes to {output_file}")
    
    # Now run buildcommands.py if it exists
    klipper_src = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    buildcommands_py = os.path.join(klipper_src, 'scripts', 'buildcommands.py')
    
    if os.path.exists(buildcommands_py):
        print(f"Running buildcommands.py...")
        
        # Set up Python path
        sys.path.insert(0, klipper_src)
        
        # Run buildcommands
        cmd = [sys.executable, buildcommands_py, output_file]
        result = subprocess.run(cmd, cwd=build_dir)
        
        if result.returncode != 0:
            print(f"Warning: buildcommands.py returned {result.returncode}")
            sys.exit(result.returncode)
    else:
        print(f"Warning: buildcommands.py not found at {buildcommands_py}")
    
    print("CTR extraction complete")

if __name__ == '__main__':
    main()

import subprocess
import re
import os

def is_package_from_local_install(package_name):
    print(f"Checking package: {package_name}")
    
    env = os.environ.copy()
    env["LC_ALL"] = "C"
    
    try:
        result = subprocess.run(
            ["apt-cache", "policy", package_name],
            capture_output=True,
            text=True,
            env=env,
            timeout=10
        )
    except subprocess.TimeoutExpired:
        print("Timeout")
        return False
        
    if result.returncode != 0:
        print("Error running apt-cache policy")
        return False
        
    output = result.stdout
    print("Output:")
    print(output)
    
    lines = output.split('\n')
    installed_version = ""
    
    # 1. Find installed version
    for line in lines:
        if line.strip().startswith("Installed:"):
            parts = line.split(':')
            if len(parts) > 1:
                installed_version = parts[1].strip()
            break
            
    print(f"Installed version: '{installed_version}'")
    
    if not installed_version or installed_version == "(none)":
        print("Not installed")
        return False
        
    # 2. Check if the installed version comes from a repository
    in_version_table = False
    checking_installed_version = False
    has_repo_source = False
    
    for line in lines:
        if line.strip().startswith("Version table:"):
            in_version_table = True
            continue
            
        if in_version_table:
            trimmed = line.strip()
            
            # Debug indentation
            indent_len = len(line) - len(line.lstrip())
            print(f"Line: {repr(line)}, Indent: {indent_len}")

            # Check for version line
            if trimmed.startswith("***"):
                # This is the currently installed version block
                parts = re.split(r'\s+', trimmed)
                if len(parts) >= 2 and parts[1] == installed_version:
                    checking_installed_version = True
                    print(f"Found installed version block: {line}")
                    continue
                else:
                    checking_installed_version = False
            # RELAXED CHECK: Check if it's a source line based on indentation or content
            # Version lines start with "***" (1 space indent) or have 5 space indent
            # Source lines have 8 space indent
            elif indent_len < 6: 
                # Likely a new version entry (less indented than sources)
                print("Likely new version entry (indent < 6)")
                checking_installed_version = False
                
            if checking_installed_version:
                print(f"Checking source line: '{line}'")
                # Check sources for this version
                # NEW LOGIC: Check for any URI scheme
                if "://" in line:
                    print(f"Found repo source (URI scheme detected): {line.strip()}")
                    has_repo_source = True
                    break
    
    print(f"Has repo source: {has_repo_source}")
    is_local = not has_repo_source
    print(f"Is local: {is_local}")
    return is_local

# Test with a common package that is likely installed from a repo
is_package_from_local_install("coreutils")
is_package_from_local_install("apt")


def parse_policy(output):
    lines = output.split('\n')
    installed_version = None
    is_local = False
    
    # helper to check if a line indicates a repo
    def is_repo_line(line):
        return "http://" in line or "ftp://" in line
        
    # 1. Find installed version
    for line in lines:
        if line.strip().startswith("Installed:"):
            installed_version = line.split(":", 1)[1].strip()
            break
            
    if not installed_version or installed_version == "(none)":
        return False # Not installed
        
    print(f"Installed version: {installed_version}")
    
    # 2. Find the version table entry for the installed version
    in_version_table = False
    checking_installed_version = False
    has_repo_source = False
    
    for line in lines:
        if line.strip().startswith("Version table:"):
            in_version_table = True
            continue
            
        if in_version_table:
            # Check for version line (starts with *** or just spaces)
            # Example: " *** 5.2.37-2ubuntu5 500"
            # or "     5.2.37-2ubuntu5 500"
            
            trimmed = line.strip()
            if trimmed.startswith("***"):
                # This is the installed version block
                parts = trimmed.split()
                if len(parts) >= 2 and parts[1] == installed_version:
                    checking_installed_version = True
                    continue
                else:
                    checking_installed_version = False
            elif trimmed and not line.startswith("        "): # New version entry (indented less?)
                # Actually version lines are indented by 1 space, sources by 8 spaces
                # " *** 1.2.3" -> indented 1 space
                # "     1.2.3" -> indented 5 spaces
                # "        500 http..." -> indented 8 spaces
                
                # If we hit a new version line, stop checking
                # A version line usually starts with a version number or ***
                pass
                
            if checking_installed_version:
                # Check sources for this version
                if is_repo_line(line):
                    has_repo_source = True
                    
    # If we found the installed version and it has NO repo source, it's local
    # But wait, if it's installed from a .deb, it ONLY has /var/lib/dpkg/status
    # If it's installed from repo, it has /var/lib/dpkg/status AND http://...
    
    # Let's re-verify this assumption.
    # I'll simulate some outputs.
    return not has_repo_source

# Test cases
output_repo = """bash:
  Installed: 5.2.37-2ubuntu5
  Candidate: 5.2.37-2ubuntu5
  Version table:
 *** 5.2.37-2ubuntu5 500
        500 http://archive.ubuntu.com/ubuntu questing/main amd64 Packages
        100 /var/lib/dpkg/status"""

output_local = """mypackage:
  Installed: 1.0-local
  Candidate: 1.0-local
  Version table:
 *** 1.0-local 100
        100 /var/lib/dpkg/status"""

output_local_with_repo_avail = """mypackage:
  Installed: 1.0-local
  Candidate: 1.0-local
  Version table:
 *** 1.0-local 100
        100 /var/lib/dpkg/status
     0.9-repo 500
        500 http://archive.ubuntu.com/ubuntu questing/main amd64 Packages"""

print(f"Repo package is local? {parse_policy(output_repo)}")
print(f"Local package is local? {parse_policy(output_local)}")
print(f"Local package (with repo avail) is local? {parse_policy(output_local_with_repo_avail)}")

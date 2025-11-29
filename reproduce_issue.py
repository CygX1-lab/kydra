import subprocess
import os

def test_parse_deb(file_path):
    print(f"Testing parseDebFile on {file_path}")
    if not os.path.exists(file_path):
        print("File does not exist")
        return

    try:
        process = subprocess.Popen(["dpkg-deb", "--info", file_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate(timeout=5)
        
        if process.returncode != 0:
            print(f"Failed to parse .deb file: {file_path}")
            print(f"Error: {stderr.decode()}")
        else:
            print("Successfully parsed .deb file")
            print(stdout.decode())
            
    except subprocess.TimeoutExpired:
        print(f"Timeout parsing .deb file: {file_path}")
        process.kill()

def test_is_local_install(package_name):
    print(f"Testing isPackageFromLocalInstall on {package_name}")
    try:
        process = subprocess.Popen(["apt-cache", "policy", package_name], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate(timeout=10)
        
        if process.returncode != 0:
            print(f"Failed to run apt-cache policy on {package_name}")
            return

        output = stdout.decode()
        print(f"Output:\n{output}")
        
        has_local_entry = "/var/lib/dpkg/status" in output
        has_repo_entry = "http://" in output or "ftp://" in output
        
        print(f"Has local entry: {has_local_entry}")
        print(f"Has repo entry: {has_repo_entry}")
        print(f"Is local install: {has_local_entry and not has_repo_entry}")

    except subprocess.TimeoutExpired:
        print(f"Timeout running apt-cache policy")
        process.kill()

if __name__ == "__main__":
    # Test with dummy file (should fail)
    test_parse_deb("dummy.deb")
    
    # Test with a known package (e.g. bash)
    test_is_local_install("bash")

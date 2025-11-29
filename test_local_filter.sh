#!/bin/bash

# Test script to verify local package detection
# Run from the kydra directory

echo "=== Testing Local Package Detection ==="
echo

# Test a package that should NOT be local (from repository)
echo "Testing 'apt' package (should be from repo):"
apt-cache policy apt | head -n 10
echo

# Create a simple test program
cat > test_local_detection.cpp << 'EOF'
#include <iostream>
#include <cstdlib>
#include <string>

bool testPackage(const QString& packageName) {
    // Simplified version of the C++ logic
    std::string cmd = "LC_ALL=C apt-cache policy " + std::string(packageName.toStdString());
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return false;
    
    char buffer[256];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }
    pclose(pipe);
    
    // Parse for installed version
    size_t pos = output.find("Installed:");
    if (pos == std::string::npos) return false;
    
    // Find if version table has :// in source lines
    pos = output.find("Version table:");
    if (pos == std::string::npos) return false;
    
    size_t uri_pos = output.find("://", pos);
    
    // If we found :// after the version table, it's from a repo
    return uri_pos == std::string::npos; // true if local (no ://)
}

int main() {
    std::cout << "This would need Qt to compile properly\n";
    return 0;
}
EOF

echo "Simple test: checking if apt has :// in policy output"
if LC_ALL=C apt-cache policy apt | grep -q "://"; then
    echo "✓ apt has repository sources (://) - should NOT be in local filter"
else
    echo "✗ apt has NO repository sources - would incorrectly appear in local filter"
fi
echo

# Check how many installed packages would be detected as "local" with current logic
echo "Counting installed packages that appear 'local' (no :// in policy):"
count=0
total=0
for pkg in $(dpkg -l | grep '^ii' | awk '{print $2}' | head -n 20); do
    total=$((total + 1))
    if ! LC_ALL=C apt-cache policy "$pkg" 2>/dev/null | grep -q "://"; then
        count=$((count + 1))
        echo "  - $pkg appears LOCAL (no ://)"
    fi
done

echo
echo "Summary: $count out of $total packages appear 'local' with current detection"
echo "Expected: only packages manually installed from .deb files should be local"

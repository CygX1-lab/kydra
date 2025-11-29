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

#include "commandIO.hpp"
#include "session_manager.hpp"
#include <string>
#include <vector>

bool eye(std::vector<std::string> args) {
    smr::semgr << "\n\n---\neye() test text\n---\n";
    return true;
}

// bool zeros(std::vector<std::string> args) {
// }

// bool ones(std::vector<std::string> args) {
// }

// bool random(std::vector<std::string> args) {
// }

// bool randn(std::vector<std::string> args) {
// }

// bool diag(std::vector<std::string> args) {
// }

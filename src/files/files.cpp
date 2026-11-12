#include "files.hpp"

namespace fs = std::filesystem;

Files::Files(std::shared_ptr<Poddy> pod) {
    path = PodUtil::musicDir;

}
void Files::refresh() {
    fs::directory_iterator it{path};
    for (const auto &ent : it) {
        
    }
}
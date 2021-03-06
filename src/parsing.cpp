#include "parsing.hpp"

namespace parsing {

symtb::VarRef get_var(const std::string& name)
{
    auto pos = symtb::lookup_var(name);
    if (pos) {
        return *pos;
    } else {
        fprintf(stderr, "SEMANTIC ERROR (%d): ", 0); // TODO
        fprintf(stderr, "variable '%s' not declared.\n", name.c_str());
        exit(1);
    }
}

} // namespace parsing

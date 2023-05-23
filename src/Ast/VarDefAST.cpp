//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/VarDefAST.h"

void VarDefAST::Dump() const {

}

IRGen::IRBase *VarDefAST::codegen() {
    // -> IDENT
    IRGen::IRBase *res;
    auto &builder = IR::get()->getBuilder();
    if (InitVal == nullptr) { ;
    }
        // -> IDENT = InitVal
    else { ;
    }
    std::cout << "VarDefAST::codegen has been deprecated. Use VarDefAST::get_defs instead.\n";
    return nullptr;
}

std::pair<const std::string &, IRGen::IRBase *> VarDefAST::get_defs() const {
    // -> IDENT
    if (InitVal == nullptr) {
        return std::pair<const std::string &, IRGen::IRBase *>{ident, nullptr};
    }
        // -> IDENT = InitVal
    else {
        return std::pair<const std::string &, IRGen::IRBase *>{ident, InitVal->codegen()};
    }
}

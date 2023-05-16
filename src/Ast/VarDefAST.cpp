//
// Created by Hao Zhong on 4/25/23.
//

#include "Ast/VarDefAST.h"

void VarDefAST::Dump() const {

}

llvm::Value *VarDefAST::codegen() {
    // -> IDENT
    llvm::Value *res;
    auto &builder = IR::get()->getBuilder();
    if (InitVal == nullptr) { ;
    }
        // -> IDENT = InitVal
    else { ;
    }
    std::cout << "VarDefAST::codegen has been deprecated. Use VarDefAST::get_defs instead.\n";
    return nullptr;
}

std::pair<const std::string &, llvm::Value *> VarDefAST::get_defs() const {
    // -> IDENT
    if (InitVal == nullptr) {
        return std::pair<const std::string &, llvm::Value *>{ident, nullptr};
    }
        // -> IDENT = InitVal
    else {
        return std::pair<const std::string &, llvm::Value *>{ident, InitVal->codegen()};
    }
}

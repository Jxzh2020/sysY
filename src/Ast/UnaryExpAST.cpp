//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/UnaryExpAST.h"

void UnaryExpAST::Dump() const {
    std::cout << "UnaryExpAST { ";
    if (PrimaryExp == nullptr) {
        std::cout << (OpType == PLUS ? "PLUS " : OpType == MINUS ? "MINUS " : "COMPLEMENT ");
        UnaryExp->Dump();
    } else
        PrimaryExp->Dump();
    std::cout << " }";
}

IRGen::IRBase *UnaryExpAST::codegen() {
    IRGen::IRBase *res;
    auto zero_val = IRGen::Constant::get(IRGen::Type::getInt32(), 0);

    if (ident.empty()) {  // still exp, not a function call
        // UnaryOp UnaryExp form
        if (PrimaryExp == nullptr) {
            switch (OpType) {
                case PLUS:
                    // PLUS does not affect the value
                    res = UnaryExp->codegen();
                    break;
                case MINUS:
                    //res = IRGen::ConstantExpr::getSub(zero_val,IRGen::ConstantExpr::getPointerCast(UnaryExp->codegen(),IRGen::Type::getInt32PtrTy(*IR::get()->getContext())));
                    res = IR::get()->getBuilder()->CreateSub(zero_val, UnaryExp->codegen());
                    //res = IRGen::BinaryOperator::CreateNeg(UnaryExp->codegen());
                    break;
                case COMPLEMENT:
                    res = IR::get()->getBuilder()->CreateNeg(UnaryExp->codegen());
                    //res = IRGen::BinaryOperator::CreateNot(UnaryExp->codegen());
                    break;
            }
        }
            // PrimaryExp form
        else {
            res = PrimaryExp->codegen();
        }
    } else {   // a function call
        std::vector<IRGen::IRBase *> vals;
        if (params.empty()) { // no parameters
            res = IR::get()->getBuilder()->CreateCall(IR::get()->getModule()->get_function(ident));
        } else {
            for (auto &i: params)
                vals.push_back(i->codegen());
            res = IR::get()->getBuilder()->CreateCall(IR::get()->getModule()->get_function(ident), vals);
        }
    }
    return res;
}

//
// Created by Hao Zhong on 4/18/23.
//

#include "Ast/UnaryExpAST.h"

void UnaryExpAST::Dump() const {
    std::cout << "UnaryExpAST { ";
    if( PrimaryExp == nullptr){
        std::cout << (OpType == PLUS ? "PLUS " : OpType == MINUS ? "MINUS " : "COMPLEMENT ");
        UnaryExp->Dump();
    }
    else
        PrimaryExp->Dump();
    std::cout << " }";
}

llvm::Value *UnaryExpAST::codegen() const {
    llvm::Value* res;
    auto zero_val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*IR::get()->getContext()), 0);

    if(ident.empty()){  // still exp, not a function call
        // UnaryOp UnaryExp form
        if( PrimaryExp == nullptr){
            switch(OpType){
                case PLUS:
                    // PLUS does not affect the value
                    res = UnaryExp->codegen();
                    break;
                case MINUS:
                    //res = llvm::ConstantExpr::getSub(zero_val,llvm::ConstantExpr::getPointerCast(UnaryExp->codegen(),llvm::Type::getInt32PtrTy(*IR::get()->getContext())));
                    res = IR::get()->getBuilder()->CreateSub(zero_val,UnaryExp->codegen());
                    //res = llvm::BinaryOperator::CreateNeg(UnaryExp->codegen());
                    break;
                case COMPLEMENT:
                    res = IR::get()->getBuilder()->CreateNeg(UnaryExp->codegen());
                    //res = llvm::BinaryOperator::CreateNot(UnaryExp->codegen());
                    break;
            }
        }
        // PrimaryExp form
        else{
            res = PrimaryExp->codegen();
        }
    }
    else{   // a function call
        std::vector<llvm::Value*> vals;
        if(params.empty()){ // no parameters
            res = IR::get()->getBuilder()->CreateCall(IR::get()->getModule()->getFunction(ident));
        }
        else{
            for(auto &i: params)
                vals.push_back(i->codegen());
            res = IR::get()->getBuilder()->CreateCall(IR::get()->getModule()->getFunction(ident),vals);
        }
    }
    return res;
}

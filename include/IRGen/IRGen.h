//
// Created by Hao Zhong on 5/15/23.
//

#ifndef SYSY_IRGEN_H
#define SYSY_IRGEN_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <list>


/** IRGen namespace:
 *  IRGen namespace implements the part of LLVM Lib used in sysY project
 *  Within the namespace, memory recycle is implemented to avoid leakage.
 */
namespace IRGen {
    enum IR_TYPE {
        IR_INT,
        IR_VOID
    };

    // declare classes
    class IRBase;
    class Type;
    class Builder;
    class FunctionType;
    class Function;


    class Builder {
        ;
    };

    /** Type class
     *  cope with all the primitive types and complex type
     */
    class Type {
    public:
        static Type getInt32();
//        static Type getInt32();
        static Type getVoid();
//        static Type getInt32();
    private:
        ;
    };
    class IRBase {
    public:
        [[nodiscard]] const std::string& get_name() const { return this->name; }
        [[nodiscard]] const Type& get_type() const { return this->type; }
    private:
        std::string name;
        Type type;
    };
    class FunctionType {
        ;
    };
    class BasicBlock {

    };

    /**
     *
     */
    class Module {
    public:
        ;
    private:
        std::unordered_map<std::string,Function*> func_list;
    };
    class Function {
        static Function* Create(FunctionType*, const std::string& name, Module&);
    };
};


#endif //SYSY_IRGEN_H

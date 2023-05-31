//
// Created by Hao Zhong on 4/18/23.
//

#ifndef SYSY_BLOCKAST_H
#define SYSY_BLOCKAST_H

#include "BaseAST.h"
#include "BlockItemAST.h"

class BlockAST : public BaseAST {
public:
  std::vector<std::unique_ptr<BaseAST>> BlockItems;

  std::string astJson(int size) override;
  [[nodiscard]] bool isBranch() const;

  [[nodiscard]] IRGen::IRBase *codegen() override;
};

#endif // SYSY_BLOCKAST_H

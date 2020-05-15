#include "Visitors/Passes/PassManager.hpp"

void PassManager::runAll(INode *ast) {
    for (auto &pass : passes) {
        ast->accept(*pass);
    }
}

void PassManager::addPass(OptimizationPass *pass) {
    passes.emplace_back(pass);
}

void PassManager::dumpStats() const {
    for (auto &pass : passes) {
        pass->dumpStats();
    }
}

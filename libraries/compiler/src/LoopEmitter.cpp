#include "LoopEmitter.h"
#include "FunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		namespace ir
		{
			const std::string ForInitBlockName = "for.init";
			const std::string ForConditionBlockName = "for.cond";
			const std::string ForBodyBlockName = "for.body";
			const std::string ForIncBlockName = "for.inc";
			const std::string ForAfterBlockName = "for.after";

			void ForLoopEmitter::End()
			{
				assert(_pIncBlock != nullptr);
				_fn.Branch(_pIncBlock);
				_fn.CurrentBlock(_pAfterBlock);
			}

			llvm::BasicBlock* ForLoopEmitter::Begin(const int iStartAt, const int iMaxValue, const int stepSize)
			{
				CreateBlocks();
				EmitIterationVar(ValueType::Int32, _fn.Literal(iStartAt));
				EmitCondition(ComparisonType::Lt, _fn.Literal(iMaxValue));
				EmitIncrement(ValueType::Int32, _fn.Literal(stepSize));
				return PrepareBody();
			}
			
			void ForLoopEmitter::CreateBlocks()
			{
				_pInitBlock = _fn.Block(ForInitBlockName);
				_pConditionBlock = _fn.Block(ForConditionBlockName);
				_pBodyBlock = _fn.Block(ForBodyBlockName);
				_pIncBlock = _fn.Block(ForIncBlockName);
				_pAfterBlock = _fn.Block(ForAfterBlockName);
			}

			void ForLoopEmitter::EmitIterationVar(ValueType type, llvm::Value* pStartValue)
			{
				_fn.Branch(_pInitBlock);
				_fn.CurrentBlock(_pInitBlock);
				_pIterationVar = _fn.Var(type);
				_fn.Store(_pIterationVar, pStartValue);
			}

			void ForLoopEmitter::EmitCondition(ComparisonType type, llvm::Value* pTestValue)
			{
				_fn.Branch(_pConditionBlock);
				_fn.CurrentBlock(_pConditionBlock);
				llvm::Value* pCmp = _fn.Cmp(type, _fn.Load(_pIterationVar), pTestValue);
				_fn.Branch(pCmp, _pBodyBlock, _pAfterBlock);
			}

			void ForLoopEmitter::EmitIncrement(ValueType type, llvm::Value* pIncValue)
			{
				_fn.CurrentBlock(_pIncBlock);
				_fn.OpAndUpdate(_pIterationVar, OperatorType::Add, pIncValue);
				_fn.Branch(_pConditionBlock);
			}

			llvm::BasicBlock* ForLoopEmitter::PrepareBody()
			{
				_fn.CurrentBlock(_pBodyBlock);
				return _pBodyBlock;
			}
		}
	}
}
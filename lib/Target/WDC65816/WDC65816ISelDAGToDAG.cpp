//===- WDC65816ISelDAGToDAG.cpp - A dag to dag inst selector for WDC65816 -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the WDC65816 target.
//
//===----------------------------------------------------------------------===//

#include "WDC65816TargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===--------------------------------------------------------------------===//
/// WDC65816DAGToDAGISel - WDC65816 specific code to select WDC65816 machine
/// instructions for SelectionDAG operations.
///
namespace {
    class WDC65816DAGToDAGISel : public SelectionDAGISel {
        const WDC65816Subtarget &Subtarget;
        WDC65816TargetMachine &TM;
    public:
        explicit WDC65816DAGToDAGISel(WDC65816TargetMachine &tm)
        : SelectionDAGISel(tm), Subtarget(tm.getSubtarget<WDC65816Subtarget>()),
        TM(tm) {
        }
        
        SDNode *Select(SDNode *N);
        
        bool SelectAbs(SDValue N, SDValue &R1);
        bool SelectAbsLong(SDValue N, SDValue &R1);
        bool SelectDirectPage(SDValue N, SDValue &R1);
        
#if 0 // WDC_TODO - we don't need these pattern selectors but we will need some
        // Complex Pattern Selectors.
        bool SelectADDRrr(SDValue N, SDValue &R1, SDValue &R2);
        bool SelectADDRri(SDValue N, SDValue &Base, SDValue &Offset);
        
        /// SelectInlineAsmMemoryOperand - Implement addressing mode selection for
        /// inline asm expressions.
        virtual bool SelectInlineAsmMemoryOperand(const SDValue &Op,
                                                  char ConstraintCode,
                                                  std::vector<SDValue> &OutOps);
#endif
        
        virtual const char *getPassName() const {
            return "WDC65816 DAG->DAG Pattern Instruction Selection";
        }
        
        // Include the pieces autogenerated from the target description.
#include "WDC65816GenDAGISel.inc"
        
    private:
        SDNode* getGlobalBaseReg();
    };
}  // end anonymous namespace

SDNode* WDC65816DAGToDAGISel::getGlobalBaseReg() {
    unsigned GlobalBaseReg = TM.getInstrInfo()->getGlobalBaseReg(MF);
    WDC_LOG("WDC_TODO - This method is probably implemented wrong...");
    return CurDAG->getRegister(GlobalBaseReg,
                               getTargetLowering()->getPointerTy()).getNode();
}




bool WDC65816DAGToDAGISel::SelectAbs(SDValue N, SDValue &R1) {
    WDC_LOG("WDC_TODO - Unimplemented method called");
    return false;
}


bool WDC65816DAGToDAGISel::SelectAbsLong(SDValue N, SDValue &R1) {
    WDC_LOG("WDC_TODO - Unimplemented method called");
    return false;
}


bool WDC65816DAGToDAGISel::SelectDirectPage(SDValue N, SDValue &R1) {
    WDC_LOG("WDC_TODO - Unimplemented method called");
    return false;
}


#if 0 // WDC_TODO - we don't need these exactly, we need out own
bool SparcDAGToDAGISel::SelectADDRri(SDValue Addr,
                                     SDValue &Base, SDValue &Offset) {
    if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(),
                                           getTargetLowering()->getPointerTy());
        Offset = CurDAG->getTargetConstant(0, MVT::i32);
        return true;
    }
    if (Addr.getOpcode() == ISD::TargetExternalSymbol ||
        Addr.getOpcode() == ISD::TargetGlobalAddress ||
        Addr.getOpcode() == ISD::TargetGlobalTLSAddress)
        return false;  // direct calls.
    
    if (Addr.getOpcode() == ISD::ADD) {
        if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1))) {
            if (isInt<13>(CN->getSExtValue())) {
                if (FrameIndexSDNode *FIN =
                    dyn_cast<FrameIndexSDNode>(Addr.getOperand(0))) {
                    // Constant offset from frame ref.
                    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(),
                                                       getTargetLowering()->getPointerTy());
                } else {
                    Base = Addr.getOperand(0);
                }
                Offset = CurDAG->getTargetConstant(CN->getZExtValue(), MVT::i32);
                return true;
            }
        }
        if (Addr.getOperand(0).getOpcode() == SPISD::Lo) {
            Base = Addr.getOperand(1);
            Offset = Addr.getOperand(0).getOperand(0);
            return true;
        }
        if (Addr.getOperand(1).getOpcode() == SPISD::Lo) {
            Base = Addr.getOperand(0);
            Offset = Addr.getOperand(1).getOperand(0);
            return true;
        }
    }
    Base = Addr;
    Offset = CurDAG->getTargetConstant(0, MVT::i32);
    return true;
}

bool SparcDAGToDAGISel::SelectADDRrr(SDValue Addr, SDValue &R1, SDValue &R2) {
    if (Addr.getOpcode() == ISD::FrameIndex) return false;
    if (Addr.getOpcode() == ISD::TargetExternalSymbol ||
        Addr.getOpcode() == ISD::TargetGlobalAddress ||
        Addr.getOpcode() == ISD::TargetGlobalTLSAddress)
        return false;  // direct calls.
    
    if (Addr.getOpcode() == ISD::ADD) {
        if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1)))
            if (isInt<13>(CN->getSExtValue()))
                return false;  // Let the reg+imm pattern catch this!
        if (Addr.getOperand(0).getOpcode() == SPISD::Lo ||
            Addr.getOperand(1).getOpcode() == SPISD::Lo)
            return false;  // Let the reg+imm pattern catch this!
        R1 = Addr.getOperand(0);
        R2 = Addr.getOperand(1);
        return true;
    }
    
    R1 = Addr;
    R2 = CurDAG->getRegister(SP::G0, getTargetLowering()->getPointerTy());
    return true;
}
#endif

SDNode *WDC65816DAGToDAGISel::Select(SDNode *N) {
    SDLoc dl(N);
    if (N->isMachineOpcode()) {
        N->setNodeId(-1);
        return NULL;   // Already selected.
    }
    
    WDC_LOG("WDC_TODO - Unimplemented method called, opcode=" << N->getOpcode());
    // Uncomment the following line to see the node info.
    // N->dumprFull(CurDAG);
    switch (N->getOpcode()) {
        default: break;
            
#if 0 // WDC_TODO - Do I need any of this
        case SPISD::GLOBAL_BASE_REG:
            return getGlobalBaseReg();
            
        case ISD::SDIV:
        case ISD::UDIV: {
            // sdivx / udivx handle 64-bit divides.
            if (N->getValueType(0) == MVT::i64)
                break;
            // FIXME: should use a custom expander to expose the SRA to the dag.
            SDValue DivLHS = N->getOperand(0);
            SDValue DivRHS = N->getOperand(1);
            
            // Set the Y register to the high-part.
            SDValue TopPart;
            if (N->getOpcode() == ISD::SDIV) {
                TopPart = SDValue(CurDAG->getMachineNode(SP::SRAri, dl, MVT::i32, DivLHS,
                                                         CurDAG->getTargetConstant(31, MVT::i32)), 0);
            } else {
                TopPart = CurDAG->getRegister(SP::G0, MVT::i32);
            }
            TopPart = SDValue(CurDAG->getMachineNode(SP::WRYrr, dl, MVT::Glue, TopPart,
                                                     CurDAG->getRegister(SP::G0, MVT::i32)), 0);
            
            // FIXME: Handle div by immediate.
            unsigned Opcode = N->getOpcode() == ISD::SDIV ? SP::SDIVrr : SP::UDIVrr;
            return CurDAG->SelectNodeTo(N, Opcode, MVT::i32, DivLHS, DivRHS,
                                        TopPart);
        }
        case ISD::MULHU:
        case ISD::MULHS: {
            // FIXME: Handle mul by immediate.
            SDValue MulLHS = N->getOperand(0);
            SDValue MulRHS = N->getOperand(1);
            unsigned Opcode = N->getOpcode() == ISD::MULHU ? SP::UMULrr : SP::SMULrr;
            SDNode *Mul = CurDAG->getMachineNode(Opcode, dl, MVT::i32, MVT::Glue,
                                                 MulLHS, MulRHS);
            // The high part is in the Y register.
            return CurDAG->SelectNodeTo(N, SP::RDY, MVT::i32, SDValue(Mul, 1));
        }
#endif
    }
    
    return SelectCode(N);
}


#if 0 // WDC_TODO - Not sure what this is about...
/// SelectInlineAsmMemoryOperand - Implement addressing mode selection for
/// inline asm expressions.
bool
SparcDAGToDAGISel::SelectInlineAsmMemoryOperand(const SDValue &Op,
                                                char ConstraintCode,
                                                std::vector<SDValue> &OutOps) {
    SDValue Op0, Op1;
    switch (ConstraintCode) {
        default: return true;
        case 'm':   // memory
            if (!SelectADDRrr(Op, Op0, Op1))
                SelectADDRri(Op, Op0, Op1);
            break;
    }
    
    OutOps.push_back(Op0);
    OutOps.push_back(Op1);
    return false;
}
#endif

/// createWDC65816ISelDag - This pass converts a legalized DAG into a
/// WDC65816-specific DAG, ready for instruction scheduling.
///
FunctionPass *llvm::createWDC65816ISelDag(WDC65816TargetMachine &TM) {
    WDC_LOG("ISel DAG created");
    return new WDC65816DAGToDAGISel(TM);
}

#pragma once

#include "rv_f_ext.h"

namespace Ripes {
namespace RVISA {
namespace ExtD {

struct Fld_d : public ExtF::F_Lw<Fld_d, OpcodeID::LOAD_FP, 0b011> {
  constexpr static std::string_view NAME = "fld";
};

struct Fsd_d : public ExtF::F_Sw<Fsd_d, OpcodeID::STORE_FP, 0b011> {
  constexpr static std::string_view NAME = "fsd";
};

struct Fadd_d : public ExtF::F_Instr<Fadd_d, ExtF::Funct5::FADD, ExtF::Fmt::D> {
  constexpr static std::string_view NAME = "fadd.d";
};

struct Fsub_d : public ExtF::F_Instr<Fsub_d, ExtF::Funct5::FSUB, ExtF::Fmt::D> {
  constexpr static std::string_view NAME = "fsub.d";
};

struct Fmul_d : public ExtF::F_Instr<Fmul_d, ExtF::Funct5::FMUL, ExtF::Fmt::D> {
  constexpr static std::string_view NAME = "fmul.d";
};

struct Fdiv_d : public ExtF::F_Instr<Fdiv_d, ExtF::Funct5::FDIV, ExtF::Fmt::D> {
  constexpr static std::string_view NAME = "fdiv.d";
};

struct Fsqrt_d : public ExtF::F_Sqrt<Fsqrt_d, ExtF::Fmt::D> {
  constexpr static std::string_view NAME = "fsqrt.d";
};

struct Fmin_d : public ExtF::F_Instr<Fmin_d, ExtF::Funct5::FMINMAX, ExtF::Fmt::D, ExtF::Rm::RNE> {
  constexpr static std::string_view NAME = "fmin.d";
};

struct Fmax_d : public ExtF::F_Instr<Fmax_d, ExtF::Funct5::FMINMAX, ExtF::Fmt::D, ExtF::Rm::RTZ> {
  constexpr static std::string_view NAME = "fmax.d";
};

struct Fsgnj_d : public ExtF::F_Instr<Fsgnj_d, ExtF::Funct5::FSGNJ, ExtF::Fmt::D, ExtF::Rm::RNE> {
  constexpr static std::string_view NAME = "fsgnj.d";
};

struct Fsgnjn_d : public ExtF::F_Instr<Fsgnjn_d, ExtF::Funct5::FSGNJ, ExtF::Fmt::D, ExtF::Rm::RTZ> {
  constexpr static std::string_view NAME = "fsgnjn.d";
};

struct Fsgnjx_d : public ExtF::F_Instr<Fsgnjx_d, ExtF::Funct5::FSGNJ, ExtF::Fmt::D, ExtF::Rm::RDN> {
  constexpr static std::string_view NAME = "fsgnjx.d";
};

struct Fcvt_s_d : public ExtF::F_Cvt_Fmt<Fcvt_s_d, ExtF::Fmt::S, ExtF::Rs2::D> {
  constexpr static std::string_view NAME = "fcvt.s.d";
};

struct Fcvt_d_s : public ExtF::F_Cvt_Fmt<Fcvt_d_s, ExtF::Fmt::D, ExtF::Rs2::S> {
  constexpr static std::string_view NAME = "fcvt.d.s";
};

void enableExt(const ISAInfoBase *isa, InstrVec &instructions,
               PseudoInstrVec &pseudoInstructions);

} // namespace ExtD
} // namespace RVISA
} // namespace Ripes

OpcodeInfo(nop,                     "NOP",                 0)
OpcodeInfo(ld_bc_u16,               "LD BC, $%04X",        2)
OpcodeInfo(ld_deref_bc_a,           "LD (BC), A",          0)
OpcodeInfo(inc_bc,                  "INC BC",              0)
OpcodeInfo(inc_b,                   "INC B",               0)
OpcodeInfo(dec_b,                   "DEC B",               0)
OpcodeInfo(ld_b_u8,                 "LD B, $%02X",         1)
OpcodeInfo(rlca,                    "RLCA",                0)
OpcodeInfo(ld_deref_u16_sp,         "LD ($%04X), SP",      2)
OpcodeInfo(add_hl_bc,               "ADD HL, BC",          0)
OpcodeInfo(ld_a_deref_bc,           "LD A, (BC)",          0)
OpcodeInfo(dec_bc,                  "DEC BC",              0)
OpcodeInfo(inc_c,                   "INC C",               0)
OpcodeInfo(dec_c,                   "DEC C",               0)
OpcodeInfo(ld_c_u8,                 "LD C, $%02X",         1)
OpcodeInfo(rrca,                    "RRCA",                0)
OpcodeInfo(stop,                    "STOP",                1)
OpcodeInfo(ld_de_u16,               "LD DE, $%04X",        2)
OpcodeInfo(ld_deref_de_a,           "LD (DE), A",          0)
OpcodeInfo(inc_de,                  "INC DE",              0)
OpcodeInfo(inc_d,                   "INC D",               0)
OpcodeInfo(dec_d,                   "DEC D",               0)
OpcodeInfo(ld_d_u8,                 "LD D, $%02X",         1)
OpcodeInfo(rla,                     "RLA",                 0)
OpcodeInfo(jr_i8,                   "JR $%02X",            1)
OpcodeInfo(add_hl_de,               "ADD HL, DE",          0)
OpcodeInfo(ld_a_deref_de,           "LD A, (DE)",          0)
OpcodeInfo(dec_de,                  "DEC DE",              0)
OpcodeInfo(inc_e,                   "INC E",               0)
OpcodeInfo(dec_e,                   "DEC E",               0)
OpcodeInfo(ld_e_u8,                 "LD E, $%02X",         1)
OpcodeInfo(rra,                     "RRA",                 0)
OpcodeInfo(jr_nz_i8,                "JR NZ, $%02X",        1)
OpcodeInfo(ld_hl_u16,               "LD HL, $%04X",        2)
OpcodeInfo(ld_deref_hl_incr_a,      "LD (HL+), A",         0)
OpcodeInfo(inc_hl,                  "INC HL",              0)
OpcodeInfo(inc_h,                   "INC H",               0)
OpcodeInfo(dec_h,                   "DEC H",               0)
OpcodeInfo(ld_h_u8,                 "LD H, $%02X",         1)
OpcodeInfo(daa,                     "DAA",                 0)
OpcodeInfo(jr_z_i8,                 "JR Z, $%02X",         1)
OpcodeInfo(add_hl_hl,               "ADD HL, HL",          0)
OpcodeInfo(ldi_a_deref_hl,          "LD A, (HL+)",         0)
OpcodeInfo(dec_hl,                  "DEC HL",              0)
OpcodeInfo(inc_l,                   "INC L",               0)
OpcodeInfo(dec_l,                   "DEC L",               0)
OpcodeInfo(ld_l_u8,                 "LD L, $%02X",         1)
OpcodeInfo(cpl,                     "CPL",                 0)
OpcodeInfo(jr_nc_i8,                "JR NC, $%02X",        1)
OpcodeInfo(ld_sp_u16,               "LD SP, $%04X",        2)
OpcodeInfo(ldd_deref_hl_a,          "LD (HL-), A",         0)
OpcodeInfo(inc_sp,                  "INC SP",              0)
OpcodeInfo(inc_deref_hl,            "INC (HL)",            0)
OpcodeInfo(dec_deref_hl,            "DEC (HL)",            0)
OpcodeInfo(ld_deref_hl_u8,          "LD (HL), $%02X",      1)
OpcodeInfo(scf,                     "SCF",                 0)
OpcodeInfo(jr_c_i8,                 "JR C, $%02X",         1)
OpcodeInfo(add_hl_sp,               "ADD HL, SP",          0)
OpcodeInfo(ldd_a_deref_hl,          "LD A, (HL-)",         0)
OpcodeInfo(dec_sp,                  "DEC SP",              0)
OpcodeInfo(inc_a,                   "INC A",               0)
OpcodeInfo(dec_a,                   "DEC A",               0)
OpcodeInfo(ld_a_u8,                 "LD A, $%02X",         1)
OpcodeInfo(ccf,                     "CCF",                 0)
OpcodeInfo(ld_b_b,                  "LD B, B",             0)
OpcodeInfo(ld_b_c,                  "LD B, C",             0)
OpcodeInfo(ld_b_d,                  "LD B, D",             0)
OpcodeInfo(ld_b_e,                  "LD B, E",             0)
OpcodeInfo(ld_b_h,                  "LD B, H",             0)
OpcodeInfo(ld_b_l,                  "LD B, L",             0)
OpcodeInfo(ld_b_deref_hl,           "LD B, (HL)",          0)
OpcodeInfo(ld_b_a,                  "LD B, A",             0)
OpcodeInfo(ld_c_b,                  "LD C, B",             0)
OpcodeInfo(ld_c_c,                  "LD C, C",             0)
OpcodeInfo(ld_c_d,                  "LD C, D",             0)
OpcodeInfo(ld_c_e,                  "LD C, E",             0)
OpcodeInfo(ld_c_h,                  "LD C, H",             0)
OpcodeInfo(ld_c_l,                  "LD C, L",             0)
OpcodeInfo(ld_c_deref_hl,           "LD C, (HL)",          0)
OpcodeInfo(ld_c_a,                  "LD C, A",             0)
OpcodeInfo(ld_d_b,                  "LD D, B",             0)
OpcodeInfo(ld_d_c,                  "LD D, C",             0)
OpcodeInfo(ld_d_d,                  "LD D, D",             0)
OpcodeInfo(ld_d_e,                  "LD D, E",             0)
OpcodeInfo(ld_d_h,                  "LD D, H",             0)
OpcodeInfo(ld_d_l,                  "LD D, L",             0)
OpcodeInfo(ld_d_deref_hl,           "LD D, (HL)",          0)
OpcodeInfo(ld_d_a,                  "LD D, A",             0)
OpcodeInfo(ld_e_b,                  "LD E, B",             0)
OpcodeInfo(ld_e_c,                  "LD E, C",             0)
OpcodeInfo(ld_e_d,                  "LD E, D",             0)
OpcodeInfo(ld_e_e,                  "LD E, E",             0)
OpcodeInfo(ld_e_h,                  "LD E, H",             0)
OpcodeInfo(ld_e_l,                  "LD E, L",             0)
OpcodeInfo(ld_e_deref_hl,           "LD E, (HL)",          0)
OpcodeInfo(ld_e_a,                  "LD E, A",             0)
OpcodeInfo(ld_h_b,                  "LD H, B",             0)
OpcodeInfo(ld_h_c,                  "LD H, C",             0)
OpcodeInfo(ld_h_d,                  "LD H, D",             0)
OpcodeInfo(ld_h_e,                  "LD H, E",             0)
OpcodeInfo(ld_h_h,                  "LD H, H",             0)
OpcodeInfo(ld_h_l,                  "LD H, L",             0)
OpcodeInfo(ld_h_deref_hl,           "LD H, (HL)",          0)
OpcodeInfo(ld_h_a,                  "LD H, A",             0)
OpcodeInfo(ld_l_b,                  "LD L, B",             0)
OpcodeInfo(ld_l_c,                  "LD L, C",             0)
OpcodeInfo(ld_l_d,                  "LD L, D",             0)
OpcodeInfo(ld_l_e,                  "LD L, E",             0)
OpcodeInfo(ld_l_h,                  "LD L, H",             0)
OpcodeInfo(ld_l_l,                  "LD L, L",             0)
OpcodeInfo(ld_l_deref_hl,           "LD L, (HL)",          0)
OpcodeInfo(ld_l_a,                  "LD L, A",             0)
OpcodeInfo(ld_deref_hl_b,           "LD (HL), B",          0)
OpcodeInfo(ld_deref_hl_c,           "LD (HL), C",          0)
OpcodeInfo(ld_deref_hl_d,           "LD (HL), D",          0)
OpcodeInfo(ld_deref_hl_e,           "LD (HL), E",          0)
OpcodeInfo(ld_deref_hl_h,           "LD (HL), H",          0)
OpcodeInfo(ld_deref_hl_l,           "LD (HL), L",          0)
OpcodeInfo(halt,                    "HALT",                0)
OpcodeInfo(ld_deref_hl_a,           "LD (HL), A",          0)
OpcodeInfo(ld_a_b,                  "LD A, B",             0)
OpcodeInfo(ld_a_c,                  "LD A, C",             0)
OpcodeInfo(ld_a_d,                  "LD A, D",             0)
OpcodeInfo(ld_a_e,                  "LD A, E",             0)
OpcodeInfo(ld_a_h,                  "LD A, H",             0)
OpcodeInfo(ld_a_l,                  "LD A, L",             0)
OpcodeInfo(ld_a_deref_hl,           "LD A, (HL)",          0)
OpcodeInfo(ld_a_a,                  "LD A, A",             0)
OpcodeInfo(add_a_b,                 "ADD A, B",            0)
OpcodeInfo(add_a_c,                 "ADD A, C",            0)
OpcodeInfo(add_a_d,                 "ADD A, D",            0)
OpcodeInfo(add_a_e,                 "ADD A, E",            0)
OpcodeInfo(add_a_h,                 "ADD A, H",            0)
OpcodeInfo(add_a_l,                 "ADD A, L",            0)
OpcodeInfo(add_a_deref_hl,          "ADD A, (HL)",         0)
OpcodeInfo(add_a_a,                 "ADD A, A",            0)
OpcodeInfo(adc_a_b,                 "ADC A, B",            0)
OpcodeInfo(adc_a_c,                 "ADC A, C",            0)
OpcodeInfo(adc_a_d,                 "ADC A, D",            0)
OpcodeInfo(adc_a_e,                 "ADC A, E",            0)
OpcodeInfo(adc_a_h,                 "ADC A, H",            0)
OpcodeInfo(adc_a_l,                 "ADC A, L",            0)
OpcodeInfo(adc_a_deref_hl,          "ADC A, (HL)",         0)
OpcodeInfo(adc_a_a,                 "ADC A, A",            0)
OpcodeInfo(sub_a_b,                 "SUB A, B",            0)
OpcodeInfo(sub_a_c,                 "SUB A, C",            0)
OpcodeInfo(sub_a_d,                 "SUB A, D",            0)
OpcodeInfo(sub_a_e,                 "SUB A, E",            0)
OpcodeInfo(sub_a_h,                 "SUB A, H",            0)
OpcodeInfo(sub_a_l,                 "SUB A, L",            0)
OpcodeInfo(sub_a_deref_hl,          "SUB A, (HL)",         0)
OpcodeInfo(sub_a_a,                 "SUB A, A",            0)
OpcodeInfo(sbc_a_b,                 "SBC A, B",            0)
OpcodeInfo(sbc_a_c,                 "SBC A, C",            0)
OpcodeInfo(sbc_a_d,                 "SBC A, D",            0)
OpcodeInfo(sbc_a_e,                 "SBC A, E",            0)
OpcodeInfo(sbc_a_h,                 "SBC A, H",            0)
OpcodeInfo(sbc_a_l,                 "SBC A, L",            0)
OpcodeInfo(sbc_a_deref_hl,          "SBC A, (HL)",         0)
OpcodeInfo(sbc_a_a,                 "SBC A, A",            0)
OpcodeInfo(and_a_b,                 "AND A, B",            0)
OpcodeInfo(and_a_c,                 "AND A, C",            0)
OpcodeInfo(and_a_d,                 "AND A, D",            0)
OpcodeInfo(and_a_e,                 "AND A, E",            0)
OpcodeInfo(and_a_h,                 "AND A, H",            0)
OpcodeInfo(and_a_l,                 "AND A, L",            0)
OpcodeInfo(and_a_deref_hl,          "AND A, (HL)",         0)
OpcodeInfo(and_a_a,                 "AND A, A",            0)
OpcodeInfo(xor_a_b,                 "XOR A, B",            0)
OpcodeInfo(xor_a_c,                 "XOR A, C",            0)
OpcodeInfo(xor_a_d,                 "XOR A, D",            0)
OpcodeInfo(xor_a_e,                 "XOR A, E",            0)
OpcodeInfo(xor_a_h,                 "XOR A, H",            0)
OpcodeInfo(xor_a_l,                 "XOR A, L",            0)
OpcodeInfo(xor_a_deref_hl,          "XOR A, (HL)",         0)
OpcodeInfo(xor_a_a,                 "XOR A, A",            0)
OpcodeInfo(or_a_b,                  "OR A, B",             0)
OpcodeInfo(or_a_c,                  "OR A, C",             0)
OpcodeInfo(or_a_d,                  "OR A, D",             0)
OpcodeInfo(or_a_e,                  "OR A, E",             0)
OpcodeInfo(or_a_h,                  "OR A, H",             0)
OpcodeInfo(or_a_l,                  "OR A, L",             0)
OpcodeInfo(or_a_deref_hl,           "OR A, (HL)",          0)
OpcodeInfo(or_a_a,                  "OR A, A",             0)
OpcodeInfo(cp_a_b,                  "CP A, B",             0)
OpcodeInfo(cp_a_c,                  "CP A, C",             0)
OpcodeInfo(cp_a_d,                  "CP A, D",             0)
OpcodeInfo(cp_a_e,                  "CP A, E",             0)
OpcodeInfo(cp_a_h,                  "CP A, H",             0)
OpcodeInfo(cp_a_l,                  "CP A, L",             0)
OpcodeInfo(cp_a_deref_hl,           "CP A, (HL)",          0)
OpcodeInfo(cp_a_a,                  "CP A, A",             0)
OpcodeInfo(ret_nz,                  "RET NZ",              0)
OpcodeInfo(pop_bc,                  "POP BC",              0)
OpcodeInfo(jp_nz_u16,               "JP NZ, $%04X",        2)
OpcodeInfo(jp_u16,                  "JP $%04X",            2)
OpcodeInfo(call_nz_u16,             "CALL NZ, $%04X",      2)
OpcodeInfo(push_bc,                 "PUSH BC",             0)
OpcodeInfo(add_a_u8,                "ADD A, $%02X",        1)
OpcodeInfo(rst_00h,                 "RST 00h",             0)
OpcodeInfo(ret_z,                   "RET Z",               0)
OpcodeInfo(ret,                     "RET",                 0)
OpcodeInfo(jp_z_u16,                "JP Z, $%04X",         2)
OpcodeInfoManual(prefix_cb,         "PREFIX CB",           0, Interpret_unused)
OpcodeInfo(call_z_u16,              "CALL Z, $%04X",       2)
OpcodeInfo(call_u16,                "CALL $%04X",          2)
OpcodeInfo(adc_a_u8,                "ADC A, $%02X",        1)
OpcodeInfo(rst_08h,                 "RST 08h",             0)
OpcodeInfo(ret_nc,                  "RET NC",              0)
OpcodeInfo(pop_de,                  "POP DE",              0)
OpcodeInfo(jp_nc_u16,               "JP NC, $%04X",        2)
OpcodeInfoManual(UNUSED_1,          "UNUSED",              0, Interpret_unused)
OpcodeInfo(call_nc_u16,             "CALL NC, $%04X",      2)
OpcodeInfo(push_de,                 "PUSH DE",             0)
OpcodeInfo(sub_a_u8,                "SUB A, $%02X",        1)
OpcodeInfo(rst_10h,                 "RST 10h",             0)
OpcodeInfo(ret_c,                   "RET C",               0)
OpcodeInfo(reti,                    "RETI",                0)
OpcodeInfo(jp_c_u16,                "JP C, $%04X",         2)
OpcodeInfoManual(UNUSED_2,          "UNUSED",              0, Interpret_unused)
OpcodeInfo(call_c_u16,              "CALL C, $%04X",       2)
OpcodeInfoManual(UNUSED_3,          "UNUSED",              0, Interpret_unused)
OpcodeInfo(sbc_a_u8,                "SBC A, $%02X",        1)
OpcodeInfo(rst_18h,                 "RST 18h",             0)
OpcodeInfo(ld_deref_ff00_plus_u8_a, "LD (FF00+$%02X), A",  1)
OpcodeInfo(pop_hl,                  "POP HL",              0)
OpcodeInfo(ld_deref_ff00_plus_c_a,  "LD (FF00+C), A",      0)
OpcodeInfoManual(UNUSED_4,          "UNUSED",              0, Interpret_unused)
OpcodeInfoManual(UNUSED_5,          "UNUSED",              0, Interpret_unused)
OpcodeInfo(push_hl,                 "PUSH HL",             0)
OpcodeInfo(and_a_u8,                "AND A, $%02X",        1)
OpcodeInfo(rst_20h,                 "RST 20h",             0)
OpcodeInfo(add_sp_i8,               "ADD SP, $%02X",       1)
OpcodeInfo(jp_hl,                   "JP HL",               0)
OpcodeInfo(ld_deref_u16_a,          "LD ($%04X), A",       2)
OpcodeInfoManual(UNUSED_6,          "UNUSED",              0, Interpret_unused)
OpcodeInfoManual(UNUSED_7,          "UNUSED",              0, Interpret_unused)
OpcodeInfoManual(UNUSED_8,          "UNUSED",              0, Interpret_unused)
OpcodeInfo(xor_a_u8,                "XOR A, $%02X",        1)
OpcodeInfo(rst_28h,                 "RST 28h",             0)
OpcodeInfo(ld_a_deref_ff00_plus_u8, "LD A, (FF00+$%02X)",  1)
OpcodeInfo(pop_af,                  "POP AF",              0)
OpcodeInfo(ld_a_deref_ff00_plus_c,  "LD A, (FF00+C)",      0)
OpcodeInfo(di,                      "DI",                  0)
OpcodeInfoManual(UNUSED_9,          "UNUSED",              0, Interpret_unused)
OpcodeInfo(push_af,                 "PUSH AF",             0)
OpcodeInfo(or_a_u8,                 "OR A, $%02X",         1)
OpcodeInfo(rst_30h,                 "RST 30h",             0)
OpcodeInfo(ld_hl_sp_plus_i8,        "LD HL, SP+$%02X",     1)
OpcodeInfo(ld_sp_hl,                "LD SP, HL",           0)
OpcodeInfo(ld_a_deref_u16,          "LD A, ($%04X)",       2)
OpcodeInfo(ei,                      "EI",                  0)
OpcodeInfoManual(UNUSED_10,         "UNUSED",              0, Interpret_unused)
OpcodeInfoManual(UNUSED_11,         "UNUSED",              0, Interpret_unused)
OpcodeInfo(cp_a_u8,                 "CP A, %02X",          1)
OpcodeInfo(rst_38h,                 "RST 38h",             0)
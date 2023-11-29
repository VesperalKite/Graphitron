# Diff Details

Date : 2023-11-06 13:04:27

Directory /Users/vesperal_kite/important_files/Graphitron/src

Total : 70 files,  2850 codes, -173 comments, -162 blanks, all 2515 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [include/graphitron/backend/backend.h](/include/graphitron/backend/backend.h) | C++ | -17 | -3 | -4 | -24 |
| [include/graphitron/backend/codegen_alpha.h](/include/graphitron/backend/codegen_alpha.h) | C++ | -88 | 0 | -17 | -105 |
| [include/graphitron/backend/codegen_ecp.h](/include/graphitron/backend/codegen_ecp.h) | C++ | -82 | -3 | -15 | -100 |
| [include/graphitron/backend/gen_Apply_decl.h](/include/graphitron/backend/gen_Apply_decl.h) | C++ | -43 | -3 | -6 | -52 |
| [include/graphitron/backend/gen_Expr.h](/include/graphitron/backend/gen_Expr.h) | C++ | -59 | -3 | -6 | -68 |
| [include/graphitron/backend/gen_ScatterGather_decl.h](/include/graphitron/backend/gen_ScatterGather_decl.h) | C++ | -46 | -4 | -8 | -58 |
| [include/graphitron/backend/gen_Stmt.h](/include/graphitron/backend/gen_Stmt.h) | C++ | -41 | -3 | -5 | -49 |
| [include/graphitron/backend/gen_Type.h](/include/graphitron/backend/gen_Type.h) | C++ | -23 | -4 | -4 | -31 |
| [include/graphitron/frontend/error.h](/include/graphitron/frontend/error.h) | C++ | -181 | -14 | -53 | -248 |
| [include/graphitron/frontend/fir.h](/include/graphitron/frontend/fir.h) | C++ | -574 | -12 | -219 | -805 |
| [include/graphitron/frontend/fir_context.h](/include/graphitron/frontend/fir_context.h) | C++ | -21 | -3 | -9 | -33 |
| [include/graphitron/frontend/fir_printer.h](/include/graphitron/frontend/fir_printer.h) | C++ | -81 | -44 | -16 | -141 |
| [include/graphitron/frontend/fir_visitor.h](/include/graphitron/frontend/fir_visitor.h) | C++ | -139 | -52 | -107 | -298 |
| [include/graphitron/frontend/frontend.h](/include/graphitron/frontend/frontend.h) | C++ | -14 | -3 | -7 | -24 |
| [include/graphitron/frontend/parser.h](/include/graphitron/frontend/parser.h) | C++ | -91 | -50 | -115 | -256 |
| [include/graphitron/frontend/scanner.h](/include/graphitron/frontend/scanner.h) | C++ | -23 | -3 | -8 | -34 |
| [include/graphitron/frontend/token.h](/include/graphitron/frontend/token.h) | C++ | -75 | -53 | -13 | -141 |
| [include/graphitron/midend/configuration_lower.h](/include/graphitron/midend/configuration_lower.h) | C++ | -32 | -3 | -8 | -43 |
| [include/graphitron/midend/edgeset_lower.h](/include/graphitron/midend/edgeset_lower.h) | C++ | -13 | -3 | -4 | -20 |
| [include/graphitron/midend/label_scope.h](/include/graphitron/midend/label_scope.h) | C++ | -41 | -7 | -11 | -59 |
| [include/graphitron/midend/midend.h](/include/graphitron/midend/midend.h) | C++ | -23 | -5 | -4 | -32 |
| [include/graphitron/midend/mir.h](/include/graphitron/midend/mir.h) | C++ | -539 | -146 | -223 | -908 |
| [include/graphitron/midend/mir_context.h](/include/graphitron/midend/mir_context.h) | C++ | -312 | -37 | -83 | -432 |
| [include/graphitron/midend/mir_emitter.h](/include/graphitron/midend/mir_emitter.h) | C++ | -90 | -4 | -72 | -166 |
| [include/graphitron/midend/mir_lower.h](/include/graphitron/midend/mir_lower.h) | C++ | -12 | -3 | -5 | -20 |
| [include/graphitron/midend/mir_printer.h](/include/graphitron/midend/mir_printer.h) | C++ | -26 | -3 | -6 | -35 |
| [include/graphitron/midend/mir_rewriter.h](/include/graphitron/midend/mir_rewriter.h) | C++ | -24 | -3 | -4 | -31 |
| [include/graphitron/midend/mir_visitor.h](/include/graphitron/midend/mir_visitor.h) | C++ | -123 | -5 | -76 | -204 |
| [include/graphitron/midend/parameter.h](/include/graphitron/midend/parameter.h) | C++ | -48 | -6 | -23 | -77 |
| [include/graphitron/midend/parameter_space.h](/include/graphitron/midend/parameter_space.h) | C++ | -51 | -3 | -10 | -64 |
| [include/graphitron/midend/prop_array_lower.h](/include/graphitron/midend/prop_array_lower.h) | C++ | -13 | -3 | -4 | -20 |
| [include/graphitron/midend/var.h](/include/graphitron/midend/var.h) | C++ | -36 | -7 | -10 | -53 |
| [include/graphitron/utils/command_line.h](/include/graphitron/utils/command_line.h) | C++ | -73 | -8 | -13 | -94 |
| [include/graphitron/utils/exec_cmd.h](/include/graphitron/utils/exec_cmd.h) | C++ | -10 | -3 | -4 | -17 |
| [include/graphitron/utils/scopedmap.h](/include/graphitron/utils/scopedmap.h) | C++ | -112 | -19 | -23 | -154 |
| [include/graphitron/utils/util.h](/include/graphitron/utils/util.h) | C++ | -72 | -12 | -18 | -102 |
| [src/backend/backend.cpp](/src/backend/backend.cpp) | C++ | 11 | 31 | 2 | 44 |
| [src/backend/codegen_alpha.cpp](/src/backend/codegen_alpha.cpp) | C++ | 514 | 3 | 40 | 557 |
| [src/backend/codegen_ecp.cpp](/src/backend/codegen_ecp.cpp) | C++ | 365 | 11 | 39 | 415 |
| [src/backend/gen_Apply_decl.cpp](/src/backend/gen_Apply_decl.cpp) | C++ | 57 | 3 | 5 | 65 |
| [src/backend/gen_Expr.cpp](/src/backend/gen_Expr.cpp) | C++ | 306 | 5 | 40 | 351 |
| [src/backend/gen_ScatterGather_decl.cpp](/src/backend/gen_ScatterGather_decl.cpp) | C++ | 133 | 7 | 9 | 149 |
| [src/backend/gen_Stmt.cpp](/src/backend/gen_Stmt.cpp) | C++ | 177 | 4 | 10 | 191 |
| [src/backend/gen_Type.cpp](/src/backend/gen_Type.cpp) | C++ | 43 | 5 | 8 | 56 |
| [src/default_schedule.cpp](/src/default_schedule.cpp) | C++ | 5 | 0 | 2 | 7 |
| [src/frontend/error.cpp](/src/frontend/error.cpp) | C++ | 80 | 7 | 18 | 105 |
| [src/frontend/fir.cpp](/src/frontend/fir.cpp) | C++ | 18 | 3 | 6 | 27 |
| [src/frontend/fir_printer.cpp](/src/frontend/fir_printer.cpp) | C++ | 520 | 4 | 116 | 640 |
| [src/frontend/fir_visitor.cpp](/src/frontend/fir_visitor.cpp) | C++ | 249 | 4 | 62 | 315 |
| [src/frontend/frontend.cpp](/src/frontend/frontend.cpp) | C++ | 18 | 6 | 9 | 33 |
| [src/frontend/parser.cpp](/src/frontend/parser.cpp) | C++ | 1,119 | 127 | 264 | 1,510 |
| [src/frontend/scanner.cpp](/src/frontend/scanner.cpp) | C++ | 390 | 14 | 29 | 433 |
| [src/frontend/token.cpp](/src/frontend/token.cpp) | C++ | 255 | 4 | 31 | 290 |
| [src/graphitronc.py](/src/graphitronc.py) | Python | 76 | 10 | 26 | 112 |
| [src/main.cpp](/src/main.cpp) | C++ | 48 | 10 | 16 | 74 |
| [src/midend/configuration_lower.cpp](/src/midend/configuration_lower.cpp) | C++ | 44 | 11 | 10 | 65 |
| [src/midend/edgeset_lower.cpp](/src/midend/edgeset_lower.cpp) | C++ | 23 | 3 | 2 | 28 |
| [src/midend/midend.cpp](/src/midend/midend.cpp) | C++ | 163 | 3 | 2 | 168 |
| [src/midend/mir.cpp](/src/midend/mir.cpp) | C++ | 5 | 3 | 3 | 11 |
| [src/midend/mir_context.cpp](/src/midend/mir_context.cpp) | C++ | 3 | 3 | 2 | 8 |
| [src/midend/mir_emitter.cpp](/src/midend/mir_emitter.cpp) | C++ | 750 | 53 | 155 | 958 |
| [src/midend/mir_lower.cpp](/src/midend/mir_lower.cpp) | C++ | 12 | 3 | 2 | 17 |
| [src/midend/mir_printer.cpp](/src/midend/mir_printer.cpp) | C++ | 35 | 3 | 9 | 47 |
| [src/midend/mir_visitor.cpp](/src/midend/mir_visitor.cpp) | C++ | 203 | 4 | 50 | 257 |
| [src/midend/mri_rewriter.cpp](/src/midend/mri_rewriter.cpp) | C++ | 13 | 3 | 3 | 19 |
| [src/midend/parameter.cpp](/src/midend/parameter.cpp) | C++ | 212 | 3 | 53 | 268 |
| [src/midend/parameter_space.cpp](/src/midend/parameter_space.cpp) | C++ | 52 | 3 | 2 | 57 |
| [src/midend/prop_array_lower.cpp](/src/midend/prop_array_lower.cpp) | C++ | 20 | 3 | 2 | 25 |
| [src/utils/exec_cmd.cpp](/src/utils/exec_cmd.cpp) | C++ | 13 | 3 | 2 | 18 |
| [src/utils/util.cpp](/src/utils/util.cpp) | C++ | 166 | 5 | 22 | 193 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details
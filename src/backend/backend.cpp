//
// Created by Zheng Feng on 01/05/23.
//

#include <facet/backend/backend.h>
#include <fstream>

namespace facet {
    int Backend::emitACC(std::string output_path){
        std::ofstream l2_h(output_path+"l2.h");
        std::ofstream mir_txt(output_path+"mir.txt");
        std::ofstream schedule_txt(output_path+"schedule.txt");
        GenAcc* gen_mir = new GenAcc(mir_txt, mir_context_);
        GenAcc* gen_l2 = new GenAcc(l2_h, mir_context_);
        GenAcc* gen_schedule = new GenAcc(schedule_txt, mir_context_);
        int flag1 = gen_l2->genl2_h();
        int flag2 = gen_mir->genMIR();
        int flag3 = gen_schedule->genSchedule();
        l2_h.close();
        mir_txt.close();
        schedule_txt.close();
        delete gen_l2; 
        delete gen_mir;
        delete gen_schedule;
        return flag1 | flag2 | flag3;
    }
}   
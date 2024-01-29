#_*_coding:utf8_*_
import sys
if len(sys.argv) != 4:
	print('usage: python generateXML.py {performance data} {xml template} {new xml file}')
	exit(1)

file_performance = sys.argv[1] # performance data
file_template = sys.argv[2] #xml template
file_xml = sys.argv[3] # new xml file
variables = ["system.cpu.numCycles", "system.cpu.idleCycles", "system.cpu.commit.op_class_0::total",
             "system.cpu.branchPred.condPredicted", "system.cpu.branchPred.condIncorrect", "system.cpu.committedInsts",
             "system.cpu.commit.int_insts", "system.cpu.commit.fp_insts", "system.cpu.rob.rob_reads",
             "system.cpu.rob.rob_writes", "system.cpu.rename.int_rename_lookups", "system.cpu.rename.fp_rename_lookups",
             "system.cpu.iq.int_inst_queue_reads", "system.cpu.iq.int_inst_queue_writes",
             "system.cpu.iq.int_inst_queue_wakeup_accesses", "system.cpu.iq.fp_inst_queue_reads",
             "system.cpu.iq.fp_inst_queue_writes", "system.cpu.iq.fp_inst_queue_wakeup_accesses",
             "system.cpu.int_regfile_reads", "system.cpu.fp_regfile_reads", "system.cpu.int_regfile_writes",
             "system.cpu.fp_regfile_writes", "system.cpu.commit.function_calls", "system.cpu.iq.int_alu_accesses",
             "system.cpu.iq.fp_alu_accesses", "system.cpu.itb.wrAccesses", "system.cpu.itb.wrMisses",
             "system.cpu.icache.ReadReq_accesses::total", "system.cpu.icache.ReadReq_misses::total",
             "system.cpu.icache.replacements", "system.cpu.dtb.wrAccesses", "system.cpu.dtb.wrMisses",
             "system.cpu.dcache.ReadReq_accesses::total", "system.cpu.dcache.WriteReq_accesses::total",
             "system.cpu.dcache.ReadReq_misses::total", "system.cpu.dcache.WriteReq_misses::total",
             "system.cpu.dcache.replacements", "system.cpu.branchPred.BTBLookups", "system.cpu.commit.branches",
             "system.l2.ReadExReq_accesses::total", "system.l2.ReadExReq_misses::total", "system.mem_ctrls.readReqs",
             "system.mem_ctrls.writeReqs", "system.cpu.commit.op_class_0::No_OpClass",
             "system.cpu.commit.op_class_0::IntAlu", "system.cpu.commit.op_class_0::IntMult",
             "system.cpu.commit.op_class_0::IntDiv", "system.cpu.commit.op_class_0::IprAccess",
             "system.cpu.commit.op_class_0::FloatAdd", "system.cpu.commit.op_class_0::FloatCmp",
             "system.cpu.commit.op_class_0::FloatCvt", "system.cpu.commit.op_class_0::FloatMult",
             "system.cpu.commit.op_class_0::FloatDiv", "system.cpu.commit.op_class_0::FloatMisc",
             "system.cpu.commit.op_class_0::FloatSqrt", "system.cpu.commit.op_class_0::MemRead",
             "system.cpu.commit.op_class_0::FloatMemRead", "system.cpu.commit.op_class_0::InstPrefetch",
             "system.cpu.commit.op_class_0::MemWrite", "system.cpu.commit.op_class_0::FloatMemWrite",
             "system.cpu.rename.RenamedOperands", "system.cpu.rename.int_rename_lookups",
             "system.cpu.rename.RenameLookups", "system.cpu.rename.RenamedOperands",
             "system.cpu.rename.fp_rename_lookups", "system.cpu.rename.RenameLookups",
             "system.l2.overall_accesses::total", "system.l2.writebacks::total", "system.l2.overall_misses::total",
             "system.l2.ReadExReq_misses::total", "system.l2.demand_accesses::total", "system.mem_ctrls.readReqs",
             "system.mem_ctrls.writeReqs"]
values = {}

f = open(file_performance, 'r')
lines = f.readlines()

for var in variables:
    for line in lines:
        if var in line:
            parts = line.split()
            values[var] = int(parts[1])
            break

f.close()

template_file = file_template

with open(template_file, 'r') as f:
    content = f.readlines()

for i, line in enumerate(content):
    replace1 = '"stats.system.cpu.numCycles"'
    replace2 = str(values["system.cpu.numCycles"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.idleCycles"'
    replace2 = str(values["system.cpu.idleCycles"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.op_class_0::total"'
    replace2 = str(values["system.cpu.commit.op_class_0::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.branchPred.condPredicted"'
    replace2 = str(values["system.cpu.branchPred.condPredicted"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.branchPred.condIncorrect"'
    replace2 = str(values["system.cpu.branchPred.condIncorrect"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.committedInsts"'
    replace2 = str(values["system.cpu.committedInsts"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.int_insts"'
    replace2 = str(values["system.cpu.commit.int_insts"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.fp_insts"'
    replace2 = str(values["system.cpu.commit.fp_insts"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rob.rob_reads"'
    replace2 = str(values["system.cpu.rob.rob_reads"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rob.rob_writes"'
    replace2 = str(values["system.cpu.rob.rob_writes"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.int_rename_lookups"'
    replace2 = str(values["system.cpu.rename.int_rename_lookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.fp_rename_lookups"'
    replace2 = str(values["system.cpu.rename.fp_rename_lookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.iq.int_inst_queue_reads"'
    replace2 = str(values["system.cpu.iq.int_inst_queue_reads"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.iq.int_inst_queue_writes"'
    replace2 = str(values["system.cpu.iq.int_inst_queue_writes"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.iq.int_inst_queue_wakeup_accesses"'
    replace2 = str(values["system.cpu.iq.int_inst_queue_wakeup_accesses"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.iq.fp_inst_queue_reads"'
    replace2 = str(values["system.cpu.iq.fp_inst_queue_reads"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.iq.fp_inst_queue_writes"'
    replace2 = str(values["system.cpu.iq.fp_inst_queue_writes"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.iq.fp_inst_queue_wakeup_accesses"'
    replace2 = str(values["system.cpu.iq.fp_inst_queue_wakeup_accesses"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.int_regfile_reads"'
    replace2 = str(values["system.cpu.int_regfile_reads"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.fp_regfile_reads"'
    replace2 = str(values["system.cpu.fp_regfile_reads"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.int_regfile_writes"'
    replace2 = str(values["system.cpu.int_regfile_writes"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.fp_regfile_writes"'
    replace2 = str(values["system.cpu.fp_regfile_writes"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.function_calls"'
    replace2 = str(values["system.cpu.commit.function_calls"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.iq.int_alu_accesses"'
    replace2 = str(values["system.cpu.iq.int_alu_accesses"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.iq.fp_alu_accesses"'
    replace2 = str(values["system.cpu.iq.fp_alu_accesses"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.itb.wrAccesses"'
    replace2 = str(values["system.cpu.itb.wrAccesses"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"system.cpu.itb.wrMisses"'
    replace2 = str(values["system.cpu.itb.wrMisses"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.icache.ReadReq_accesses::total"'
    replace2 = str(values["system.cpu.icache.ReadReq_accesses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.icache.ReadReq_misses::total"'
    replace2 = str(values["system.cpu.icache.ReadReq_misses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.icache.replacements"'
    replace2 = str(values["system.cpu.icache.replacements"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.dtb.wrAccesses"'
    replace2 = str(values["system.cpu.dtb.wrAccesses"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.dtb.wrMisses"'
    replace2 = str(values["system.cpu.dtb.wrMisses"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.dcache.ReadReq_accesses::total"'
    replace2 = str(values["system.cpu.dcache.ReadReq_accesses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.dcache.WriteReq_accesses::total"'
    replace2 = str(values["system.cpu.dcache.WriteReq_accesses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.dcache.ReadReq_misses::total"'
    replace2 = str(values["system.cpu.dcache.ReadReq_misses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.dcache.WriteReq_misses::total"'
    replace2 = str(values["system.cpu.dcache.WriteReq_misses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.dcache.replacements"'
    replace2 = str(values["system.cpu.dcache.replacements"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.branchPred.BTBLookups"'
    replace2 = str(values["system.cpu.branchPred.BTBLookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.branches"'
    replace2 = str(values["system.cpu.commit.branches"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.l2.ReadExReq_accesses::total"'
    replace2 = str(values["system.l2.ReadExReq_accesses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.l2.ReadExReq_misses::total"'
    replace2 = str(values["system.l2.ReadExReq_misses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.mem_ctrls.readReqs"'
    replace2 = str(values["system.mem_ctrls.readReqs"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.mem_ctrls.writeReqs"'
    replace2 = str(values["system.mem_ctrls.writeReqs"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.int_rename_lookups"'
    replace2 = str(values["system.cpu.rename.int_rename_lookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.fp_rename_lookups"'
    replace2 = str(values["system.cpu.rename.fp_rename_lookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.l2.ReadExReq_misses::total"'
    replace2 = str(values["system.l2.ReadExReq_misses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.mem_ctrls.readReqs"'
    replace2 = str(values["system.mem_ctrls.readReqs"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.mem_ctrls.writeReqs"'
    replace2 = str(values["system.mem_ctrls.writeReqs"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.numCycles - stats.system.cpu.idleCycles"'
    replace2 = str(values["system.cpu.numCycles"] - values["system.cpu.idleCycles"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.RenamedOperands * stats.system.cpu.rename.int_rename_lookups / stats.system.cpu.rename.RenameLookups"'
    replace2 = str(
        values["system.cpu.rename.RenamedOperands"] * values["system.cpu.rename.int_rename_lookups"] / values[
            "system.cpu.rename.RenameLookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.RenamedOperands * stats.system.cpu.rename.fp_rename_lookups / stats.system.cpu.rename.RenameLookups"'
    replace2 = str(values["system.cpu.rename.RenamedOperands"] * values["system.cpu.rename.fp_rename_lookups"] / values[
        "system.cpu.rename.RenameLookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.l2.overall_misses::total - stats.system.l2.ReadExReq_misses::total"'
    replace2 = str(values["system.l2.overall_misses::total"] - values["system.l2.ReadExReq_misses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.mem_ctrls.readReqs + stats.system.mem_ctrls.writeReqs"'
    replace2 = str(values["system.mem_ctrls.readReqs"] + values["system.mem_ctrls.writeReqs"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.mem_ctrls.readReqs + stats.system.mem_ctrls.writeReqs"'
    replace2 = str(values["system.mem_ctrls.readReqs"] + values["system.mem_ctrls.writeReqs"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.op_class_0::No_OpClass + stats.system.cpu.commit.op_class_0::IntAlu + stats.system.cpu.commit.op_class_0::IntMult + stats.system.cpu.commit.op_class_0::IntDiv + stats.system.cpu.commit.op_class_0::IprAccess"'
    replace2 = str(
        values["system.cpu.commit.op_class_0::No_OpClass"] + values["system.cpu.commit.op_class_0::IntAlu"] + values[
            "system.cpu.commit.op_class_0::IntMult"] + values["system.cpu.commit.op_class_0::IntDiv"] + values[
            "system.cpu.commit.op_class_0::IprAccess"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.op_class_0::FloatAdd + stats.system.cpu.commit.op_class_0::FloatCmp + stats.system.cpu.commit.op_class_0::FloatCvt + stats.system.cpu.commit.op_class_0::FloatMult + stats.system.cpu.commit.op_class_0::FloatDiv + stats.system.cpu.commit.op_class_0::FloatMisc + stats.system.cpu.commit.op_class_0::FloatSqrt"'
    replace2 = str(
        values["system.cpu.commit.op_class_0::FloatAdd"] + values["system.cpu.commit.op_class_0::FloatCmp"] + values[
            "system.cpu.commit.op_class_0::FloatCvt"] + values["system.cpu.commit.op_class_0::FloatMult"] + values[
            "system.cpu.commit.op_class_0::FloatDiv"] + values["system.cpu.commit.op_class_0::FloatMisc"] + values[
            "system.cpu.commit.op_class_0::FloatSqrt"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.op_class_0::MemRead + system.cpu.commit.op_class_0::FloatMemRead + stats.system.cpu.commit.op_class_0::InstPrefetch"'
    replace2 = str(
        values["system.cpu.commit.op_class_0::MemRead"] + values["system.cpu.commit.op_class_0::FloatMemRead"] + values[
            "system.cpu.commit.op_class_0::InstPrefetch"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.op_class_0::MemWrite + system.cpu.commit.op_class_0::FloatMemWrite"'
    replace2 = str(
        values["system.cpu.commit.op_class_0::MemWrite"] + values["system.cpu.commit.op_class_0::FloatMemWrite"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.RenamedOperands * stats.system.cpu.rename.int_rename_lookups / stats.system.cpu.rename.RenameLookups"'
    replace2 = str(
        values["system.cpu.rename.RenamedOperands"] * values["system.cpu.rename.int_rename_lookups"] / values[
            "system.cpu.rename.RenameLookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.RenamedOperands * stats.system.cpu.rename.int_rename_lookups / stats.system.cpu.rename.RenameLookups"'
    replace2 = str(
        values["system.cpu.rename.RenamedOperands"] * values["system.cpu.rename.int_rename_lookups"] / values[
            "system.cpu.rename.RenameLookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.RenamedOperands * stats.system.cpu.rename.fp_rename_lookups / stats.system.cpu.rename.RenameLookups"'
    replace2 = str(values["system.cpu.rename.RenamedOperands"] * values["system.cpu.rename.fp_rename_lookups"] / values[
        "system.cpu.rename.RenameLookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.l2.overall_misses::total - stats.system.l2.ReadExReq_misses::total"'
    replace2 = str(values["system.l2.overall_misses::total"] - values["system.l2.ReadExReq_misses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.mem_ctrls.readReqs + stats.system.mem_ctrls.writeReqs"'
    replace2 = str(values["system.mem_ctrls.readReqs"] + values["system.mem_ctrls.writeReqs"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.l2.demand_accesses::total * 2"'
    replace2 = str(values["system.l2.demand_accesses::total"] * 2)
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.l2.overall_misses::total - stats.system.l2.ReadExReq_misses::total"'
    replace2 = str(values["system.l2.overall_misses::total"] - values["system.l2.ReadExReq_misses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.l2.overall_accesses::total + stats.system.l2.writebacks::total"'
    replace2 = str(values["system.l2.overall_accesses::total"] + values["system.l2.writebacks::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.RenamedOperands * stats.system.cpu.rename.int_rename_lookups / stats.system.cpu.rename.RenameLookups"'
    replace2 = str(
        values["system.cpu.rename.RenamedOperands"] * values["system.cpu.rename.int_rename_lookups"] / values[
            "system.cpu.rename.RenameLookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.RenamedOperands * stats.system.cpu.rename.int_rename_lookups / stats.system.cpu.rename.RenameLookups"'
    replace2 = str(
        values["system.cpu.rename.RenamedOperands"] * values["system.cpu.rename.int_rename_lookups"] / values[
            "system.cpu.rename.RenameLookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.commit.op_class_0::No_OpClass + stats.system.cpu.commit.op_class_0::IntAlu +	stats.system.cpu.commit.op_class_0::IntMult + stats.system.cpu.commit.op_class_0::IntDiv + stats.system.cpu.commit.op_class_0::IprAccess"'
    replace2 = str(
        values["system.cpu.commit.op_class_0::No_OpClass"] + values["system.cpu.commit.op_class_0::IntAlu"] + values[
            "system.cpu.commit.op_class_0::IntMult"] + values["system.cpu.commit.op_class_0::IntDiv"] + values[
            "system.cpu.commit.op_class_0::IprAccess"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.cpu.rename.RenamedOperands * stats.system.cpu.rename.int_rename_lookups / stats.system.cpu.rename.RenameLookups"'
    replace2 = str(
        values["system.cpu.rename.RenamedOperands"] * values["system.cpu.rename.int_rename_lookups"] / values[
            "system.cpu.rename.RenameLookups"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

    replace1 = '"stats.system.l2.overall_misses::total - stats.system.l2.ReadExReq_misses::total"'
    replace2 = str(values["system.l2.overall_misses::total"] - values["system.l2.ReadExReq_misses::total"])
    if replace1 in line:
        line = line.replace(replace1, replace2)
        content[i] = line

with open(file_xml, 'w') as f:
    f.writelines(content)

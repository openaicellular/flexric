#ifndef CPU_AMARISOFT_MIR_H
#define CPU_AMARISOFT_MIR_H 

typedef struct{

  // cpu load % in one core
  int global;

} cpu_amr_t;

void free_cpu_amr(cpu_amr_t* src);

cpu_amr_t parse_cpu_amr(void* it);

#endif

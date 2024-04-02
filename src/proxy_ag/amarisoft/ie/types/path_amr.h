#ifndef PATH_AMR_MIR_H
#define PATH_AMR_MIR_H 


typedef struct{

  // Mandatory
  char* type;

  // Mandatory. Path delay in ns
  float delay;

  // Mandatory. Path gain in dB
  float gain;

  // Mandatory. Frequency shift in Hz
  float freq_shift;

  // Mandatory. Doppler frequency in Hz
  float freq_doppler;

} path_amr_t;

void free_path_amr(path_amr_t* src);

path_amr_t parse_path_amr(void* it);

#endif

#ifndef CFG_LOADING_H
#define CFG_LOADING_H
#define CFG_DIR "/config.cfg"
#define NO_SUCH_SETTING "NO_SUCH_SETTING"
#define NO_SETTINGS_LOADED "NO_SETTINGS_LOADED"

typedef struct cfg_field{
        char* mem;
        char* field[2];

}cfg_field;



extern cfg_field** cfgs;


char* find_value_in_cfg_arr(char* setting,cfg_field** cfgs);
void loadCfg(void);
void freeCFGs(cfg_field*** cfgs);
#endif

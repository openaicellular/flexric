#ifndef REPORT_TYPE_CCC_H
#define REPORT_TYPE_CCC_H

// 9.3.9  Report Type

typedef enum {
    ALL_REPORT_TYPE_CCC,     //  the RAN Configuration Structure shall be reported regardless there has been a change or not in its value
    CHANGE_REPORT_TYPE_CCC,  //  the RAN Configuration Structure shall be reported only if its value has been changed (e.g. modified, added, deleted)

    END_REPORT_TYPE_CCC

} report_type_ccc_e;


#endif

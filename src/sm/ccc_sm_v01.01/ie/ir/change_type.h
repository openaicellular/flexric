#ifndef CHANGE_TYPE_CCC_H
#define CHANGE_TYPE_CCC_H

typedef enum {
    NONE_CHANGE_TYPE,          // no change occurred in reported RAN configuration structure attributes – e.g., due to periodic reporting or initial subscription. Values of attributes shall be reported through “Values of Attributes”.
    MODIFICATION_CHANGE_TYPE,  // a modification of value occurred in at least one attribute of the RAN configuration structure. New values shall be reported through “Values of Attributes” and old attribute values shall be reported through “Old Values of Attributes”.
    ADDITION_CHANGE_TYPE,      // Notification of addition of a new RAN configuration structure. Values of its attributes shall be reported through “Values of Attributes”.
    DELETION_CHANGE_TYPE,      // Notification of deletion of a RAN configuration structure. Deleted attribute values shall be reported through “Values of Attributes”.

    END_CHANGE_TYPE

} change_type_e;



#endif

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "interfaces/interfaces.h"

/*
 * Validation function type
 * Function pointer type for interface validators
 */
typedef bool (*ValidateFunc)(const void *interface, const char *name);

/*
 * Interface validator structure
 * Used to validate interface implementation
 */
typedef struct InterfaceValidator {
    bool capability_flag;         // Is this interface enabled
    const void *interface;        // Interface pointer
    ValidateFunc validate;        // Validation function
} InterfaceValidator;

/*
 * Interface validator macro
 * Creates validator structure for interface checking
 */
#define VALIDATOR(cap_field, iface_field, val_func) \
    { \
        .capability_flag = interfaces->capabilities.cap_field, \
        .interface = interfaces->iface_field, \
        .validate = val_func \
    }

/*
 * Validation functions
 * Interface validation
 */
bool validate_object_interfaces(const ObjectInterfaces *interfaces);

#endif

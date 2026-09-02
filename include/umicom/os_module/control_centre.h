/*-----------------------------------------------------------------------------
 * Umicom OS Module
 * File: include/umicom/os_module/control_centre.h
 *
 * PURPOSE:
 *   Define the thin user-space Umicom OS Control Centre contribution and make
 *   the kernel, boot, recovery and Framework boundaries inspectable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OS_MODULE_CONTROL_CENTRE_H
#define UMICOM_OS_MODULE_CONTROL_CENTRE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OS_CONTROL_TEXT_CAPACITY 192U
#define UMI_OS_CONTROL_MAX_BOUNDARIES 16U

/**
 * List the named os layer values accepted by this public contract.
 */
typedef enum UmiOsLayer {
    UMI_OS_LAYER_FIRMWARE = 1,
    UMI_OS_LAYER_BOOT = 2,
    UMI_OS_LAYER_KERNEL = 3,
    UMI_OS_LAYER_RECOVERY = 4,
    UMI_OS_LAYER_SYSTEM_SERVICES = 5,
    UMI_OS_LAYER_FRAMEWORK = 6,
    UMI_OS_LAYER_DESKTOP = 7,
    UMI_OS_LAYER_APPLICATION = 8
} UmiOsLayer;

/**
 * List the named os ownership values accepted by this public contract.
 */
typedef enum UmiOsOwnership {
    UMI_OS_OWNERSHIP_FULL_OS_REPOSITORY = 1,
    UMI_OS_OWNERSHIP_UPSTREAM_PROJECT = 2,
    UMI_OS_OWNERSHIP_FRAMEWORK = 3,
    UMI_OS_OWNERSHIP_OS_MODULE = 4,
    UMI_OS_OWNERSHIP_APPLICATION_MODULE = 5
} UmiOsOwnership;

/**
 * Represent the os boundary data shared with callers of this public contract.
 */
typedef struct UmiOsBoundary {
    UmiOsLayer layer;
    UmiOsOwnership ownership;
    char component[UMI_OS_CONTROL_TEXT_CAPACITY];
    char repository[UMI_OS_CONTROL_TEXT_CAPACITY];
    char rationale[UMI_OS_CONTROL_TEXT_CAPACITY];
    bool required_for_normal_user_space;
    bool required_for_recovery;
} UmiOsBoundary;

/**
 * Represent the os control centre snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiOsControlCentreSnapshot {
    size_t boundary_count;
    size_t framework_owned_count;
    size_t os_repository_owned_count;
    bool kernel_inside_framework;
    bool recovery_depends_on_framework;
    bool normal_user_space_uses_framework;
    uint64_t revision;
} UmiOsControlCentreSnapshot;

/**
 * Represent the os control centre data shared with callers of this public contract.
 */
typedef struct UmiOsControlCentre UmiOsControlCentre;

/**
 * Initialise os control centre from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_os_control_centre_create(
    UmiOsControlCentre **out_centre);
/**
 * Release or reset state held by os control centre so the same storage can be reused
 * safely.
 */
void umi_os_control_centre_destroy(UmiOsControlCentre *centre);

/**
 * Find os control centre boundary while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_os_control_centre_boundary_at(
    const UmiOsControlCentre *centre,
    size_t index,
    UmiOsBoundary *out_boundary);
/**
 * Provide the os control centre snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_os_control_centre_snapshot(
    const UmiOsControlCentre *centre,
    UmiOsControlCentreSnapshot *out_snapshot);

/**
 * Provide the os layer text operation used by this module and its client applications.
 */
const char *umi_os_layer_text(UmiOsLayer layer);
/**
 * Provide the os ownership text operation used by this module and its client applications.
 */
const char *umi_os_ownership_text(UmiOsOwnership ownership);

#ifdef __cplusplus
}
#endif

#endif

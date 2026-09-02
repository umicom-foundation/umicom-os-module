/*-----------------------------------------------------------------------------
 * Umicom OS Module
 * File: src/control_centre.c
 *
 * PURPOSE:
 *   Implement the user-space OS Control Centre boundary catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/os_module/control_centre.h"

#include <stdlib.h>
#include <string.h>

struct UmiOsControlCentre {
    UmiOsBoundary boundaries[UMI_OS_CONTROL_MAX_BOUNDARIES];
    size_t count;
    uint64_t revision;
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U ||
        source == NULL || source[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the add boundary operation used by this module and its client applications. */
static UmiStatus add_boundary(
    UmiOsControlCentre *centre,
    UmiOsLayer layer,
    UmiOsOwnership ownership,
    const char *component,
    const char *repository,
    const char *rationale,
    bool required_for_normal_user_space,
    bool required_for_recovery)
{
    UmiOsBoundary *boundary;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL ||
        centre->count >= UMI_OS_CONTROL_MAX_BOUNDARIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    boundary = &centre->boundaries[centre->count];
    (void)memset(boundary, 0, sizeof(*boundary));
    boundary->layer = layer;
    boundary->ownership = ownership;
    status = copy_text(boundary->component,
                       sizeof(boundary->component), component);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(boundary->repository,
                           sizeof(boundary->repository), repository);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(boundary->rationale,
                           sizeof(boundary->rationale), rationale);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    boundary->required_for_normal_user_space =
        required_for_normal_user_space;
    boundary->required_for_recovery = required_for_recovery;
    centre->count += 1U;
    centre->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Initialise os control centre from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_os_control_centre_create(
    UmiOsControlCentre **out_centre)
{
    UmiOsControlCentre *centre;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiOsControlCentre *)calloc(1U, sizeof(*centre));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->revision = 1U;

    status = add_boundary(
        centre, UMI_OS_LAYER_FIRMWARE,
        UMI_OS_OWNERSHIP_UPSTREAM_PROJECT,
        "UEFI, OpenSBI and platform firmware",
        "upstream firmware projects and umicom-os integration",
        "Firmware must operate before Framework user space exists.",
        false, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_boundary(
            centre, UMI_OS_LAYER_BOOT,
            UMI_OS_OWNERSHIP_FULL_OS_REPOSITORY,
            "Bootloader configuration and initramfs",
            "umicom-foundation/umicom-os",
            "Boot and early recovery are distribution responsibilities.",
            false, true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_boundary(
            centre, UMI_OS_LAYER_KERNEL,
            UMI_OS_OWNERSHIP_UPSTREAM_PROJECT,
            "Upstream Linux kernel, configuration and patches",
            "upstream Linux plus umicom-os kernel metadata",
            "Framework is portable user-space software, not a kernel fork.",
            false, true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_boundary(
            centre, UMI_OS_LAYER_RECOVERY,
            UMI_OS_OWNERSHIP_FULL_OS_REPOSITORY,
            "Minimal recovery and filesystem repair",
            "umicom-foundation/umicom-os",
            "Recovery must function when Framework is damaged.",
            false, true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_boundary(
            centre, UMI_OS_LAYER_SYSTEM_SERVICES,
            UMI_OS_OWNERSHIP_FULL_OS_REPOSITORY,
            "systemd, D-Bus, Wayland and package integration",
            "umicom-foundation/umicom-os",
            "The distribution selects and configures host user-space services.",
            true, false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_boundary(
            centre, UMI_OS_LAYER_FRAMEWORK,
            UMI_OS_OWNERSHIP_FRAMEWORK,
            "Portable process, device, settings and package contracts",
            "umicom-foundation/umicom-framework",
            "Framework owns reusable user-space contracts and adapters.",
            true, false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_boundary(
            centre, UMI_OS_LAYER_DESKTOP,
            UMI_OS_OWNERSHIP_OS_MODULE,
            "System Settings and Control Centre views",
            "umicom-foundation/umicom-os-module",
            "The thin module contributes product identity and system panels.",
            true, false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_boundary(
            centre, UMI_OS_LAYER_APPLICATION,
            UMI_OS_OWNERSHIP_APPLICATION_MODULE,
            "Studio, Trader and optional application packages",
            "independent Umicom application-module repositories",
            "OS image profiles package applications without owning their source.",
            false, false);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(centre);
        return status;
    }
    *out_centre = centre;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by os control centre so the same storage can be reused
 * safely.
 */
void umi_os_control_centre_destroy(UmiOsControlCentre *centre)
{
    free(centre);
}

/*
 * Find os control centre boundary while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_os_control_centre_boundary_at(
    const UmiOsControlCentre *centre,
    size_t index,
    UmiOsBoundary *out_boundary)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || out_boundary == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= centre->count) return UMI_STATUS_NOT_FOUND;
    *out_boundary = centre->boundaries[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the os control centre snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_os_control_centre_snapshot(
    const UmiOsControlCentre *centre,
    UmiOsControlCentreSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->boundary_count = centre->count;
    out_snapshot->normal_user_space_uses_framework = true;
    out_snapshot->kernel_inside_framework = false;
    out_snapshot->recovery_depends_on_framework = false;
    out_snapshot->revision = centre->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < centre->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (centre->boundaries[index].ownership ==
            UMI_OS_OWNERSHIP_FRAMEWORK) {
            out_snapshot->framework_owned_count += 1U;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (centre->boundaries[index].ownership ==
            UMI_OS_OWNERSHIP_FULL_OS_REPOSITORY) {
            out_snapshot->os_repository_owned_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

/* Provide the os layer text operation used by this module and its client applications. */
const char *umi_os_layer_text(UmiOsLayer layer)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (layer) {
    case UMI_OS_LAYER_FIRMWARE: return "firmware";
    case UMI_OS_LAYER_BOOT: return "boot";
    case UMI_OS_LAYER_KERNEL: return "kernel";
    case UMI_OS_LAYER_RECOVERY: return "recovery";
    case UMI_OS_LAYER_SYSTEM_SERVICES: return "system-services";
    case UMI_OS_LAYER_FRAMEWORK: return "framework";
    case UMI_OS_LAYER_DESKTOP: return "desktop";
    case UMI_OS_LAYER_APPLICATION: return "application";
    default: return "invalid";
    }
}

/* Provide the os ownership text operation used by this module and its client applications. */
const char *umi_os_ownership_text(UmiOsOwnership ownership)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (ownership) {
    case UMI_OS_OWNERSHIP_FULL_OS_REPOSITORY: return "umicom-os";
    case UMI_OS_OWNERSHIP_UPSTREAM_PROJECT: return "upstream";
    case UMI_OS_OWNERSHIP_FRAMEWORK: return "umicom-framework";
    case UMI_OS_OWNERSHIP_OS_MODULE: return "umicom-os-module";
    case UMI_OS_OWNERSHIP_APPLICATION_MODULE: return "application-module";
    default: return "invalid";
    }
}

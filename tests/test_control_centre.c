/*-----------------------------------------------------------------------------
 * Umicom OS Module
 * File: tests/test_control_centre.c
 *
 * PURPOSE:
 *   Prevent accidental movement of kernel, boot or recovery ownership into
 *   Umicom Framework or the thin OS module.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/os_module/control_centre.h"

#include <stdio.h>
#include <string.h>

#define REQUIRE(condition)                                                     \
    do {                                                                       \
        if (!(condition)) {                                                    \
            (void)fprintf(stderr, "[FAIL] %s:%d: %s\n",                       \
                          __FILE__, __LINE__, #condition);                      \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiOsControlCentre *centre = NULL;
    UmiOsControlCentreSnapshot snapshot;
    UmiCtOsArchitectureDecision architecture;
    UmiOsBoundary boundary;
    size_t index;
    bool saw_kernel = false;
    bool saw_research_kernel = false;
    bool saw_framework = false;

    REQUIRE(umi_os_control_centre_create(&centre) == UMI_STATUS_OK);
    /* The projection API must reject missing caller-owned storage before it
     * reads the Control Centre or writes a decision record. */
    REQUIRE(umi_os_control_centre_architecture_decision(NULL, &architecture) ==
            UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_os_control_centre_architecture_decision(centre, NULL) ==
            UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_os_control_centre_snapshot(centre, &snapshot) ==
            UMI_STATUS_OK);
    REQUIRE(snapshot.boundary_count >= 8U);
    REQUIRE(!snapshot.kernel_inside_framework);
    REQUIRE(!snapshot.recovery_depends_on_framework);
    REQUIRE(snapshot.normal_user_space_uses_framework);
    REQUIRE(umi_os_control_centre_architecture_decision(
                centre, &architecture) == UMI_STATUS_OK);
    REQUIRE(umi_ct_umicom_os_architecture_decision_validate(
                &architecture) == UMI_STATUS_OK);
    REQUIRE(!architecture.kernel_uses_framework);
    REQUIRE(!architecture.recovery_uses_framework);
    REQUIRE(architecture.normal_user_space_uses_framework);
    REQUIRE(architecture.freestanding_subset_allowed);
    REQUIRE(architecture.separate_kernel_repository);
    REQUIRE(!architecture.research_is_product_default);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.boundary_count; ++index) {
        REQUIRE(umi_os_control_centre_boundary_at(
                    centre, index, &boundary) == UMI_STATUS_OK);
        /* Apply this branch only when its contract condition is satisfied. */
        if (boundary.layer == UMI_OS_LAYER_KERNEL) {
            /* Production and research kernels share a layer but have different
             * owners, repositories and release responsibilities. */
            if (boundary.ownership == UMI_OS_OWNERSHIP_UPSTREAM_PROJECT) {
                saw_kernel = true;
                REQUIRE(strstr(boundary.repository, "Linux") != NULL ||
                        strstr(boundary.repository, "linux") != NULL);
                REQUIRE(boundary.required_for_recovery);
            } else if (boundary.ownership ==
                       UMI_OS_OWNERSHIP_KERNEL_RESEARCH) {
                saw_research_kernel = true;
                REQUIRE(strstr(boundary.repository, "umicom-kernel") != NULL);
                REQUIRE(!boundary.required_for_normal_user_space);
                REQUIRE(!boundary.required_for_recovery);
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (boundary.layer == UMI_OS_LAYER_FRAMEWORK) {
            saw_framework = true;
            REQUIRE(boundary.ownership == UMI_OS_OWNERSHIP_FRAMEWORK);
            REQUIRE(boundary.required_for_normal_user_space);
            REQUIRE(!boundary.required_for_recovery);
        }
    }
    REQUIRE(saw_kernel);
    REQUIRE(saw_research_kernel);
    REQUIRE(saw_framework);

    umi_os_control_centre_destroy(centre);
    return 0;
}

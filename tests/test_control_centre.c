/*-----------------------------------------------------------------------------
 * Umicom OS Module
 * File: tests/test_control_centre.c
 *
 * PURPOSE:
 *   Prevent accidental movement of kernel, boot or recovery ownership into
 *   Umicom Framework or the thin OS module.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

int main(void)
{
    UmiOsControlCentre *centre = NULL;
    UmiOsControlCentreSnapshot snapshot;
    UmiOsBoundary boundary;
    size_t index;
    bool saw_kernel = false;
    bool saw_framework = false;

    REQUIRE(umi_os_control_centre_create(&centre) == UMI_STATUS_OK);
    REQUIRE(umi_os_control_centre_snapshot(centre, &snapshot) ==
            UMI_STATUS_OK);
    REQUIRE(snapshot.boundary_count >= 8U);
    REQUIRE(!snapshot.kernel_inside_framework);
    REQUIRE(!snapshot.recovery_depends_on_framework);
    REQUIRE(snapshot.normal_user_space_uses_framework);

    for (index = 0U; index < snapshot.boundary_count; ++index) {
        REQUIRE(umi_os_control_centre_boundary_at(
                    centre, index, &boundary) == UMI_STATUS_OK);
        if (boundary.layer == UMI_OS_LAYER_KERNEL) {
            saw_kernel = true;
            REQUIRE(boundary.ownership ==
                    UMI_OS_OWNERSHIP_UPSTREAM_PROJECT);
            REQUIRE(strstr(boundary.repository, "Linux") != NULL ||
                    strstr(boundary.repository, "linux") != NULL);
        }
        if (boundary.layer == UMI_OS_LAYER_FRAMEWORK) {
            saw_framework = true;
            REQUIRE(boundary.ownership == UMI_OS_OWNERSHIP_FRAMEWORK);
            REQUIRE(boundary.required_for_normal_user_space);
            REQUIRE(!boundary.required_for_recovery);
        }
    }
    REQUIRE(saw_kernel);
    REQUIRE(saw_framework);

    umi_os_control_centre_destroy(centre);
    return 0;
}

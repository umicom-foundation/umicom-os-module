/*-----------------------------------------------------------------------------
 * Umicom OS Module
 * File: src/console/main.c
 *
 * PURPOSE:
 *   Print the inspectable Umicom OS ownership boundary from user space.
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

int main(void)
{
    UmiOsControlCentre *centre = NULL;
    UmiOsControlCentreSnapshot snapshot;
    UmiStatus status = umi_os_control_centre_create(&centre);
    size_t index;

    if (status == UMI_STATUS_OK) {
        status = umi_os_control_centre_snapshot(centre, &snapshot);
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Umicom OS Control Centre failed: %s\n",
                      umi_status_text(status));
        umi_os_control_centre_destroy(centre);
        return 1;
    }

    (void)printf("Umicom OS user-space boundary\n");
    (void)printf("Kernel inside Framework: %s\n",
                 snapshot.kernel_inside_framework ? "yes" : "no");
    (void)printf("Recovery depends on Framework: %s\n",
                 snapshot.recovery_depends_on_framework ? "yes" : "no");
    (void)printf("Normal user space uses Framework: %s\n\n",
                 snapshot.normal_user_space_uses_framework ? "yes" : "no");

    for (index = 0U; index < snapshot.boundary_count; ++index) {
        UmiOsBoundary boundary;
        status = umi_os_control_centre_boundary_at(
            centre, index, &boundary);
        if (status != UMI_STATUS_OK) break;
        (void)printf("%-16s %-20s %s\n",
                     umi_os_layer_text(boundary.layer),
                     umi_os_ownership_text(boundary.ownership),
                     boundary.component);
    }

    umi_os_control_centre_destroy(centre);
    return status == UMI_STATUS_OK ? 0 : 1;
}

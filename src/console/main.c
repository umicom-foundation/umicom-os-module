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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiOsControlCentre *centre = NULL;
    UmiOsControlCentreSnapshot snapshot;
    UmiCtOsArchitectureDecision architecture;
    UmiStatus status = umi_os_control_centre_create(&centre);
    size_t index;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_os_control_centre_snapshot(centre, &snapshot);
    }
    /* Read the same Framework-owned decision that future graphical panels will
     * render, rather than maintaining a second list in the console frontend. */
    if (status == UMI_STATUS_OK) {
        status = umi_os_control_centre_architecture_decision(
            centre, &architecture);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    (void)printf("Production foundation: %s\n",
                 umi_ct_os_foundation_text(
                     architecture.production_foundation));
    (void)printf("Portability target: %s\n",
                 umi_ct_os_foundation_text(
                     architecture.portability_foundation));
    (void)printf("Independent research: %s\n\n",
                 umi_ct_os_foundation_text(
                     architecture.research_foundation));

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.boundary_count; ++index) {
        UmiOsBoundary boundary;
        status = umi_os_control_centre_boundary_at(
            centre, index, &boundary);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        (void)printf("%-16s %-20s %s\n",
                     umi_os_layer_text(boundary.layer),
                     umi_os_ownership_text(boundary.ownership),
                     boundary.component);
    }

    umi_os_control_centre_destroy(centre);
    return status == UMI_STATUS_OK ? 0 : 1;
}

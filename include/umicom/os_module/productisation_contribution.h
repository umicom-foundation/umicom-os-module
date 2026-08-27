/*-----------------------------------------------------------------------------
 * Umicom OS Control Centre Module
 * File: include/umicom/os_module/productisation_contribution.h
 *
 * PURPOSE:
 *   Declare this thin module's adoption of Framework-owned application
 *   experience, components, layouts and productisation evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OS_MODULE_PRODUCTISATION_CONTRIBUTION_H
#define UMICOM_OS_MODULE_PRODUCTISATION_CONTRIBUTION_H

#include "umicom/application/productisation/adoption.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiProductApplicationAdoption *
umi_os_module_productisation_contribution(void);
UmiStatus umi_os_module_productisation_snapshot(
    UmiProductApplicationAdoptionSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif

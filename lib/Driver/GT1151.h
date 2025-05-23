#include "DEV_Config.h"

#ifndef __GT1151_H
#define __GT1151_H

// EXTERN
#ifdef __cplusplus
extern "C"
{
#endif

#define CT_MAX_TOUCH 5

    typedef struct
    {
        UBYTE Touch;
        UBYTE TouchpointFlag;
        UBYTE TouchCount;

        UBYTE Touchkeytrackid[CT_MAX_TOUCH];
        UWORD X[CT_MAX_TOUCH];
        UWORD Y[CT_MAX_TOUCH];
        UWORD S[CT_MAX_TOUCH];
    } GT1151_Dev;

    // Kira
    GT1151_Dev *GT_GetDev_Now();
    // Kira
    GT1151_Dev *GT_GetDev_Old();

    void GT_Reset(void);
    void GT_Write(UWORD Reg, char *Data, UBYTE len);
    void GT_Read(UWORD Reg, char *Data, UBYTE len);
    void GT_ReadVersion(void);
    UBYTE GT_Scan(void);
    void GT_Init(void);
    void GT_Gesture(void);

// EXTERN END
#ifdef __cplusplus
}
#endif
#endif

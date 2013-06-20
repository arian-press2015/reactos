
#include "DibLib.h"

/*
 *  M D S P    Generic function
 * -----------------------
 *  0 0 0 0    -> BLACKNESS, WHITENESS
 *  0 0 0 1    -> PATCOPY, NOTPATCOPY
 *  0 0 1 0    -> SRCCOPY, NOTSRCCOPY
 *  0 0 1 1    SrcPatBlt
 *  0 1 0 0    -> NOOP, DESTINVERT
 *  0 1 0 1    PatPaint
 *  0 1 1 0    SrcPaint
 *  0 1 1 1    BitBlt
 *  1 0 0 0    MaskCopy, -> SRCCOPY / SRCINVERT using Mask als source
 *  1 0 0 1    MaskPatBlt
 *  1 0 1 0    MaskSrcBlt
 *  1 0 1 1    MaskSrcPatBlt
 *  1 1 0 0    MaskPaint
 *  1 1 0 1    MaskPatPaint
 *  1 1 1 0    MaskSrcPaint
 *  1 1 1 1    MaskBlt
 */


const BYTE ajShift4[2] = {4, 0};

enum
{
    INDEX_BitBlt_NOOP,
    INDEX_BitBlt_DSTINVERT,
    INDEX_BitBlt_BLACKNESS,
    INDEX_BitBlt_WHITENESS,
    INDEX_BitBlt_PATCOPY,
    INDEX_BitBlt_NOTPATCOPY,
    INDEX_BitBlt_SRCCOPY,
    INDEX_BitBlt_NOTSRCCOPY,
    INDEX_BitBlt_SRCERASE,
    INDEX_BitBlt_NOTSRCERASE,
    INDEX_BitBlt_PATINVERT,
    INDEX_BitBlt_SRCINVERT,
    INDEX_BitBlt_SRCAND,
    INDEX_BitBlt_MERGEPAINT,
    INDEX_BitBlt_MERGECOPY,
    INDEX_BitBlt_SRCPAINT,
    INDEX_BitBlt_PATPAINT,
    INDEX_SrcPatBlt,
    INDEX_PatPaint,
    INDEX_SrcPaint,
    INDEX_BitBlt,
};

const PFN_DIBFUNCTION
gapfnDibFunction[] =
{
    Dib_BitBlt_NOOP,
    Dib_BitBlt_DSTINVERT,
    Dib_BitBlt_BLACKNESS,
    Dib_BitBlt_WHITENESS,
    Dib_BitBlt_PATCOPY,
    Dib_BitBlt_NOTPATCOPY,
    Dib_BitBlt_SRCCOPY,
    Dib_BitBlt_NOTSRCCOPY,
    Dib_BitBlt_SRCERASE,
    Dib_BitBlt_NOTSRCERASE,
    Dib_BitBlt_PATINVERT,
    Dib_BitBlt_SRCINVERT,
    Dib_BitBlt_SRCAND,
    Dib_BitBlt_MERGEPAINT,
    Dib_BitBlt_MERGECOPY,
    Dib_BitBlt_SRCPAINT,
    Dib_BitBlt_PATPAINT,
    Dib_SrcPatBlt,
    Dib_PatPaint,
    Dib_SrcPaint,
    Dib_BitBlt,
};

const UCHAR
gajIndexPerRop[256] =
{
    INDEX_BitBlt_BLACKNESS, // BLACKNESS
    INDEX_BitBlt, // DPSoon,
    INDEX_BitBlt, // DPSona,
    INDEX_SrcPatBlt, // PSon,
    INDEX_BitBlt, // SDPona,
    INDEX_PatPaint, // DPon,
    INDEX_BitBlt, // PDSxnon,
    INDEX_BitBlt, // PDSaon,
    INDEX_BitBlt, // SDPnaa,
    INDEX_BitBlt, // PDSxon,
    INDEX_PatPaint, // DPna,
    INDEX_BitBlt, // PSDnaon,
    INDEX_SrcPatBlt, // SPna,
    INDEX_BitBlt, // PDSnaon,
    INDEX_BitBlt, // PDSonon,
    INDEX_BitBlt_NOTPATCOPY, // Pn,
    INDEX_BitBlt, // PDSona,
    INDEX_BitBlt_NOTSRCERASE, // DSon / NOTSRCERASE
    INDEX_BitBlt, // SDPxnon,
    INDEX_BitBlt, // SDPaon,
    INDEX_BitBlt, // DPSxnon,
    INDEX_BitBlt, // DPSaon,
    INDEX_BitBlt, // PSDPSanaxx,
    INDEX_BitBlt, // SSPxDSxaxn,
    INDEX_BitBlt, // SPxPDxa,
    INDEX_BitBlt, // SDPSanaxn,
    INDEX_BitBlt, // PDSPaox,
    INDEX_BitBlt, // SDPSxaxn,
    INDEX_BitBlt, // PSDPaox,
    INDEX_BitBlt, // DSPDxaxn,
    INDEX_BitBlt, // PDSox,
    INDEX_BitBlt, // PDSoan,
    INDEX_BitBlt, // DPSnaa,
    INDEX_BitBlt, // SDPxon,
    INDEX_SrcPaint, // DSna,
    INDEX_BitBlt, // SPDnaon,
    INDEX_BitBlt, // SPxDSxa,
    INDEX_BitBlt, // PDSPanaxn,
    INDEX_BitBlt, // SDPSaox,
    INDEX_BitBlt, // SDPSxnox,
    INDEX_BitBlt, // DPSxa,
    INDEX_BitBlt, // PSDPSaoxxn,
    INDEX_BitBlt, // DPSana,
    INDEX_BitBlt, // SSPxPDxaxn,
    INDEX_BitBlt, // SPDSoax,
    INDEX_BitBlt, // PSDnox,
    INDEX_BitBlt, // PSDPxox,
    INDEX_BitBlt, // PSDnoan,
    INDEX_SrcPatBlt, // PSna,
    INDEX_BitBlt, // SDPnaon,
    INDEX_BitBlt, // SDPSoox,
    INDEX_BitBlt_NOTSRCCOPY, // Sn / NOTSRCCOPY
    INDEX_BitBlt, // SPDSaox,
    INDEX_BitBlt, // SPDSxnox,
    INDEX_BitBlt, // SDPox,
    INDEX_BitBlt, // SDPoan,
    INDEX_BitBlt, // PSDPoax,
    INDEX_BitBlt, // SPDnox,
    INDEX_BitBlt, // SPDSxox,
    INDEX_BitBlt, // SPDnoan,
    INDEX_SrcPatBlt, // PSx,
    INDEX_BitBlt, // SPDSonox,
    INDEX_BitBlt, // SPDSnaox,
    INDEX_SrcPatBlt, // PSan,
    INDEX_BitBlt, // PSDnaa,
    INDEX_BitBlt, // DPSxon,
    INDEX_BitBlt, // SDxPDxa,
    INDEX_BitBlt, // SPDSanaxn,
    INDEX_BitBlt_SRCERASE, // SDna / SRCERASE
    INDEX_BitBlt, // DPSnaon,
    INDEX_BitBlt, // DSPDaox,
    INDEX_BitBlt, // PSDPxaxn,
    INDEX_BitBlt, // SDPxa,
    INDEX_BitBlt, // PDSPDaoxxn,
    INDEX_BitBlt, // DPSDoax,
    INDEX_BitBlt, // PDSnox,
    INDEX_BitBlt, // SDPana,
    INDEX_BitBlt, // SSPxDSxoxn,
    INDEX_BitBlt, // PDSPxox,
    INDEX_BitBlt, // PDSnoan,
    INDEX_PatPaint, // PDna,
    INDEX_BitBlt, // DSPnaon,
    INDEX_BitBlt, // DPSDaox,
    INDEX_BitBlt, // SPDSxaxn,
    INDEX_BitBlt, // DPSonon,
    INDEX_BitBlt_DSTINVERT, // Dn / DSTINVERT
    INDEX_BitBlt, // DPSox,
    INDEX_BitBlt, // DPSoan,
    INDEX_BitBlt, // PDSPoax,
    INDEX_BitBlt, // DPSnox,
    INDEX_BitBlt_PATINVERT, // DPx / PATINVERT
    INDEX_BitBlt, // DPSDonox,
    INDEX_BitBlt, // DPSDxox,
    INDEX_BitBlt, // DPSnoan,
    INDEX_BitBlt, // DPSDnaox,
    INDEX_PatPaint, // DPan,
    INDEX_BitBlt, // PDSxa,
    INDEX_BitBlt, // DSPDSaoxxn,
    INDEX_BitBlt, // DSPDoax,
    INDEX_BitBlt, // SDPnox,
    INDEX_BitBlt, // SDPSoax,
    INDEX_BitBlt, // DSPnox,
    INDEX_BitBlt_SRCINVERT, // DSx / SRCINVERT
    INDEX_BitBlt, // SDPSonox,
    INDEX_BitBlt, // DSPDSonoxxn,
    INDEX_BitBlt, // PDSxxn,
    INDEX_BitBlt, // DPSax,
    INDEX_BitBlt, // PSDPSoaxxn,
    INDEX_BitBlt, // SDPax,
    INDEX_BitBlt, // PDSPDoaxxn,
    INDEX_BitBlt, // SDPSnoax,
    INDEX_BitBlt, // PDSxnan,
    INDEX_BitBlt, // PDSana,
    INDEX_BitBlt, // SSDxPDxaxn,
    INDEX_BitBlt, // SDPSxox,
    INDEX_BitBlt, // SDPnoan,
    INDEX_BitBlt, // DSPDxox,
    INDEX_BitBlt, // DSPnoan,
    INDEX_BitBlt, // SDPSnaox,
    INDEX_SrcPaint, // DSan,
    INDEX_BitBlt, // PDSax,
    INDEX_BitBlt, // DSPDSoaxxn,
    INDEX_BitBlt, // DPSDnoax,
    INDEX_BitBlt, // SDPxnan,
    INDEX_BitBlt, // SPDSnoax,
    INDEX_BitBlt, // DPSxnan,
    INDEX_BitBlt, // SPxDSxo,
    INDEX_BitBlt, // DPSaan,
    INDEX_BitBlt, // DPSaa,
    INDEX_BitBlt, // SPxDSxon,
    INDEX_BitBlt, // DPSxna,
    INDEX_BitBlt, // SPDSnoaxn,
    INDEX_BitBlt, // SDPxna,
    INDEX_BitBlt, // PDSPnoaxn,
    INDEX_BitBlt, // DSPDSoaxx,
    INDEX_BitBlt, // PDSaxn,
    INDEX_BitBlt_SRCAND, // DSa / SRCAND
    INDEX_BitBlt, // SDPSnaoxn,
    INDEX_BitBlt, // DSPnoa,
    INDEX_BitBlt, // DSPDxoxn,
    INDEX_BitBlt, // SDPnoa,
    INDEX_BitBlt, // SDPSxoxn,
    INDEX_BitBlt, // SSDxPDxax,
    INDEX_BitBlt, // PDSanan,
    INDEX_BitBlt, // PDSxna,
    INDEX_BitBlt, // SDPSnoaxn,
    INDEX_BitBlt, // DPSDPoaxx,
    INDEX_BitBlt, // SPDaxn,
    INDEX_BitBlt, // PSDPSoaxx,
    INDEX_BitBlt, // DPSaxn,
    INDEX_BitBlt, // DPSxx,
    INDEX_BitBlt, // PSDPSonoxx,
    INDEX_BitBlt, // SDPSonoxn,
    INDEX_SrcPaint, // DSxn,
    INDEX_BitBlt, // DPSnax,
    INDEX_BitBlt, // SDPSoaxn,
    INDEX_BitBlt, // SPDnax,
    INDEX_BitBlt, // DSPDoaxn,
    INDEX_BitBlt, // DSPDSaoxx,
    INDEX_BitBlt, // PDSxan,
    INDEX_PatPaint, // DPa,
    INDEX_BitBlt, // PDSPnaoxn,
    INDEX_BitBlt, // DPSnoa,
    INDEX_BitBlt, // DPSDxoxn,
    INDEX_BitBlt, // PDSPonoxn,
    INDEX_PatPaint, // PDxn,
    INDEX_BitBlt, // DSPnax,
    INDEX_BitBlt, // PDSPoaxn,
    INDEX_BitBlt, // DPSoa,
    INDEX_BitBlt, // DPSoxn,
    INDEX_BitBlt_NOOP, // D,
    INDEX_BitBlt, // DPSono,
    INDEX_BitBlt, // SPDSxax,
    INDEX_BitBlt, // DPSDaoxn,
    INDEX_BitBlt, // DSPnao,
    INDEX_PatPaint, // DPno,
    INDEX_BitBlt, // PDSnoa,
    INDEX_BitBlt, // PDSPxoxn,
    INDEX_BitBlt, // SSPxDSxox,
    INDEX_BitBlt, // SDPanan,
    INDEX_BitBlt, // PSDnax,
    INDEX_BitBlt, // DPSDoaxn,
    INDEX_BitBlt, // DPSDPaoxx,
    INDEX_BitBlt, // SDPxan,
    INDEX_BitBlt, // PSDPxax,
    INDEX_BitBlt, // DSPDaoxn,
    INDEX_BitBlt, // DPSnao,
    INDEX_BitBlt_MERGEPAINT, // DSno / MERGEPAINT
    INDEX_BitBlt, // SPDSanax,
    INDEX_BitBlt, // SDxPDxan,
    INDEX_BitBlt, // DPSxo,
    INDEX_BitBlt, // DPSano,
    INDEX_BitBlt_MERGECOPY, // PSa / MERGECOPY
    INDEX_BitBlt, // SPDSnaoxn,
    INDEX_BitBlt, // SPDSonoxn,
    INDEX_SrcPatBlt, // PSxn,
    INDEX_BitBlt, // SPDnoa,
    INDEX_BitBlt, // SPDSxoxn,
    INDEX_BitBlt, // SDPnax,
    INDEX_BitBlt, // PSDPoaxn,
    INDEX_BitBlt, // SDPoa,
    INDEX_BitBlt, // SPDoxn,
    INDEX_BitBlt, // DPSDxax,
    INDEX_BitBlt, // SPDSaoxn,
    INDEX_BitBlt_SRCCOPY, // S / SRCCOPY
    INDEX_BitBlt, // SDPono,
    INDEX_BitBlt, // SDPnao,
    INDEX_SrcPatBlt, // SPno,
    INDEX_BitBlt, // PSDnoa,
    INDEX_BitBlt, // PSDPxoxn,
    INDEX_BitBlt, // PDSnax,
    INDEX_BitBlt, // SPDSoaxn,
    INDEX_BitBlt, // SSPxPDxax,
    INDEX_BitBlt, // DPSanan,
    INDEX_BitBlt, // PSDPSaoxx,
    INDEX_BitBlt, // DPSxan,
    INDEX_BitBlt, // PDSPxax,
    INDEX_BitBlt, // SDPSaoxn,
    INDEX_BitBlt, // DPSDanax,
    INDEX_BitBlt, // SPxDSxan,
    INDEX_BitBlt, // SPDnao,
    INDEX_SrcPaint, // SDno,
    INDEX_BitBlt, // SDPxo,
    INDEX_BitBlt, // SDPano,
    INDEX_BitBlt, // PDSoa,
    INDEX_BitBlt, // PDSoxn,
    INDEX_BitBlt, // DSPDxax,
    INDEX_BitBlt, // PSDPaoxn,
    INDEX_BitBlt, // SDPSxax,
    INDEX_BitBlt, // PDSPaoxn,
    INDEX_BitBlt, // SDPSanax,
    INDEX_BitBlt, // SPxPDxan,
    INDEX_BitBlt, // SSPxDSxax,
    INDEX_BitBlt, // DSPDSanaxxn,
    INDEX_BitBlt, // DPSao,
    INDEX_BitBlt, // DPSxno,
    INDEX_BitBlt, // SDPao,
    INDEX_BitBlt, // SDPxno,
    INDEX_BitBlt_SRCPAINT, // DSo / SRCPAINT
    INDEX_BitBlt, // SDPnoo,
    INDEX_BitBlt_PATCOPY, // P / PATCOPY
    INDEX_BitBlt, // PDSono,
    INDEX_BitBlt, // PDSnao,
    INDEX_SrcPatBlt, // PSno,
    INDEX_BitBlt, // PSDnao,
    INDEX_PatPaint, // PDno,
    INDEX_BitBlt, // PDSxo,
    INDEX_BitBlt, // PDSano,
    INDEX_BitBlt, // PDSao,
    INDEX_BitBlt, // PDSxno,
    INDEX_PatPaint, // DPo,
    INDEX_BitBlt_PATPAINT, // DPSnoo, // PATPAINT
    INDEX_SrcPatBlt, // PSo,
    INDEX_BitBlt, // PSDnoo,
    INDEX_BitBlt, // DPSoo,
    INDEX_BitBlt_WHITENESS //WHITENESS
};

const PFN_DIBFUNCTION
gapfnMaskFunction[8] =
{
    Dib_MaskCopy,
    Dib_MaskPatBlt,
    Dib_MaskSrcBlt,
    Dib_MaskSrcPatBlt,
    Dib_MaskPaint,
    Dib_MaskPatPaint,
    Dib_MaskSrcPaint,
    Dib_MaskBlt
};

/** 
  * Copyright (C) 2008-2011, Marvell International Ltd. 
  * 
  * This software file (the "File") is distributed by Marvell International 
  * Ltd. under the terms of the GNU General Public License Version 2, June 1991 
  * (the "License").  You may use, redistribute and/or modify this File in 
  * accordance with the terms and conditions of the License, a copy of which 
  * is available by writing to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
  * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE 
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE 
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about 
  * this warranty disclaimer.
  *
  */

/*******************************************************************************************
*
* File: linkmgt.h
*        Client Link Management Module Module Header
* Description:  Implementation of the Client Link Management Module Services
*
*******************************************************************************************/

#ifndef LINK_MGT
#define LINK_MGT

/* Definition of virtual mac entry for each instance */
#define SELECT_FOR_BSSID    (1)
#define SELECT_FOR_SSID     (1<<1)
#define LNK_MGT_POLL_TICK   50
#define LNK_MGT_RESTART_WAIT_TICK   10
#define LNK_MGT_SCAN_IE_BUF_LEN 256
#define LNK_MGT_BSS_PROFILE_BUF_LEN 256

typedef struct linkMgtEntry_t
{
	UINT8				    active;  	
	phyMacId_t			    phyHwMacIndx;
    Timer 				    linkTimer;
    UINT8                   searchBitMap;
    UINT8	                bssid[IEEEtypes_ADDRESS_SIZE];
    IEEEtypes_SsIdElement_t ssidIE;
    UINT8                   scanIeBufLen;
    UINT8                   scanIeBuf[LNK_MGT_SCAN_IE_BUF_LEN];
	vmacEntry_t             *vMac_p;
}linkMgtEntry_t;

extern void linkMgtInit(UINT8 phyIndex);
extern SINT32 linkMgtParseScanResult(UINT8 phyIndex);
extern SINT32 linkMgtStop(UINT8 phyIndex);
extern SINT32 linkMgtStart(UINT8 phyIndex, 
                           UINT8 *prefBSSID_p, 
                           UINT8 *ieBuf_p, 
                           UINT8 ieBufLen);
extern SINT32 linkMgtReStart(UINT8 phyIndex, vmacEntry_t  *vmacEntry_p);
extern IEEEtypes_RSN_IE_t *linkMgtParseWpaIe(UINT8 *ieBuf_p, UINT16 ieBufLen);
extern IEEEtypes_RSN_IE_t *linkMgtParseWpsIe(UINT8 *ieBuf_p, UINT16 ieBufLen);
extern IEEEtypes_Generic_HT_Element_t *linkMgtParseHTGenIe(UINT8 *ieBuf_p, UINT16 ieBufLen);
extern WSC_HeaderIE_t *linkMgtParseWpsInfo(UINT16 wpsInfoID, UINT8 *ieBuf_p, UINT16 ieBufLen);
#endif /* LINK_MGT */






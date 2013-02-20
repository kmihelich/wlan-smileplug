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
#ifndef WL_KERNEL_26
#include <linux/config.h>
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/random.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,44))
#include <linux/tqueue.h>
#else
#include <linux/workqueue.h>
#endif
#include <linux/kmod.h>
#include <asm/memory.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
#include <asm/arch/irqs.h>
#endif

#include "ewb_packet.h"
#include "ewb_hash.h"
#include "wltypes.h"
#include "ewb_hash.h"

#include "linux/udp.h"
#include <linux/skbuff.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#define SKB_IPHDR(skb) ((struct iphdr*)skb->network_header)
#define SKB_NHDR(skb) skb->network_header
#define SKB_MACHDR(skb) skb->mac_header
#else
#define SKB_IPHDR(skb) skb->nh.iph
#define SKB_NHDR(skb) skb->nh.raw
#define SKB_MACHDR(skb) skb->mac.raw
#endif

/* Global Variables */
/* Import Variables */

void printMAC(unsigned char* mac)
{
    printk("%x %x %x %x %x %x ", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);	
}


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
int ewbIpToDs(unsigned char *packet, unsigned char *rfAddr)
{
	eth_hdr_t *eth;
	ewb_ip_hdr  *ip;
	UINT32 srcAddr;

	if (packet == NULL)
	{
		return (-1);
	}
	eth = (eth_hdr_t *)packet;
	ip = (ewb_ip_hdr *)(packet + ETH_HDR_LEN);
	
	memcpy(&srcAddr, ip->srcAddr, 4);
    wetUpdateHashEntry(srcAddr, eth->src);
    
    /* The original IP may have roamed! */
	memcpy(eth->src, rfAddr, HW_ADDR_LEN);
	return 0;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
int ewbIpFromDs(unsigned char *packet)
{
	eth_hdr_t *eth;
	ewb_ip_hdr  *ip;
	unsigned char *clntMac;
	UINT32 destAddr, srcAddr;
	
	eth = (eth_hdr_t *)packet;
	ip = (ewb_ip_hdr *)(packet + ETH_HDR_LEN);
	
	memcpy(&srcAddr, ip->srcAddr, 4);
   	wetClearHashEntry(srcAddr); /* The original IP may have roamed! */

	memcpy(&destAddr, ip->destAddr, 4);    
	if ((clntMac = wetGetHashEntryValue(destAddr)) == NULL)
    {
        return (-1);
    }
	memcpy(eth->dest, clntMac, HW_ADDR_LEN);
	return 0;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
int ewbArpToDs(unsigned char *packet, unsigned char *rfAddr)
{
	eth_hdr_t *eth;
	arp_eth_ip_addr *arpAddr;
	UINT32 sndIpAddr;	

	if(packet == NULL)
	{
		return -1;
	}
	eth = (eth_hdr_t *)packet;
	arpAddr = (arp_eth_ip_addr *)(packet + ETH_HDR_LEN + ARP_HDR_LEN);
	memcpy(&sndIpAddr, arpAddr->sndIpAddr, 4);
    
    //printk("\nSend IP %x mac:: ",sndIpAddr);	
    //printMAC(eth->src);
    
    wetUpdateHashEntry(sndIpAddr, eth->src);
    
    /* The original IP may have roamed! */
	memcpy(eth->src, rfAddr, HW_ADDR_LEN);
	memcpy(arpAddr->sndHwAddr, rfAddr, HW_ADDR_LEN);
	return 0;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
int ewbArpFromDs(unsigned char *packet)
{
	eth_hdr_t *eth;
	arp_eth_ip_addr *arpAddr;
	unsigned char *clntMac;
	UINT32 sndIpAddr, trgtIpAddr;	
	
	eth = (eth_hdr_t *)packet;
	arpAddr = (arp_eth_ip_addr *)(packet + ETH_HDR_LEN + ARP_HDR_LEN);
	memcpy(&sndIpAddr, arpAddr->sndIpAddr, 4);
    
	wetClearHashEntry(sndIpAddr); /* The original IP may have roamed! */
    
	memcpy(&trgtIpAddr, arpAddr->trgtIpAddr, 4);
    
    //printk("ewbArpFromDs: %x %x ",sndIpAddr,trgtIpAddr);

    if ((clntMac = wetGetHashEntryValue(trgtIpAddr)) == NULL)
    {
        printk("\nCould not find entry ");
        return -1;
    }
	memcpy(eth->dest, clntMac, HW_ADDR_LEN);
	memcpy(arpAddr->trgtHwAddr, clntMac, HW_ADDR_LEN);
	return 0;
}

int ewbDhcpFromDs(unsigned char *packet, struct udphdr *udphp)
{
  eth_hdr_t *eth =  (eth_hdr_t *)packet;
  char *clntMac;

  clntMac = (char *)udphp + 28;     /* client MAC address */

  //L2: Modify dest mac in MAC header
  memcpy(eth->dest, clntMac, HW_ADDR_LEN);
 
  return 0;

}


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern int ewbWlanRecv(struct sk_buff *skb,unsigned char *rfAddr)
{
	struct eth_hdr_t *eth;
    unsigned short etherType;
    struct iphdr *iphp;
    struct udphdr * udphp;

	eth = (eth_hdr_t *)skb->data;

	if(IS_BROADCAST_HWADDR(eth->dest))
        return 0;

    /* MULTI_CAST_SUPPORT*/
	/* Check to if it's MultiCast */
	if(eth->dest[0] == 0x01)
        return 0;

    etherType = ntohs(eth->type);
    //printk("\nP WL R %x %x  ",etherType,skb->protocol);

	switch(etherType)
	{
	    case ETH_IP_TYPE:
            iphp = (struct iphdr *) (eth + 1);
            udphp = (struct udphdr *) (iphp+1);
            if( !(memcmp(eth->dest, rfAddr, HW_ADDR_LEN))
                && iphp->protocol == IPPROTO_UDP && 
                udphp->source == htons(67) && (udphp->dest == htons(67) || udphp->dest == htons(68))) 
            {
                if(ewbDhcpFromDs(skb->data, udphp) < 0) 
                    return -1;
            }
            else
            {
                if(ewbIpFromDs(skb->data) < 0)
    		        return -1;
            }
		    break;

	    case ETH_ARP_TYPE:
    		if(ewbArpFromDs(skb->data) < 0)
    		    return -1;    		
		    break;

	    default:
		    return 0;
	}  

    return 0;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern int ewbLanRecv(struct sk_buff *skb,unsigned char *rfAddr)
{
	eth_hdr_t *eth;
	unsigned short etherType;
        
	eth = (eth_hdr_t *)skb->data;
    etherType = ntohs(eth->type);
    
	switch (etherType)
	{
        case ETH_IP_TYPE:
    		if (ewbIpToDs(skb->data, rfAddr) < 0)
    		{
    			goto dropPacket;
    		}
		    break;

	    case ETH_ARP_TYPE:
    		if (ewbArpToDs(skb->data, rfAddr) < 0)
    		{
    			goto dropPacket;
    		}
    		break;

	    case EAPOL_TYPE:
            {
                goto sendToWLAN;
	        }
        default:
            /* MULTI_CAST_SUPPORT*/
            /* Check if MultiCast */
            if(eth->dest[0] == 0x01)
            {
                memcpy(eth->src, rfAddr, HW_ADDR_LEN);
                goto sendToWLAN;
            }
    
		    goto dropPacket;    
	}

sendToWLAN:	
    return 0;

dropPacket:
    //panic("Fatal Error! Fix me");
    return (-1);
}

    
/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern int ewbInit(void)
{   
    wetHashInit();
    return 0;
}

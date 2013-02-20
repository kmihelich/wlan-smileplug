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

/** include files **/
#ifdef AP8X_STATISTICS
#include "ap8xLnxIntf.h"
#include <stdarg.h>
#include "ap8xLnxFwcmd.h"


#define WL_MAX_FILE_NAME_LEN 32
struct dev_stat
{
	unsigned char filename[WL_MAX_FILE_NAME_LEN];
	struct net_device *netdev;
	struct proc_dir_entry *ap8x_proc;
};
#define WL_MAXIMUM_INSTANCES 20


static struct dev_stat devstat[WL_MAXIMUM_INSTANCES];
static int devstat_index= 0;
struct proc_dir_entry *ap8x=NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
extern struct proc_dir_entry *proc_net;
#else
struct proc_dir_entry *proc_net=NULL;
#endif
extern long atohex2(const char *number);
void clear_ap8x_stat( struct net_device *dev )
{
	struct net_device *netdev = dev;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct net_device_stats  *stat = &(wlpptr->netDevStats);
	memset(stat, 0,  sizeof(struct net_device_stats));
}

extern UINT32 WLDBG_LEVELS;


static int ap8x_proc_read (struct net_device *netdev, char *page, char **start, off_t off, int count, int *eof,
						   void *data)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct net_device_stats  *stat = &(wlpptr->netDevStats);
	char *p = page;
	int len;
	{
		{
			wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
			stat = &(wlpptr->netDevStats);
			p += sprintf(p, "\n====================================================\n" );
			p += sprintf(p, "%s: rx statistics", netdev->name );
			p += sprintf(p, "\n-------------------------------\n" );
			p += sprintf(p, "rx_packets.......................%10u\n", (int)stat->rx_packets );
			p += sprintf(p, "rx_bytes.........................%10u\n", (int)stat->rx_bytes );
			p += sprintf(p, "rx_errors........................%10u\n", (int)stat->rx_errors );
			p += sprintf(p, "rx_dropped.......................%10u\n", (int)stat->rx_dropped );
			p += sprintf(p, "multicast........................%10u\n", (int)stat->multicast );
			p += sprintf(p, "rx_length_errors.................%10u\n", (int)stat->rx_length_errors );
			p += sprintf(p, "rx_over_errors...................%10u\n", (int)stat->rx_over_errors );
			p += sprintf(p, "rx_crc_errors....................%10u\n", (int)stat->rx_crc_errors );
			p += sprintf(p, "rx_frame_errors..................%10u\n", (int)stat->rx_frame_errors );
			p += sprintf(p, "rx_fifo_errors...................%10u\n", (int)stat->rx_fifo_errors );
			p += sprintf(p, "rx_missed_errors.................%10u\n", (int)stat->rx_missed_errors );

			p += sprintf(p, "\n====================================================\n" );
			p += sprintf(p, "%s: tx statistics", netdev->name );
			p += sprintf(p, "\n-------------------------------\n" );
			p += sprintf(p, "tx_packets.......................%10u\n", (int)stat->tx_packets );
			p += sprintf(p, "tx_bytes.........................%10u\n", (int)stat->tx_bytes );
			p += sprintf(p, "tx_errors........................%10u\n", (int)stat->tx_errors );
			p += sprintf(p, "tx_dropped.......................%10u\n", (int)stat->tx_dropped );
			p += sprintf(p, "tx_aborted_errors................%10u\n", (int)stat->tx_aborted_errors );
			p += sprintf(p, "tx_carrier_errors................%10u\n", (int)stat->tx_carrier_errors );
			p += sprintf(p, "tx_fifo_errors...................%10u\n", (int)stat->tx_fifo_errors );
			p += sprintf(p, "tx_heartbeat_errors..............%10u\n", (int)stat->tx_heartbeat_errors );
			p += sprintf(p, "tx_window_errors.................%10u\n", (int)stat->tx_window_errors );
			p += sprintf(p, "tx_headerroom_errors.............%10u\n", (int)wlpptr->wlpd_p->privStats.skbheaderroomfailure );
			p += sprintf(p, "tx_tsoframe_counts...............%10u\n", (int)wlpptr->wlpd_p->privStats.tsoframecount );

			p += sprintf(p, "\n====================================================\n" );
			p += sprintf(p, "%s(%s): statistics", "ap8xfw", netdev->name );
			p += sprintf(p, "\n-------------------------------\n" );
			p += wlFwGetHwStats(netdev, p);
		}
	}
	len = (p - page) - off;
	if (len < 0)
		len = 0;

	*eof = (len <= count) ? 1 : 0;
	*start = page + off;

	return len;
}
#define AP8X_STAT_PROC_FUN(index) \
static int ap8x_proc_read##index(char *page, char **start, off_t off, int count, int *eof, void *data){ \
	struct net_device *netdev = devstat[index].netdev; \
	return ap8x_proc_read(netdev, page, start, off, count, eof, data); \
}

AP8X_STAT_PROC_FUN ( 0);
AP8X_STAT_PROC_FUN ( 1);
AP8X_STAT_PROC_FUN ( 2);
AP8X_STAT_PROC_FUN ( 3);
AP8X_STAT_PROC_FUN ( 4);
AP8X_STAT_PROC_FUN ( 5);
AP8X_STAT_PROC_FUN ( 6);
AP8X_STAT_PROC_FUN ( 7);
AP8X_STAT_PROC_FUN ( 8);
AP8X_STAT_PROC_FUN ( 9);
AP8X_STAT_PROC_FUN ( 10);
AP8X_STAT_PROC_FUN ( 11);
AP8X_STAT_PROC_FUN ( 12);
AP8X_STAT_PROC_FUN ( 13);
AP8X_STAT_PROC_FUN ( 14);
AP8X_STAT_PROC_FUN ( 15);
AP8X_STAT_PROC_FUN ( 16);
AP8X_STAT_PROC_FUN ( 17);
AP8X_STAT_PROC_FUN ( 18);
AP8X_STAT_PROC_FUN ( 19);


int ap8x_proc_write (struct file *file, const char *buffer,
					 unsigned long count, void *data) 
{
	if (buffer && buffer[0] == 'l')
	{
		WLDBG_LEVELS=atohex2(&buffer[1]);
	}
	else
	{
		int i;
		for (i = 0; i < WL_MAXIMUM_INSTANCES; i++)
		{
			if(devstat[i].netdev)
				clear_ap8x_stat(devstat[i].netdev);
		}
	}
	return count;
}
int ap8x_stat_proc_register(struct net_device *dev)
{
#define WL_PROC(x) ap8x_proc_read##x
#define WL_CASE(x) \
	case x: \
	devstat[x].ap8x_proc->read_proc = WL_PROC( x); \
	break

	if(devstat_index >=(WL_MAXIMUM_INSTANCES))
	{
		printk("Error: more than %d instances not supported\n", WL_MAXIMUM_INSTANCES);
		return 1;
	}
	if (ap8x==NULL)
	{
		ap8x = proc_mkdir("ap8x", proc_net);
		if(!ap8x)
			return 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
		ap8x->owner = THIS_MODULE;
#endif
	}

	devstat[devstat_index].netdev = dev;

	sprintf(devstat[devstat_index].filename,  "%s_stats", dev->name);
	devstat[devstat_index].ap8x_proc = create_proc_entry(devstat[devstat_index].filename, 0666 , ap8x);

	if(!devstat[devstat_index].ap8x_proc)
	{	
		printk("create_procfs_file %s failed\n", devstat[devstat_index].filename);
		return 1;
	}
		
	switch (devstat_index)
	{
		WL_CASE(0);
		WL_CASE(1);
		WL_CASE(2);
		WL_CASE(3);
		WL_CASE(4);
		WL_CASE(5);
		WL_CASE(6);
		WL_CASE(7);
		WL_CASE(8);
		WL_CASE(9);
		WL_CASE(10);
		WL_CASE(11);
		WL_CASE(12);
		WL_CASE(13);
		WL_CASE(14);
		WL_CASE(15);
		WL_CASE(16);
		WL_CASE(17);
		WL_CASE(18);
		WL_CASE(19);
		default:
			break;
	}			
	devstat[devstat_index].ap8x_proc->write_proc = ap8x_proc_write;
	devstat[devstat_index].ap8x_proc->nlink = 1;
	devstat_index++;
	return 0;
#undef WL_PROC
#undef WL_CASE
}
int ap8x_stat_proc_unregister(struct net_device *dev)
{
	int i;
	for (i = 0; i < WL_MAXIMUM_INSTANCES ; i++)
	{
		if (devstat[i].netdev == dev)
		{
			remove_proc_entry(devstat[i].filename, ap8x);
			devstat_index--;
			return 0;
		}
	}
	return 0;
}

int ap8x_remove_folder(void )
{
	if(ap8x)
	{
		remove_proc_entry("ap8x", proc_net);
		ap8x=NULL;
		return 0;
	}
	return 1;
}

#undef WL_MAX_FILE_NAME_LEN
#undef WL_MAXIMUM_INSTANCES

#endif

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

#ifndef __EWB_HASH_H__
#define __EWB_HASH_H__


#define HASH_MAX_BUCKET         256
//#define HASH_ENTRY_MAX          HASH_MAX_BUCKET
#define HASH_ENTRY_COLUMN_MAX   256
#define HASH_ENTRY_ROW_MAX      64


typedef struct hash_entry{
	unsigned char *prvEntry;
    unsigned char   numInRow;
	unsigned long    nwIpAddr;
	unsigned char    hwAddr[6];
	unsigned char *nxtEntry;
}hash_entry;

typedef struct hash_table{
	hash_entry entry[HASH_MAX_BUCKET];
}hash_table;


extern int wetUpdateHashEntry(unsigned long key, unsigned char *clntMac);
extern unsigned char *wetGetHashEntryValue(unsigned long key);
extern int wetClearHashEntry(unsigned long key);
extern void wetHashInit(void);
extern void wetHashDeInit(void);

extern hash_entry hashTable[HASH_ENTRY_COLUMN_MAX];
#endif /* __EWB_HASH_H__ */

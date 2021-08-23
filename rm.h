// 
//	Record Manager component interface
//	

#ifndef RM_H
#define RM_H

#include "comp.h"

//
// RMFileHdr - 文件头部
// 
struct RMFileHdr {
	int free;				// 链表中第一个空闲页
	int size;				// 文件中已经分配了的页的数目
	uint rcdlen;			// 记录的大小, 该文件中每一个 record 长度
} __attribute__((packed));

// 存储数据的 page 中都有这个
struct RMPageHdrEx {
	int free; // 标记该 page 是否已经被使用，还是该 page 是空闲链表的尾
	int slots; // 存的是该页能存的 record 数量
	int remain; // 还有多少 record 可以用
	char slotMap[];
} __attribute__((packed));

/*
 
		+-----------------------+
		|	free				|
		|-----------------------|
		|	slots				|
		|-----------------------|
		|	remain				|
		|-----------------------|
		|	slotMap				|
		|						|
		|						|
		|						|
		+-----------------------+
 */

/*                                      RMPageHdr 类                            
									  这里才是存数据的地方								   
									   +--------+
				RMFileHdr类	     	   | free   |
				 第一个 page            +--------+
				 存放 RMFileHdr         | slots  |     一条 record 在 slotmap 中占一个字节
                +-------+              +--------+         +-+-+-+-+---+    
				| free  |              | remain |     +-->|1|1|0|0|...| BitMap类
				+-------+              +--------+     |   +|+-+-+-+---+
				| size  |              | slotmap| ----+    +--------+
				+-------+              +--------+                  \|/
				| rcdlen|              |  data  |               +----------+----------+-----+----------+
				+-------+              |  ...   |-------------->| record 0 | record 1 | ... | record n |
				| ...   |              +--------+               +----------+----------+-----+----------+
				+-------+                  /|\                    RMRecord类 record id 是由 slot 和 page num 共同组成
                    /|\                     +-----+     
					 +--+                         |
        +---------------|------------------+      |
        |            +--| 4096 --------+   |      |
        |           \|/ | Page        \|/ \|/     |
	+----------------+-----------------+-----------------+-----------------+-----------------+-----+
    | free(1)|size(4)|free(-2)|  data  |free(3) |  data  |free(-2)|  data  |free(-1)|  data  | ... |
    +----------------+-----------------+-----------------+-----------------+-----------------+-----+
     PFFileHdr类        PFPageHdr page     |                                   /|\ (空闲 page 链的终点)
					  (-2 表示被   存数据    +------------------------------------+
					      使用。  
					   -1 表示空
					      闲page
						  链终点)
				           
						   page 0             page 1              page 2          page 3    ...

						   
*/





#endif /* RM_H */
#ifndef PF_H
#define PF_H
#include <assert.h>
#include "redbase.h"
//
// Page����Ψһ�ر�־�ļ���һ��ҳ
//
typedef int Page;
typedef unsigned int uint;


// Page Size
// ÿһ��ҳ���ж�������ͷ����һЩ��Ϣ��PageHdr������pf_internal.h�ļ��С���������
// ����Ӧ�������һЩ��Ϣ�����Ǻܲ��ң���������ǲ���ʹ��sizeof(PageHdr),����ʵ����
// sizeof(PageHdr) == sizeof(int).
const int PF_PAGE_SIZE = 4096 - sizeof(int);

//
// FileHdr - ÿ�������ļ��Ŀ�ͷ��������һ���ṹ,������������ļ���ʹ�����
//
struct PFFileHdr {
	int free;		/* ����ҳ��ɵ����� */
	uint size;		/* ҳ����Ŀ */
};
/*


        +----------------------------------+
        |            +--- 4096 --------+   |
       \|/          \|/   Page        \|/ \|/
	+----------------+-----------------+-----------------+-----------------+-----------------+-----+
    | free(2)|size(4)|free(-2)|  data  |free(4) |  data  |free(-2)|  data  |free(-1)|  data  | ... |
    +----------------+-----------------+-----------------+-----------------+-----------------+-----+
     PFFileHdr         PFPageHdr   page    |                                   /|\
					  (-2 ��ʾ��   ������    +------------------------------------+
					      ʹ�á�  
					   -1 ��ʾ��
					      ��page
						  ���յ�)
				           
						   page 1             page 2              page 3          page 4
*/

#endif /* PF_H */
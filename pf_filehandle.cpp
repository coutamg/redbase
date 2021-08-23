#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include "pf_internal.h"
#include "pf_buffer.h"
#include "pf.h"
#include "util.h"
#include "pf_filehandle.h"
#include "pf_pagehandle.h"
#include "pf_error.h"
using namespace RedBase;

// ����Ĵ����ҳ��Ĵ�����һЩ����,����,������ǰ�����ڵ�ҳ�������߼���ǰ�����ڵ�ҳ,
// ��Ϊ�е�ʱ��,����Ҫ����һЩҳ,�ᵼ�´��̿ն��Ĳ���,Ϊ�˱�����Щ�ն�,���ǽ��������ٶ��ػ����ɵ�ҳ
// ������һ������,�����ͷ�����ļ�ͷ��.
// ����˵һ��,�����Ľṹ�����.��û��ʲô�������ԸĽ�?


//
// getFirstPage - ��ȡ��һ��ҳ��,����������PFPageHandle����һ��ʵ����
//	
RC PFFileHandle::firstPage(PFPageHandle &page) const
{
	return nextPage((Page)-1, page);
}

//
// getLastPage - ��ȡ���һ��ҳ��,�����������PFPageHandleʵ����
//	
RC PFFileHandle::lastPage(PFPageHandle &page) const
{
	return prevPage(hdr_.size, page);
}

//
// getNextPage - ��ȡ��ǰҳ�����һ��ҳ��,�������������PFPageHandle��һ��ʵ����
// 
RC PFFileHandle::nextPage(Page curr, PFPageHandle &page) const
{
	RC rc;
	if (!opened_) return PF_CLOSEDFILE;
	if (curr < -1 || curr >= hdr_.size) return PF_INVALIDPAGE;
	// ɨ���ļ�ֱ��һ����Ч��ʹ�ù���ҳ�汻�ҵ�
	for (curr++; curr < hdr_.size; curr++) {
		if (!(rc = getPage(curr, page))) return 0;
		if (rc != PF_INVALIDPAGE) return rc;
	}
	return PF_EOF;
}

//
// getPrevPage - ��ȡǰһ��ҳ�����Ϣ,������������PFPageHandle��һ��ʵ����
//	
RC PFFileHandle::prevPage(Page curr, PFPageHandle &page) const {
	RC rc;
	if (!opened_) return PF_CLOSEDFILE;
	if (curr <= 0 || curr >= hdr_.size) return PF_INVALIDPAGE;

	// ɨ���ļ���ֱ���ҵ�һ����Ч��ҳ,��ν����Ч��ҳ,ָ����
	for (curr--; curr >= 0; curr--) {
		if (!(rc = getPage(curr, page)))
			return 0;

		// If unexpected error, return it
		if (rc != PF_INVALIDPAGE)
			return rc;
	}
	return PF_EOF;
}

RC PFFileHandle::pin(Page num)
{
	if (!opened_) return PF_CLOSEDFILE;
	if (num < 0 || num >= hdr_.size) return PF_INVALIDPAGE;
	return buff_->pin(fd_, num);
}

//
// getPage - ��ȡ�ļ���ָ����ҳ, �������������һ��PFPageHandle��һ��ʵ��
// 
RC PFFileHandle::getPage(Page num, PFPageHandle &page) const
{

	if (!opened_) return PF_CLOSEDFILE;
	if (num < 0 || num >= hdr_.size) return PF_INVALIDPAGE;
	Ptr addr;
	buff_->getPage(fd_, num, addr);
	PFPageHdr *hdr = (PFPageHdr *)addr;
	// ���ҳ���Ѿ���ʹ���ˣ���ô��pphָ�����ҳ��
	if (hdr->free == PF_PAGE_USED) {
		page.num_ = num;
		// ע�⵽�����data_,ָ��Ĳ�����ҳ���ײ�,�����ײ�����4���ֽڴ�
		page.addr_ = addr + sizeof(PFPageHdr);
		return 0; // һ������
	}
	// ��������Ļ�,ҳ����Ϊ��
	unpin(num);
	return PF_INVALIDPAGE;
}



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
//
// allocatePage - ���ļ� buffer �з���һ���µ�ҳ��, ͨ�� page num ӳ�䵽������
// 
RC PFFileHandle::allocPage(PFPageHandle &page)
{
	RC rc;		// ������
	Page num;
	Ptr addr;

	if (!opened_) return PF_CLOSEDFILE;

	// ����˵�������п��е� page
	if (hdr_.free != PF_PAGE_LIST_END) { // ��Ȼ���ڿ���ҳ��,ȡ��һ��
		num = hdr_.free;
		if (rc = buff_->getPage(fd_, num, addr)) return rc;
		// tofix - �������Ǵ������ʵ�,�Ǿ����µõ���ҳ���free��ʼ��������? ��ʼ����
		hdr_.free = ((PFPageHdr *)addr)->free; // �ն���Ŀ��1
	}
	else { // ��������Ϊ��
		// �����ʱ�� num ��������� page �ڴ����е�λ����
		num = hdr_.size; // page num ֱ���Ǵ� PFFileHdr �� size �л�ȡ
		// ����һ���µ�ҳ��, Ŀǰ�Ǵ��ڴ��з����
		if (rc = buff_->allocPage(fd_, num, addr)) return rc;
		hdr_.size++;
	}
	changed_ = true; // �ļ������˱䶯
	// �����ҳ����ΪUSED���·�������� page
	((PFPageHdr *)addr)->free = PF_PAGE_USED;
	memset(addr + sizeof(PFPageHdr), 0, PF_PAGE_SIZE);
	// ��ҳ����Ϊ��
	markDirty(num);
	// ��ҳ�����Ϣ����pph��
	page.num_ = num;
	page.addr_ = addr + sizeof(PFPageHdr); // ָ��� page �����ݵ��׵�ַ
	return 0;
}

//
// disposePage - ����һ��ҳ��,��Ҫע�����,PFPageHandleʵ��ָ��Ķ���
// Ӧ�ò��ٱ�ʹ���ˣ��ڵ������������֮��
// 
RC PFFileHandle::disposePage(Page num)
{
	Ptr addr;
	if (num < 0 || num >= hdr_.size) return PF_INVALIDPAGE;
	// �����Լ�飬�ļ�����Ҫ�򿪣�Ȼ��ҳ�������Ч
	buff_->getPage(fd_, num, addr);
	PFPageHdr *hdr = (PFPageHdr *)addr;
	// ҳ�������Ч,free == PF_PAGE_USED��ʾҳ�����ڱ�ʹ��
	if (hdr->free != PF_PAGE_USED) {
		unpin(num);
		return PF_PAGEFREE;
	}
	// �����ͷ�巨
	/*
			PFFileHdr			num1 page          num2 page
		+-------------+     +-------------+    +-------------------------+
		| free = num1 |  -> | free = num2 | -> | free = PF_PAGE_LIST_END |
		+-------------+     +-------------+    +-------------------------+
	*/
	hdr->free = hdr_.free; // �����ٵ�ҳ�����������,��ô����Ϊ�˱�������ն�
	hdr_.free = num;

	changed_ = true;
	markDirty(num);
	unpin(num);
	return 0;
}

//
// markDirty - ��һ��ҳ����Ϊ��ҳ��,���ҳ�����Ƴ���������ʱ��ᱻд�뵽������ȥ��
// 
RC PFFileHandle::markDirty(Page num) const
{
	if (!opened_) return PF_CLOSEDFILE;
	if (num < 0 || num >= hdr_.size) return PF_INVALIDPAGE;
	return buff_->markDirty(fd_, num);
}

//
// flushPages - �����е����ҳ���д��������ȥ.
// 
RC PFFileHandle::flush()
{
	if (!opened_) return PF_CLOSEDFILE;
	int n; 
	// ���ͷ�����޸��ˣ���ô����д�ص�������
	if (changed_) {
		Lseek(fd_, 0, L_SET);
		// дͷ��
		n = Write(fd_, &hdr_, sizeof(PFFileHdr));
		if (n != sizeof(PFFileHdr)) return PF_HDRWRITE;
		changed_ = false;
	}
	return buff_->flush(fd_); // ������ˢ��������
}


//
// unpin - ����б�Ҫ�Ļ������ҳ�潫�ᱻд�ص������С�
//	
RC PFFileHandle::unpin(Page num) const
{
	if (!opened_) return PF_CLOSEDFILE;
	if (num < 0 || num >= hdr_.size) return PF_INVALIDPAGE;
	return buff_->unpin(fd_, num);
}

RC PFFileHandle::forcePages(Page num /* = ALL_PAGES */)
{
	if (!opened_) return PF_CLOSEDFILE;
	if (changed_) {
		Lseek(fd_, 0, L_SET);
		int size = sizeof(PFFileHdr);
		int n = Write(fd_, &hdr_, size);
		if (n != size) return PF_HDRWRITE;
		changed_ = false;
	}
	return buff_->forcePages(fd_, num);
}

void PFFileHandle::clearFilePages()
{
	buff_->clearFilePages(fd_);
}

PFPageHandle PFAllocPage(PFFilePtr& file)
{
	PFPageHandle page;
	RC rc = file->allocPage(page);
	if (rc != 0) {
		PFPrintError(rc);
		exit(0);
	}
	page.setOwner(file);
	return page;
}

PFPageHandle PFGetPage(PFFilePtr &file, Page num)
{
	PFPageHandle page;
	RC rc = file->getPage(num, page);
	if (rc != 0) {
		PFPrintError(rc);
		exit(0);
	}
	page.setOwner(file);
	return page;
}


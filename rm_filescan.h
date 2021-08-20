#ifndef RM_FILESCAN_H
#define RM_FILESCAN_H
#include "rm.h"
#include "rm_error.h"
#include "rm_filehandle.h"
#include "rm_record.h"

//
// RMFileScan - ������¼��ɨ��
//
class RMFileScan {
public:
	RMFileScan() 
		: comp_(nullptr), curr_(1, -1), op_(NO_OP)
		, offset_(0), val_(nullptr) {};
	~RMFileScan() {};
public:
	/*
		�˷���Ӧ��ʼ���Բ���fileHandle���õĴ��ļ��еļ�¼��ɨ�衣��ɨ���ڼ䣬ֻӦ����
		��ָ����������ָ����������ֵ�ıȽϣ��ļ�¼�����ֵ�ǿ�ָ�룬�򲻴����κ�������������
		ɨ���ڼ�������м�¼�����value���ǿ�ָ�룬��valueָ��Ҫ�����Խ��бȽϵ�ֵ��

		����attrType��attrLengthָʾҪ�Ƚϵ����Ե����ͺͳ��ȣ�4�ֽ�������4�ֽڸ�������
		���Ƚ���1��MAXSTRINGLEN�ֽ�֮����ַ�����MAXSTRINGLEN = 255 ������ redbase.h
		����AttrType��redbase.h�ж������£�INT��ʾ������FLOAT��ʾ��������STRING��ʾ�ַ�����

		����Ҫ��ֵת��Ϊ���Ե��ʵ����ͣ����ߣ����������򸡵㣬���临�Ƶ������ı������Ա������
		���⣩������ַ����ĳ���Ϊn�������Ժ�ֵ�ĳ��ȶ���ǡ��Ϊn�ֽڡ����ǲ���С�ڵ���n���ֽڣ�
		������Ҫ����䡱�����Ҳ�����null��β������attrOffsetָʾ��ÿ����¼���������ҵ����Ե�
		λ�á����� Operator ָʾ��¼������ֵ��value�������бȽϵķ�ʽ��

		�ο�: https://web.stanford.edu/class/cs346/2015/redbase-rm.html
	*/
	RC openScan(RMFilePtr& rmfile, AttrType attr, int len,
		int offset, Operator op,const void* val);
	RC getNextRcd(RMRecord &rcd);
	RC rewind();
	RC closeScan();
private:
	Comp *comp_;
	int offset_;
	Operator op_;
	const void* val_;
private:
	RID curr_;
	RMFilePtr rmfile_;
};

#endif /* RM_FILESCAN_H */
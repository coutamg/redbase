#ifndef RM_FILESCAN_H
#define RM_FILESCAN_H
#include "rm.h"
#include "rm_error.h"
#include "rm_filehandle.h"
#include "rm_record.h"

//
// RMFileScan - 用作记录的扫描
//
class RMFileScan {
public:
	RMFileScan() 
		: comp_(nullptr), curr_(1, -1), op_(NO_OP)
		, offset_(0), val_(nullptr) {};
	~RMFileScan() {};
public:
	/*
		此方法应初始化对参数fileHandle引用的打开文件中的记录的扫描。在扫描期间，只应检索
		其指定属性满足指定条件（与值的比较）的记录。如果值是空指针，则不存在任何条件，并且在
		扫描期间检索所有记录。如果value不是空指针，则value指向要与属性进行比较的值。

		参数attrType和attrLength指示要比较的属性的类型和长度：4字节整数、4字节浮点数或
		长度介于1和MAXSTRINGLEN字节之间的字符串。MAXSTRINGLEN = 255 定义在 redbase.h
		类型AttrType在redbase.h中定义如下：INT表示整数，FLOAT表示浮点数，STRING表示字符串。

		您需要将值转换为属性的适当类型（或者，对于整数或浮点，将其复制到单独的变量中以避免对齐
		问题）。如果字符串的长度为n，则属性和值的长度都将恰好为n字节。它们不会小于等于n个字节，
		即不需要“填充”，并且不会以null结尾。参数attrOffset指示在每个记录的内容中找到属性的
		位置。参数 Operator 指示记录的属性值与value参数进行比较的方式。

		参考: https://web.stanford.edu/class/cs346/2015/redbase-rm.html
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
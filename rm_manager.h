#ifndef RM_MANAGER_H
#define RM_MANAGER_H
#include "pf.h"
#include "rm.h"
#include "rm_error.h"
#include "rm_filehandle.h"
#include "pf_manager.h"

/*
RM组件提供用于管理无序 record 文件的类和方法,
RM组件是PF组件的客户端, RM组件将 record 存储在PF组件提供的分页文件中,
为了方便地管理文件内容，您可能希望使用每个文件的第一页作为特殊的头页, 此页
面应包含可用空间信息，以及您认为对实现有用的任何其他信息（与整个文件相关）,
您还必须确定 record 在PF文件页面上的布局方式, 每个文件将包含一组大小相同
的 record，这一事实简化了您的设计任务（尽管不同文件的 record 大小可能不同）,
固定大小的 record 更易于管理 record 和每页上的可用空间，固定大小的 record
允许给定文件中的 record identifiers 是 page number 和 record position 的
简单组合。
*/
class RMManager {
public:
	RMManager() {};
	~RMManager() {};
public:
	RC createFile(const char* pathname, uint rcdlen);
	RC destroyFile(const char* pathname);
	RC openFile(const char* pathname, RMFilePtr &rmfile);
	RC closeFile(RMFilePtr& rmfile);
};

#endif /* RM_MANAGER_H */
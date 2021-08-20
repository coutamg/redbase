这份源码来源自斯坦福大学的Redbase，我做了一定的改进，也基本上通过了一系列的测试。

这份源码里，我删除掉了原来大批量的测试代码，以及gtest的依赖，但是放心，代码是通过了测试才删掉了测试代码的。

改进后的代码和源代码风格相近，但是不完全一致，因为我不太喜欢原来的编码风格，整个代码不太优美，我感觉很难做到优美，就这样吧。我自己写了parser，没用yacc生成，改了一些函数，希望让它们更加通俗易懂，但是效果貌似也不是太好。

所以，如果你想看一下我写的代码的话，还是要对照着Redbase课程提供的说明来看的。

代码不完整，几乎没有做错误处理工作，没做update，insert等一些命令，但是查询命令完成了，为什么会这样呢？因为我懒。查询命令基本是最难啃的一部分了，其余的，肯花时间的话，没有太大难度。

# 如何运行？
首先软件的运行平台是linux，进入该文件夹，执行
```shell
make
```
可以完成代码的编译工作，当前目录下会生成三个程序，分别是**dbcreate**, **RedBase**, **dbdestroy**。

`dbcreate`用于构建数据库，使用方法为:
```bash
./dbcreate dbname  # dbname为数据库名称
```
`dbdestroy`用于删除数据库，使用方法为：
```bash
./dbdestroy dbname # dbname为数据库名称
```
`RedBase`为数据库程序，使用方法为：
```bash
./Redbase dbname
```

这里给一份测试代码：
```bash
/* Test semantic checking.  */

create table soaps(soapid  i, sname  c28, network  c4, rating  f);
create table stars(starid  i, stname  c20, plays  c12, soapid  i);

/* 加载数据 */
load soaps("../data/soaps.data");

/* print out contents of soaps */
print soaps;

/* build some indices on stars */
create index stars(starid);
create index stars(stname);


load stars("../data/stars.data");

print stars;
select * from stars;

select * from stars, soaps;

select soaps.network, rating from stars, soaps;


select soaps.network, rating from stars, soaps where soaps.soapid = stars.soapid;

print stars;

exit;

```

## File and Page Layout

each file to have a file header page on which you store information about the file 
as a whole, followed by a set of data pages. Information that might be stored on 
the header page includes the size of the records in the file, the number of records 
that may be stored on each page, the current number of pages in the file, the location 
of pages with free space, etc. Each data page will contain some header information 
and some records.

## File Header Management
When the RM_Manager::OpenFile method is called, it should call PF_Manager::OpenFile 
to actually open the file. You will probably then want to copy the file header information 
into a private variable in the file handle that refers to the open file instance. 
By copying this information, you will subsequently be able to find out details such as 
the record size and the number of pages in the file by looking in the file handle instead 
of reading the header page again (or keeping the information on every page). Once you have 
read the header information from the header page into the file handle, you can unpin the 
header page since there is no need to waste buffer space by keeping the header page in the 
buffer pool the entire time the file is open. Note, however, that any changes made to the 
header while the file is open (e.g., the number of pages in the file, or the location of free 
space) must be written back when the file is closed. You can do this by keeping a modified 
flag in the file handle and writing the information back when the flag is set.

## Record Identifiers
The RID class described above defines unique identifiers for records within a given file. 
Record identifiers will serve as tuple identifiers for higher-level RedBase components. 
Thus, the identifier for a given record should be permanent: the components of a record 
identifier should not change if the record is updated, or as the result of an insertion 
or deletion of a different record.

## Keeping Track of Free Space
When inserting records, you are strongly discouraged from performing a linear search through
pages in order to find a page with free space. One solution is to effectively create a linked
list of pages that have empty slots in them. You can do this by placing appropriate pointers
in page headers, with a pointer to the first page in the list included in the file header. 
When you need to insert a record, insert it into an empty slot in the first page of the list. 
You will need to modify the list as records are inserted and deleted.

There are a variety of ways to keep track of free record slots on a given page. One efficient 
method is to use a bitmap: If each data page can hold n records, then you can store an n-bit 
bitmap in the page header indicating which slots currently contain valid records and which slots 
are available. Note that the size of the bitmap needed for each page is dependent on the number
of records that can be stored on that page, which in turn is dependent on the record size for the 
file. Hence your bitmaps will be the same size for each page of a given file, but bitmap sizes 
should be different for different files. One way to implement variable-size bitmaps is as arrays 
of char's or int's, where the individual bits are manipulated using the operators &, |, and ^.

Note that under no circumstances should you place a limit on the total number of records that 
can be stored in a file -- each file should be able to grow arbitrarily large. (Admittedly 
there is an indirect limit because PageNum is defined as an integer, but it should be possible 
to change the type of PageNum -- say to a long integer -- without changing your code.)


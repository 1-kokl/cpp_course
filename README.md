已完成底层存储引擎全部设计与实现，包含：
1. 核心数据结构（避免STL容器）
MyVector<T>: 手写的模板化动态数组，实现了自动扩容 (resize)、元素访问 (operator[]) 和大小管理 (size, len)，替代了 std::vector。
MyString: 手写的字符串类，实现了深拷贝构造函数、赋值运算符（防止 Double Free）、c_str() 转换及内存自动释放，替代了 std::string 用于数据存储。
BPlusTree: 实现了 B+ 树索引结构。
支持叶子节点分裂 (split_leaf)，确保数据量增加时树结构的平衡。
支持父节点插入 (insert_into_parent) 及根节点分裂。
支持主键的快速搜索 (search)、插入 (insert) 和删除 (remove)。
2. 数据库与表管理 (DDL)
StorageEngine (单例模式):
createDatabase: 在文件系统创建数据库目录。
dropDatabase: 删除数据库目录及内存对象。
useDatabase: 切换当前活跃数据库上下文。
createTable: 创建表对象，初始化文件，并自动从磁盘加载历史数据。
getTable: 获取已存在的表对象。
Database: 管理多张表的集合，提供表的注册与查找功能。
Table:
add_column: 定义表结构（列名、类型、主键标识）。
自动识别主键列并初始化 B+ 树索引。
3. 数据操作 (DML)
Insert (Table::insert):
主键冲突检测：插入前检查 B+ 树，防止重复主键。
内存更新：将 Row 对象存入内存缓存。
索引更新：同步更新 B+ 树索引。
持久化写入：以二进制追加模式写入 .dat 文件。
Select (Table::select_by_primary_key):
利用 B+ 树索引实现 $O(\log N)$ 级别的主键精确查询。
Delete (Table::remove_by_primary_key):
同步删除内存中的行记录、B+ 树索引节点。
Update (Table::update_by_primary_key):
通过“先删后插”的逻辑实现主键记录的更新。
Scan All (Table::scan_all):
支持全表扫描，返回所有内存中的行记录（用于无条件查询或范围过滤的基础）。
4. 持久化与恢复 (Persistence)
二进制文件格式: 设计了紧凑的二进制存储格式 [RowID][PK_Int][StrLen][StrData]。
自动加载 (Table::load_from_disk):
在 StorageEngine::createTable 时自动调用。
解析 .dat 文件，重建内存中的 Row 对象。
重建索引：加载数据的同时重新插入 B+ 树，确保重启后索引可用。
ID 计数器恢复：自动更新 next_row_id，防止 ID 冲突。
5. 接口封装
提供了C++ API 供上层（SQL 解析器/网络层）调用。

DBMS
====

This repo contains a basic version of a relational database system. DiskManager, StorageManager and BufferManager were developed along with an indexing layer using b+ tree.
More details for Minirel RDBMS can be found at http://research.cs.wisc.edu/coral/mini_doc/logMgr/report/node3.html

This project has the following four parts:

* **Disk Manager:** This module is responsible for maintaining the disk level access. In this project, our database is a regular file, and the disk manager is responsible for creating, opening/closing and resizing the database. Along with that, it is responsible for block level read/write into the database.
* **

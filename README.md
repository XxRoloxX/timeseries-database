# Time Series Database
## 1. Introduction

The goal of this project is to create a database optimized for applications involving time-series data.
## 2. Problem Characteristics

Time-series databases store vast amounts of data, which are written and read in the context of their "creation" timestamp. The most common uses of such databases are the collection of metrics or events that are only significant in the context of their time of occurrence. These data types are characterized by high repetition rates and very high write frequencies (much higher than read frequencies, which is unusual for database systems). Additionally, due to the nature of their applications, such databases typically do not require strict consistency (in the case of scaling across multiple instances).

Therefore, it is crucial for the implementation of such a database to be optimized for non-blocking and efficient writes.

## 3. Current Solutions

Popular time-series databases, such as Prometheus, ScyllaDB, and Gorilla, use mechanisms like Log-Structured Merge Trees (LSM-trees), SSTables, and Memtables as write-back caches. These are fundamental building blocks for constructing a time-series database. A proper implementation of just these structures should be sufficient to achieve satisfactory results.

## 4. Functional Requirements

Data reading based on a range of record timestamps.
Record values should be numeric (either integer or floating point), similar to Prometheus.
Writing new records along with their timestamp.

## 5. Non-Functional Requirements

Optimization of non-blocking writes to the database.
Usage of host system files for saving the database state.

## 6. System Architecture

The system should employ state-of-the-art mechanisms in the context of time-series databases.
Database writing is split into two stages:
Write to Memtable, implemented as a red-black tree.
Write the record to the commit log (to prevent data loss in case of failure).
Once Memtable is full, write to SSTable.
Based on certain criteria (time-based or memory-based), the resulting SSTables are merged to optimize queries. Thanks to the append-only approach, most operations may be non-blocking.

Reading from the database is done by checking the presence of records within a given range in both Memtable and all corresponding SSTables. The TSDB will store individual timestamps of time-series along with references to their file and offset. Since SSTables contain sorted data, this allows limiting the key search to only the relevant range, which can be dynamically adjusted (at the cost of memory usage).

Each series has a separate LSM mechanism due to the typical query characteristics, which do not mix series.

## 7. Example Use Cases

Recording metrics from computer systems (CPU usage, network statistics/packets, execution of specific paths in source code).
Analytical data related to web systems (similar to Google Analytics / ClickHouse).

## 8. Additional tasks worth considering if time permits:

Support for efficient record encoding using the repetitive nature of records.
Use of Bloom filters to optimize queries.

## 9. Development Environment

Code Editor: Nvim (clangd, lldb) 

Test Suite: googletest

Debugger: gdb

Compiler: gcc

Build System: cmake

External Dependencies: boost

Version Control System: git

## 10. Sources, References, Inspirations:

- https://www.alibabacloud.com/blog/598114?spm=a3c0i.29367734.6737026690.4.235c7d3fJQAJtP
- https://opensource.docs.scylladb.com/stable/architecture/sstable/sstable3/sstables-3-data-file-format.html
- https://nakabonne.dev/posts/write-tsdb-from-scratch/
- https://en.wikipedia.org/wiki/Bloom_filter
- https://www.vldb.org/pvldb/vol8/p1816-teller.pdf
- https://www.scylladb.com/glossary/log-structured-merge-tree/
- https://kvs-vishnu23.medium.com/deep-dive-into-prometheus-4790a5916370
- https://www.geeksforgeeks.org/introduction-to-log-structured-merge-lsm-tree/
- https://www.geeksforgeeks.org/sstable-in-apache-cassandra/
- https://blog.cloudflare.com/how-cloudflare-runs-prometheus-at-scale/
- https://cloud.google.com/bigtable/docs/schema-design-time-series


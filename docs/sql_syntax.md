# The sql syntax of infinity

## Conventions

The following conventions are used in the synopsis of a command: brackets ([ and ]) indicate optional parts. (In the synopsis of a Tcl command, question marks (?) are used instead, as is usual in Tcl.) Braces ({ and }) and vertical lines (|) indicate that you must choose one alternative. Dots (...) mean that the preceding element can be repeated.  
Refer to [postgresql](https://www.postgresql.org/docs/9.1/notation.html#:~:text=The%20following%20conventions%20are%20used,you%20must%20choose%20one%20alternative.).

## Infinity syntax

### CREATE INDEX

CREATE INDEX — define a new index

#### Synopsis

```sql
CREATE INDEX [[IF NOT EXISTS] name] ON table_name (column_name[, ...]) USING method
[WITH (index_parameter [= value][, ...])];
```

#### Description

<!-- TODO shenyushi -->

#### Parameter

* IF NOT EXISTS  
    Do not throw an error if a relation with the same name already exists. A notice is issued in this case. Note that there is no guarantee that the existing relation is anything like the one that would have been created.
* name  
    The name (possibly schema-qualified) of the index to be created. If not specified, a name is chosen automatically.
* table_name  
    The name (possibly schema-qualified) of the table on which the index is to be created.
* method  
    The name of the index method to be used. For example, knn index choices of vector column(ex: embedding(float, 128)) are `IVFFlat`, `IVFSQ8` and `Hnsw`.
* column_name  
    The name of a column to create an index on. // TODO shenyushi: multiple columns denote what ?
* expression  
    An expression based on one or more columns of the table. The expression usually must be written with surrounding parentheses, as shown in the syntax. However, the parentheses can be omitted if the expression has the form of a function call.
* index_parameter  
    A parameter to be associated with the index method. The allowed parameters vary depending on the index method being used. See the documentation for the individual index methods for details. See [Index Parameters](#index-parameters) below for details.

##### Index Parameters

<!-- TODO shenyushi
such as: centroids_count, MetricType -->

### DROP INDEX

DROP INDEX — remove an index

#### Synopsis

```sql
DROP INDEX [IF EXISTS] name ON table_name;
```

### DESCRIBE INDEX name

#### Synopsis

```sql
DESCRIBE INDEX table_name;
```

### KNN SCAN

<!-- TODO shenyushi -->
<!-- #### Synopsis -->

#### Example

```sql
SELECT col1 FROM tbl1 SEARCH KNN(col2, [0.3, 0.3, 0.2, 0.2], 'float', 'l2', 2);
SELECT col1 FROM tbl1 SEARCH KNN(col2, [0.3, 0.3, 0.2, 0.2], 'float', 'ip', 2);
```

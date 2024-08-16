# Test Document

## infinity object
### infinity_obj.create_database
#### invalid name
- [X] database name 
  - `[],(),{},1,1.1`  

### infinity_obj.drop_database
  PASS
### infinity_obj.list_databases
  PASS
### infinity_obj.get_database
  PASS

## database object
### db_obj.create_table
#### invalid name/type/options
- [X] table name 
  - `[],(),{},1,1.1`
- [X] column name
  - `[],(),{},1,1.1`
- [X] column type
  - `[],(),{},1,1.1`
- [X] options
  - `[],(),{},1,1.1`
#### Stress Testing
- [ ] 1000 tables with 10000 columns with various column types.(only 745 tables create successfully)

### db_obj.drop_table
  PASS
### db_obj.get_table
  PASS
### db_obj.list_tables
  PASS


## table object

### table_obj.create_index
### table_obj.drop_index
### table_obj.insert
  PASS
#### Big int to Huge int
  - [ ] int128
### table_obj.import
### table_obj.delete
  PASS
### table_obj.update
  PASS
### table_obj.query_builder

## query builder

### query_builder.output
### query_builder.filter
### query_builder.match_dense
### query_builder.match_text
### query_builder.fusion
### query_builder.to_result
### query_builder.to_df
### query_builder.to_pl
### query_builder.to_arrow
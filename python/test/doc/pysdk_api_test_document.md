# Test Document

## infinity_obj.create_database

- [ ] create database with invalid name, when disconnect an error occurs 

## infinity_obj.create_table
### invalid name/type/options
- [ ] table name 
  - `[],(),{},1,1.1`
- [ ] column name
  - `[],(),{},1,1.1`
- [ ] column type
  - `[],(),{},1,1.1`
- [ ] options
  - `[],(),{},1,1.1`
### Stress Testing
- [ ] 1000 tables with 10000 columns with various column types.(only 745 tables create successfully)
- [x] 
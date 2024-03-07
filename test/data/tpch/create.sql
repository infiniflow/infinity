CREATE TABLE NATION  ( N_NATIONKEY  INTEGER NOT NULL,
                       N_NAME       VARCHAR(25) NOT NULL,
                       N_REGIONKEY  INTEGER NOT NULL,
                       N_COMMENT    VARCHAR(152),
                       PRIMARY KEY (N_NATIONKEY)
);

CREATE TABLE REGION  ( R_REGIONKEY  INTEGER NOT NULL,
                       R_NAME       VARCHAR(25) NOT NULL,
                       R_COMMENT    VARCHAR(152),
                       PRIMARY KEY (R_REGIONKEY)
);

CREATE TABLE PART  ( P_PARTKEY     INTEGER NOT NULL,
                     P_NAME        VARCHAR(55) NOT NULL,
                     P_MFGR        VARCHAR(25) NOT NULL,
                     P_BRAND       VARCHAR(10) NOT NULL,
                     P_TYPE        VARCHAR(25) NOT NULL,
                     P_SIZE        INTEGER NOT NULL,
                     P_CONTAINER   VARCHAR(10) NOT NULL,
                     P_RETAILPRICE DOUBLE NOT NULL,
                     P_COMMENT     VARCHAR(23) NOT NULL,
                     PRIMARY KEY (P_PARTKEY)
);

CREATE TABLE SUPPLIER ( S_SUPPKEY     INTEGER NOT NULL,
                        S_NAME        VARCHAR(25) NOT NULL,
                        S_ADDRESS     VARCHAR(40) NOT NULL,
                        S_NATIONKEY   INTEGER NOT NULL,
                        S_PHONE       VARCHAR(15) NOT NULL,
                        S_ACCTBAL     DOUBLE NOT NULL,
                        S_COMMENT     VARCHAR(101) NOT NULL,
                        PRIMARY KEY (S_SUPPKEY)
);

CREATE TABLE PARTSUPP ( PS_PARTKEY     INTEGER NOT NULL,
                        PS_SUPPKEY     INTEGER NOT NULL,
                        PS_AVAILQTY    INTEGER NOT NULL,
                        PS_SUPPLYCOST  DOUBLE  NOT NULL,
                        PS_COMMENT     VARCHAR(199) NOT NULL,
                        PRIMARY KEY (PS_PARTKEY, PS_SUPPKEY)
);

CREATE TABLE CUSTOMER ( C_CUSTKEY     INTEGER NOT NULL,
                        C_NAME        VARCHAR(25) NOT NULL,
                        C_ADDRESS     VARCHAR(40) NOT NULL,
                        C_NATIONKEY   INTEGER NOT NULL,
                        C_PHONE       VARCHAR(15) NOT NULL,
                        C_ACCTBAL     DOUBLE   NOT NULL,
                        C_MKTSEGMENT  VARCHAR(10) NOT NULL,
                        C_COMMENT     VARCHAR(117) NOT NULL,
                        PRIMARY KEY (C_CUSTKEY)
);

CREATE TABLE ORDERS  ( O_ORDERKEY       BIGINT NOT NULL,
                       O_CUSTKEY        INTEGER NOT NULL,
                       O_ORDERSTATUS    VARCHAR(1) NOT NULL,
                       O_TOTALPRICE     DOUBLE NOT NULL,
                       O_ORDERDATE      DATE NOT NULL,
                       O_ORDERPRIORITY  VARCHAR(15) NOT NULL,
                       O_CLERK          VARCHAR(15) NOT NULL,
                       O_SHIPPRIORITY   INTEGER NOT NULL,
                       O_COMMENT        VARCHAR(79) NOT NULL,
                       PRIMARY KEY (O_ORDERKEY)
);

CREATE TABLE LINEITEM ( L_ORDERKEY    BIGINT NOT NULL,
                        L_PARTKEY     INTEGER NOT NULL,
                        L_SUPPKEY     INTEGER NOT NULL,
                        L_LINENUMBER  INTEGER NOT NULL,
                        L_QUANTITY    INTEGER NOT NULL,
                        L_EXTENDEDPRICE  DOUBLE NOT NULL,
                        L_DISCOUNT    DOUBLE NOT NULL,
                        L_TAX         DOUBLE NOT NULL,
                        L_RETURNFLAG  VARCHAR(1) NOT NULL,
                        L_LINESTATUS  VARCHAR(1) NOT NULL,
                        L_SHIPDATE    DATE NOT NULL,
                        L_COMMITDATE  DATE NOT NULL,
                        L_RECEIPTDATE DATE NOT NULL,
                        L_SHIPINSTRUCT VARCHAR(25) NOT NULL,
                        L_SHIPMODE     VARCHAR(10) NOT NULL,
                        L_COMMENT      VARCHAR(44) NOT NULL,
                        PRIMARY KEY (L_ORDERKEY, L_LINENUMBER)
);

select
    p_brand,
    p_type,
    p_size,
    count(distinct ps_suppkey) as supplier_cnt
from
    partsupp,
    part
where
    p_partkey = ps_partkey
    and p_brand <> 'Brand#35'
    and p_type not like 'ECONOMY BURNISHED%'
    and p_size in (14, 7, 21, 24, 35, 33, 2, 20)
    and ps_suppkey not in (
    select
        s_suppkey
    from
        supplier
    where
        s_comment like '%Customer%Complaints%'
)
group by
    p_brand,
    p_type,
    p_size
order by
    supplier_cnt desc,
    p_brand,
    p_type,
    p_size
;

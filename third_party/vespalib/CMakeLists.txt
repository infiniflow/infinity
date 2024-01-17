# library target
add_library(vespalib STATIC
    btree/btree_key_data.cpp
    btree/btreeaggregator.cpp
    btree/btreebuilder.cpp
    btree/btreeinserter.cpp
    btree/btreeiterator.cpp
    btree/btreenode.cpp
    btree/btreenodeallocator.cpp
    btree/btreenodestore.cpp
    btree/btreeremover.cpp
    btree/btreeroot.cpp
    btree/btreerootbase.cpp
    btree/btreestore.cpp
    datastore/atomic_entry_ref.cpp
    datastore/buffer_free_list.cpp
    datastore/buffer_stats.cpp
    datastore/buffer_type.cpp
    datastore/bufferstate.cpp
    datastore/datastore.cpp
    datastore/datastorebase.cpp
    datastore/entry_ref_filter.cpp
    datastore/entryref.cpp
    datastore/free_list.cpp
    datastore/memory_stats.cpp
    util/address_space.cpp
    util/alloc.cpp
    util/array.cpp
    util/generationhandler.cpp
    util/generationholder.cpp
    util/hdr_abort.cpp
    util/memoryusage.cpp
    util/round_up_to_page_size.cpp
    util/stash.cpp
)

target_include_directories(vespalib PUBLIC "${CMAKE_SOURCE_DIR}/third_party")

if(CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF)
  binary(dwarfdump LIBS ${dwarf_lib})
endif()

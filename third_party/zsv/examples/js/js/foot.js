  var _loaded = false;
  const decoder = new TextDecoder("utf-8");

  function UTF8ToString2(ptr, len) {
    // modified from https://github.com/vtree-rs/vtree_html/blob/d3dd028be4a80e65bf885e71c2f4bd0815c73882/src/js/utils.js
    // using surrogate pair check used by j1
    // replaces bad utf8 with '??'
    const u8Array = HEAPU8;
    const endPtr = ptr + len;

    if (endPtr - ptr > 1024 && u8Array.subarray && decoder)
      return decoder.decode(u8Array.subarray(ptr, endPtr));

    let idx = ptr;
    let str = '';
    let last_surrogate = 0;
    while (idx < endPtr) {
      // For UTF8 byte structure, see http://en.wikipedia.org/wiki/UTF-8#Description and
      // https://www.ietf.org/rfc/rfc2279.txt and https://tools.ietf.org/html/rfc3629
      let u0 = u8Array[idx++];
      if (!(u0 & 0x80))
        str += String.fromCharCode(u0);
      else if(idx < endPtr) {
        let code;
	const u1 = u8Array[idx++] & 63;
	if ((u0 & 0xE0) === 0xC0) { // 110xxxxx
          code = ((u0 & 31) << 6) | u1;
        } else if(idx < endPtr) {
	  const u2 = u8Array[idx++] & 63;
	  if ((u0 & 0xF0) === 0xE0) // 1110xxxx
	    code = ((u0 & 15) << 12) | (u1 << 6) | u2;
	  else if(idx < endPtr) {
	    const u3 = u8Array[idx++] & 63;
	    if ((u0 & 0xF8) === 0xF0) // 11110xxx
	      code = ((u0 & 7) << 18) | (u1 << 12) | (u2 << 6) | u3;
	    else if(idx < endPtr) {
	      const u4 = u8Array[idx++] & 63;
	      if ((u0 & 0xFC) === 0xF8) // 111110xx
		code = ((u0 & 3) << 24) | (u1 << 18) | (u2 << 12) | (u3 << 6) | u4;
	      else if(idx < endPtr) {
		const u5 = u8Array[idx++] & 63;
		code = ((u0 & 1) << 30) | (u1 << 24) | (u2 << 18) | (u3 << 12) | (u4 << 6) | u5;
	      }
	    }
	  }

          // check for invalid surrogate pair
          if(0xD800 <= code && code <= 0xDBFF) {
            if(idx + 2 > endPtr) {
              str += '??'; // Invalid surrogate! Change to '??'
              continue;
            }
            let surrogate = u8Array[idx++] << 8;
            surrogate += u8Array[idx++];
            if(!(0xDC00 <= surrogate && surrogate <= 0xDFFF)) {
              str += '??'; // Invalid surrogate! Change to '??'
              idx -= 2;
              continue;
            }
            code = 0x10000 + (((code - 0xD800) << 10) |(surrogate - 0xDC00));
          }
        }
        if(code) {
          if(code > 0x10FFFF)
            code = 0xFFFD; // U+FFFD REPLACEMENT CHARACTER
          str += String.fromCharCode(u0);
        }
      }
    }
    return str;
  }

  Module.onRuntimeInitialized = function() {
    _loaded = true;
    run_if_loaded();
  };

  var do_on_load = [];
  function run_if_loaded() {
    if(_loaded)
      while(do_on_load.length)
        do_on_load.shift()();
  }

  function run_on_load(f) {
    do_on_load.push(f);
    run_if_loaded();
  }

  function str2C(s) {
    var size = lengthBytesUTF8(s) + 1;
    var ret = _malloc(size);
    stringToUTF8Array(s, HEAP8, ret, size);
    return ret;
  }

  /**
   * To support multiple concurrent parsers without increasing the number of
   * allocated function pointers required, we will create a global list of all
   * active parsers which will be reset when the last active parser has completed.
   *
   * All parsers will share a constant pool of 2 row handlers (see below), each
   * of which will then call the relevant instance-specific row handler
   */
  let activeParsers = [];
  let activeParser_count = 0;

  /**
   * Register two global row handlers, one that will aggregate the entire row
   * data before calling the row handler, and another that will leave it to the
   * row handler to determine which cells to fetch
   */
  function globalRowHandlerNoData(ix) {
    let z = activeParsers[ix];
    z.rowHandler(null, z.ctx, z);
  }

  function globalRowHandlerWithData(ix) {
    let z = activeParsers[ix];
    let count = z.cellCount();
    let row = [];

    // convert bytes to JS data
    if(z.outputIndexes) {
      for(let i = 0; i < z.outputIndexes.length; i++)
        row.push(z.getCell(z.outputIndexes[i]));
    } else {
      for(let i = 0; i < count; i++)
        row.push(z.getCell(i));
    }
    z.rowHandler(row, z.ctx, z);
  }

  function globalReadFunc(buff, n, m, ix) {
    let z = activeParsers[ix];
    let sz = n * m;
    if(sz != z.last_sz) {
      z.jsbuff = new Uint8Array(Module.HEAP8.buffer, buff, sz);
      z.sz = sz;
    }
    let bytes;
    try {
      bytes = fs.readSync(z.fd, z.jsbuff, 0, sz);
    } catch(e) {
      if(e.code == 'EAGAIN') {
        for(let i = 0; i < 100; i++)
          try {
            bytes = fs.readSync(z.fd, z.jsbuff, 0, sz);
            break;
          } catch(e1) {
          }
      }
      if(!bytes) {
        console.error('EAGAIN error: for stdin on linux/mac, node does not support sync-- use async instead', e.toString());
        throw new Error(e)
      }
    }
    z.bytesRead += bytes;
    return bytes;
  }

  let globalReadFuncp;
  let globalRowHandlerNoDatap, globalRowHandlerWithDatap;
  run_on_load(function() {
    globalReadFuncp = addFunction(globalReadFunc, 'iiiii');
    globalRowHandlerNoDatap = addFunction(globalRowHandlerNoData, 'vi');
    globalRowHandlerWithDatap = addFunction(globalRowHandlerWithData, 'vi');
  });

  let qmCode = '?'.charCodeAt(0);

  return {
    /**
     * create a push parser
     *
     * @param rowHandler callback with signature (row, ctx, parser)
     * @param ctx        a caller-defined value that will be passed to the row handler
     * @param options
     *        - rowData       if false, row data will not be passed to the row handler
     *        - sync          synchronous readableStream handle
     *        - async         async readableStream handle
     *        - end           function(ctx, parser) to attach to stream end event
     *        - outputIndexes array of 0-based indexes to output
     */
    new: function(rowHandler, ctx, options) {
      let zsv = _zsv_new(null);
      options = options || {};
      if(zsv) {
        function cellCount() {
          return _zsv_cell_count(zsv);
        };
        let z, o;

        function getCell(i) {
          let s = _zsv_get_cell_str(zsv, i);
          if(s)
            return UTF8ToString2(s, _zsv_get_cell_len(zsv, i));
          return '';
        };

        z = {
          zsv: zsv,
          rowHandler: rowHandler,
          outputIndexes: options.outputIndexes,
          cellCount: cellCount,
          getCell: getCell,
          buff: null,
          buffsize: 0,
          cellbuff: null,
          cellbuffsize: 0,
          ix: activeParsers.length,
          ctx: ctx,
          fd: 0,
          bytesRead: 0
        };

        function parseBytes(byte_array) {
          // this routine could be made more efficient by writing directly into
          // the parser buffer
          let len = byte_array.length;
          if(len) {
            // copy bytes into a chunk of memory that our library can access
            if(!(z.buffsize >= len)) {
              if(z.buff)
                _free(z.buff);
              z.buff = _malloc(len);
              z.buffsize = len;
            }
            // copy to memory that wasm can access, then parse
            writeArrayToMemory(byte_array, z.buff);
            return _zsv_parse_bytes(z.zsv, z.buff, len);
          }
        }

        if(options.sync) {
          if(!(options.sync.fd === 0 || options.sync.fd > 0))
            throw new Error('Sync read requires a file descriptor');
          z.fd = options.sync.fd;
          _zsv_set_read(zsv, globalReadFuncp);
          _zsv_set_input(zsv, z.ix);
        } else if(options.async) {
          options.async.on('data', function(data) {
            z.bytesRead += data.length;
            parseBytes(data);
          });
          let end = options.end;
          options.async.on('end', function() {
            o.finish();
            if(end)
              end(ctx, o);
            o.delete();
          });
        }

        o = {
          getBytesRead: function() {
            return z.bytesRead;
          },
          parseBytes: parseBytes,

          /** functions for async input */
          syncInput: function(fHandle) {
            z.fd = fHandle.fd;
            _zsv_set_read(zsv, globalReadFuncp);
            _zsv_set_input(zsv, z.ix);
          },
          parseMore: function() {
            return _zsv_parse_more(zsv);
          },
          finish: function() {
            return _zsv_finish(zsv);
          },
          abort: function() {
            return _zsv_abort(zsv);
          },
          delete: function() {
            if(z.buff)
              _free(z.buff);
            if(z.cellbuff)
              _free(z.cellbuff);
            activeParsers[z.ix] = null;
            activeParser_count--;
            if(activeParser_count == 0)
              activeParsers = [];
            return _zsv_delete(zsv);
          },
          cellCount: cellCount,
          getCell: getCell
        };

        activeParsers.push(z);
        activeParser_count++;
        _zsv_set_row_handler(zsv, options.rowData === false ? globalRowHandlerNoDatap : globalRowHandlerWithDatap);
        _zsv_set_context(zsv, z.ix);

        if(options.sync)
          while(_zsv_parse_more(zsv) == 0) // _zsv_status_ok
            ;
        return o;
      }
    },
    runOnLoad: run_on_load
  };
})();

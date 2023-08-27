/**
 * in this example, we use
 */

let zHandle;
let rowcount;
let start;
let data;
let bytes_read;

// reset our variables for a new parse
function reset() {
  rowcount = 0;
  start = performance.now();
  data = [];
  bytes_read = 0;
}

// handle a single parsed row
function rowHandler() {
  rowcount++;
  let count = zHandle.cellCount();
  let row = [];
  for(let i = 0; i < count; i++)
    row.push(zHandle.getCell(i));
  data.push(row);
}

// after we're done parsing, clean up and display the result
function finish() {
  zHandle.finish();
  zHandle.delete();
  alert('Parsed ' + bytes_read + ' bytes; ' + rowcount + ' rows in '
        + (performance.now() - start) + 'ms. '
        + 'You can view the parsed data in your browser dev tools console (right-click and select Inspect)');
  console.log('zsv parsed data', data);
  data = null;
}

// whenever the file input element changes, parse the file the user selected
function fileChanged(e) {
  if(e.target.files && e.target.files.length) {
    // initialize
    reset();
    zHandle = zsvParser.new(rowHandler);

    // stream the file through our parser
    let chunkSize = 1024*1024; // 1 mb
    streamFile(e.target.files[0], chunkSize, function(err, data) {
      if(!err) {
        if(data) {
          bytes_read += data.length;
          zHandle.parseBytes(data);
        } else // all done
          finish();
      } else {
        console.error('Error!', err);
        // cleanup
        finish();
      }
    });
  }
}

// after the page has loaded, add our change listener to the file input element
window.addEventListener('load', function() {
  let file_input = document.body.querySelector('#file_input');
  file_input.addEventListener('change', fileChanged);
})

// Generic function to stream a file through a function
function streamFile(file, chunk_size, cb) {
  if(!(chunk_size >= 1024))
    chunk_size = 1024;
  let offset = 0;
  let fr = new FileReader();

  function getNextChunk() {
    // getNextChunk(): will fire 'onload' event when specified chunk size is loaded
    let slice = file.slice(offset, offset + chunk_size);
    fr.readAsArrayBuffer(slice);
  }

  fr.onload = function() {
    let view = new Uint8Array(fr.result);
    if(!cb(null, view)) {
      offset += chunk_size;

      if(offset < file.size && view.length)
        getNextChunk();
      else // all done
        cb(null, null);
    }
  };

  fr.onerror = function() {
    cb('read error');
  };

  getNextChunk();
}

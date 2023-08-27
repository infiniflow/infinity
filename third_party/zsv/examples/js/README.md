# ZSV in web assembly example

## Overview

Two examples here demonstrate how the zsv CSV parser can be compiled to web assembly and called via javascript
via a static page in a browser or a Node module.

Most of the operative code is in [js/foot.js](js/foot.js) which effectively just converts between Javascript and emscripten.

### Browser
To run the browser demo, run `make run`.
Static files will be built in a subdirectory of the `build` directory, and a python local https server
will be started to serve them on https://127.0.0.1:8888

You can view a [demo of the built example here](https://liquidaty.github.io/zsv/examples/wasm/build/)

### Node module

To build a node module, run `make node`. Module files will be placed in node/node_modules/zsv-parser

### Node example and test
To run a test via node, run `make test`. The node module will be built, a sample program will be copied to
`node/index.js`, which reads CSV from stdin and outputs JSON, and a test will be run

## Prerequisites

To build, you need emscripten. To run the example web server, you need python3. Unlike some of the other examples,
this example does not require that libzsv is already installed

## Quick start

1. From the zsv base directory, run configure in your emscripten environment and save the config output
   to config.emcc:
   ```
   emconfigure ./configure CONFIGFILE=config.emcc
   ```

2. Change back to this directory (examples/wasm), then run `emmake make run`. You should see output messages
   ending with `Listening on https://127.0.0.1:8888`

3. Navigate to https://127.0.0.1:8888. If you get a browser warning, then using Chrome you can type "thisisunsafe" to proceed

4. Click the button to upload a file

## Performance

Running ZSV lib from Javascript is still experimental and is not yet fully optimized.
Some performance challenges are particular to web assembly + Javascript, e.g. where a lot of string data
is being passed between Javascript and the library (see e.g. https://hacks.mozilla.org/2019/08/webassembly-interface-types/).

However, initial results are promising:

* Running only "count", zsv-lib is ~90%+ faster than `csv-parser` and `papaparse`
* The more cell data that is fetched, the more this advantage diminishes due to the aforementioned Javascript/wasm memory overhead.
  Our benchmarking suggests that if the entire row's data is fetched, performance is about on par with both csv-parser and papaparse.
  If only a portion is fetched, performance is about the same for papaparse, and faster than csv-parser (how much faster
  being roughly proportional to the difference between count (~90% faster) and the
  amount of total data fetched)

## All the build commands

Separate commands can be used for build, run and clean:
```
make build
make node
make run
make clean
```

Add MINIFY=1 to any of the above to generate minified code

To run benchmark tests:
```
make benchmark
```

To see all make options:
```
make
```

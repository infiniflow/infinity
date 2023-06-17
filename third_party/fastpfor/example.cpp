//
// A simple example to get you started with the library.
// You can compile and run this example like so:
//
//   make example
//   ./example
//
//  Warning: If your compiler does not fully support C++11, some of
//  this example may require changes.
//

#include "headers/codecfactory.h"
#include "headers/deltautil.h"

int main() {
  using namespace FastPForLib;

  // We pick a CODEC
  IntegerCODEC &codec = *CODECFactory::getFromName("simdfastpfor256");
  // could use others, e.g., "simdbinarypacking", "varintg8iu"
  ////////////
  //
  // create a container with some integers in it
  //
  // for this example, we will not assume that the
  // integers are in sorted order
  //
  // (Note: You don't need to use a vector.)
  //
  size_t N = 10 * 1000;
  std::vector<uint32_t> mydata(N);
  for (uint32_t i = 0; i < N; i += 150)
    mydata[i] = i;
  //
  // the vector mydata could contain anything, really
  //
  ///////////
  //
  // You need some "output" container. You are responsible
  // for allocating enough memory.
  //
  std::vector<uint32_t> compressed_output(N + 1024);
  // N+1024 should be plenty
  //
  //
  size_t compressedsize = compressed_output.size();
  codec.encodeArray(mydata.data(), mydata.size(), compressed_output.data(),
                    compressedsize);
  //
  // if desired, shrink back the array:
  compressed_output.resize(compressedsize);
  compressed_output.shrink_to_fit();
  // display compression rate:
  std::cout << std::setprecision(3);
  std::cout << "You are using "
            << 32.0 * static_cast<double>(compressed_output.size()) /
                   static_cast<double>(mydata.size())
            << " bits per integer. " << std::endl;
  //
  // You are done!... with the compression...
  //
  ///
  // decompressing is also easy:
  //
  std::vector<uint32_t> mydataback(N);
  size_t recoveredsize = mydataback.size();
  //
  codec.decodeArray(compressed_output.data(), compressed_output.size(),
                    mydataback.data(), recoveredsize);
  mydataback.resize(recoveredsize);
  //
  // That's it!
  //
  if (mydataback != mydata)
    throw std::runtime_error("bug!");

  // If you need to use differential coding, you can use
  // calls like these to get the deltas and recover the original
  // data from the deltas:
  Delta::deltaSIMD(mydata.data(), mydata.size());
  Delta::inverseDeltaSIMD(mydata.data(), mydata.size());
  // be mindful of CPU caching issues

  // If you do use differential coding a lot, you might want 
  // to check out these other libraries...
  // https://github.com/lemire/FastDifferentialCoding
  // and
  // https://github.com/lemire/SIMDCompressionAndIntersection
}

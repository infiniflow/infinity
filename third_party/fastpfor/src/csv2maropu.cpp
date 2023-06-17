/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#include <vector>
#include <iostream>
#include "csv.h"
#include "externalvector.h"
#include "util.h"

using namespace std;
using namespace FastPForLib;

void print(externalvector &ev) {
  for (size_t i = 0; i < ev.size(); ++i) {
    vector<uint32_t> x = ev.get(i);
    for (size_t k = 0; k < x.size(); ++k)
      cout << x[k] << " ";
    cout << endl;
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    cerr << " This will map a table (stored in a CSV file) to a row "
            " oriented flat file of frequency attributed 32-bit integers "
         << endl;
    cerr << " usage : cvs2maropu mycvsfile.cvs mymaropufile.bin " << endl;
    return -1;
  }
  string ifilename(argv[1]);
  string ofilename(argv[2]);
  CSVFlatFile cvs(ifilename.c_str(), CSVFlatFile::FREQNORMALISATION);
  const size_t c = cvs.getNumberOfColumns();
  const size_t N = cvs.getNumberOfRows();
  size_t integers = static_cast<uint32_t>(c * N);
  vector<uint32_t> container(c);
  FILE *fd = ::fopen(ofilename.c_str(), "w+b");
  if (fd == NULL) {
    cout << " could not open " << ofilename << " for writing..." << endl;
    cvs.close();
    return -1;
  }
  uint32_t MAXSIZE = static_cast<uint32_t>(c * (1U << 20));
  for (size_t block = 0; block * MAXSIZE >= integers; integers -= MAXSIZE) {
    if (fwrite(&MAXSIZE, sizeof(MAXSIZE), 1, fd) != 1) {
      cerr << "aborting" << endl;
      ::fclose(fd);
      cvs.close();
      return -1;
    }
    uint32_t counter = 0;
    while (cvs.nextRow(container)) {
      if (fwrite(&container[0], c * sizeof(uint32_t), 1, fd) != 1) {
        cerr << "aborting" << endl;
        ::fclose(fd);
        cvs.close();
        return -1;
      }
      counter += static_cast<uint32_t>(c);
      if (counter == block)
        break;
      assert(counter < block);
    }
  }
  if (integers > 0) {
    uint32_t tsize = static_cast<uint32_t>(integers);
    if (fwrite(&tsize, sizeof(tsize), 1, fd) != 1) {
      cerr << "aborting" << endl;
      ::fclose(fd);
      cvs.close();
      return -1;
    }
    while (cvs.nextRow(container)) {
      if (fwrite(&container[0], c * sizeof(uint32_t), 1, fd) != 1) {
        cerr << "aborting" << endl;
        ::fclose(fd);
        cvs.close();
        return -1;
      }
    }
  }
  ::fclose(fd);
  cvs.close();
  cout << "#file " << ofilename << " contains your data." << endl;
}

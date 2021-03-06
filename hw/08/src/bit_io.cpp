#include "bit_io.h"

#include <ios>

namespace ipd {
    using std::ios;

    bistream &bistream::read(bool &bit) {
        if (nbits == 0) {
            if (!next_byte(bitbuf)) return *this;
            nbits = 8;
        }

        --nbits;
        bit = bitbuf >> nbits & 1;

        return *this;
    }

    bistringstream::bistringstream(std::vector<unsigned char> v)
            : bytes(v), bytes_index(0) {}

    bool bistringstream::next_byte(char &bitbuf) {
        if (bytes_index < bytes.size()) {
            bitbuf = bytes[bytes_index];
            bytes_index++;
            return true;
        }
        bitbuf = 0;
        return false;
    }

    bool bistringstream::good() const {
        return nbits > 0 || bytes_index < bytes.size();
    }

    bool bistringstream::eof() const {
        return nbits == 0 && bytes_index == bytes.size();
    }


    bifstream::bifstream(const char *filespec)
            : stream(filespec, ios::binary) {}

    bool bifstream::next_byte(char &bitbuf) {
        return (bool) stream.read(&bitbuf, 1);
    }

    bool bifstream::good() const {
        return nbits > 0 || stream.good();
    }

    bool bifstream::eof() const {
        return nbits == 0 && stream.eof();
    }

    bofstream &bofstream::write(bool bit) {
        bitbuf |= ((char) bit) << (7 - nbits);

        if (++nbits == 8) write_out();

        return *this;
    }

    bostream::operator bool() const {
        return good();
    }

    bofstream::bofstream(const char *filespec)
            : bitbuf(0), nbits(0), stream(filespec, ios::binary | ios::trunc) {}

    bool bofstream::good() const {
        return stream.good();
    }

    void bofstream::write_out() {
        if (stream.write(&bitbuf, 1)) {
            bitbuf = 0;
            nbits = 0;
        }
    }

    bofstream::~bofstream() {
        if (nbits) {
            write_out();
        }
    }

    bool bostringstream::good() const {
        return true;
    }

    bostringstream &bostringstream::write(bool bit) {
        int index = bits_written / 8;
        int nbits = bits_written % 8;
        if (index >= data.size()) data.push_back(0);
        data[index] |= ((char) bit) << (7 - nbits);
        bits_written++;
        return *this;
    }

    std::vector<unsigned char> bostringstream::get_data() {
        return data;
    }
}


#include <iostream>
#include <fstream>
#include <bits/stdc++.h>

using namespace std;

#pragma pack (push, 1)

struct header {
    short packet_length = 0;
    short num_market_updates = 0;

    friend ostream &operator<<(ostream &os, const header &packet) {
        os << "Packet : packet_length: " << packet.packet_length << " num_market_updates: "
           << packet.num_market_updates;
        return os;
    }
} __attribute__ ((aligned (1)));

#pragma pack(pop)

#pragma pack (push, 1)
struct update_type {
    short length = 0;
    char type = 'T';
};
#pragma pack(pop)

#pragma pack (push, 1)

struct trade {
    char symbol[5];
    short trade_size = 0;
    __int64 trade_price = 0;
} __attribute__ ((aligned (1)));

#pragma pack(pop)

#pragma pack (push, 1)

struct quote {
    char symbol[6];
    short price_level = 0;
    short price_level_size = 0;
    __int64 price_level_price = 0;
} __attribute__ ((aligned (1)));

#pragma pack(pop)

bool read_packet(const std::string &filename) {
    auto pFile = fopen(filename.data(), "rb");

    auto outputfile = "C:\\\\Users\\\\A02290691\\\\CLionProjects\\\\GTSX\\\\output.dat";
    if (!pFile) {
        return false;
    }

    std::ofstream ofile(outputfile, std::ofstream::out);
    if (!ofile) throw std::runtime_error("Failed to successfully create the file");

    header header;
    while (true) {
        try {
            auto read_header = fread(&header, sizeof(header), 1, pFile) == 1;
            header.num_market_updates = __builtin_bswap16(header.num_market_updates);
            header.packet_length = __builtin_bswap16(header.packet_length);
            cout << "Header : " << header.packet_length << "\t" << header.num_market_updates << std::endl;

            update_type update;

            int iter = header.num_market_updates;
            trade tt;
            for (size_t i = 0; i < iter; i++) {
                auto update_read = fread(&update, sizeof(update), 1, pFile);
                update.length = __builtin_bswap16(update.length);

                cout << "Update : " << update.length << "\t" << update.type << endl;
                if (update.type == 'T') {
                    auto trade_read = fread(&tt, sizeof(tt), 1, pFile);
                    tt.trade_size = __builtin_bswap16(tt.trade_size);
                    tt.trade_price = __builtin_bswap64(tt.trade_price);

                    char newsymbol[6];
                    for(size_t i = 0; i < 5; i++) newsymbol[i] = tt.symbol[i];
                    newsymbol[5] = '\0';

                    int skip = update.length - sizeof(tt) - 3;
                    char buffer[skip];
                    auto skipped = fread(&buffer, skip, 1, pFile) == 1;
                    ofile << tt.trade_size << ' ' << newsymbol << " @ " << double(tt.trade_price * 1.0/10000) << endl;
                } else {
                    int skip = update.length - 3;
                    char buffer[skip];
                    auto skipped = fread(&buffer, skip, 1, pFile) == 1;
                }
            }
        }
        catch (...) {
            break;
        }
    }

    ofile.close();
    std::cout << "Your text editor saved at " << filename << std::endl;

    return false;
}

int main() {
    std::string fileName("C:\\Users\\A02290691\\CLionProjects\\GTSX\\input.dat");
    read_packet(fileName);
    return 0;
}

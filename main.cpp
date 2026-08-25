#include <iostream>
#include <chrono>
#include <utility>
#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <thread>
#include <syncstream>

// ifstream + getline + stod
// 1121 1171 1569
//void parse_csv_naive(const std::string &f) {
//    struct CityStats {
//        double min = std::numeric_limits<double>::max();
//        double max = std::numeric_limits<double>::lowest();
//    };
//
//    std::ifstream file(f);
//
//    if (!file.is_open()) {
//        std::cerr << "Error: Could not open the file.\n";
//        return;
//    }
//
//    std::unordered_map<std::string, CityStats> results;
//    std::string line;
//
//    // 2. Read the file one line at a time
//    while (std::getline(file, line)) {
//
//        // 3. Find the separator (assuming a comma in this example)
//        size_t delimiter_pos = line.find(',');
//
//        if (delimiter_pos != std::string::npos) {
//
//            // 4. Extract the city name into a new string
//            std::string city = line.substr(0, delimiter_pos);
//
//            // 5. Extract the number string and convert it using the standard library
//            std::string value_str = line.substr(delimiter_pos + 1);
//            double value = std::stod(value_str);
//
//            // 6. Update the map
//            results[city].min = std::min(results[city].min, value);
//            results[city].max = std::max(results[city].max, value);
//        }
//    }
//
//    // 7. Print the results
//    for (const auto& [city, stats] : results) {
//        std::cout << city << " -> Min: " << stats.min << ", Max: " << stats.max << "\n";
//    } std::cout << std::endl;
//}

// load into inmem buf + unordered_map + manual parsing
//void foo1(const std::string &f) {
//    struct stats {
//        double min = std::numeric_limits<double>::max();
//        double max = std::numeric_limits<double>::lowest();
//    };
//
//    std::ifstream file(f, std::ios::binary);
//    if (!file) {
//        std::cerr << "error reading .csv file!" << std::endl;
//        return;
//    }
//
//    file.seekg(0, std::ios::end);
//    std::streamsize fsize = file.tellg();
//    file.seekg(0, std::ios::beg);
//
//    std::unique_ptr<char[]> buf = std::make_unique<char[]>(fsize);
//
//    file.read(buf.get(), fsize);
//
//
//    std::unordered_map<std::string_view, stats> m;
//
//    const char* ptr = buf.get();
//    const char* const end = buf.get() + fsize;
//    while (ptr < end) {
//        const char* start = ptr;
//        while (ptr < end && *ptr != ',') {
//            ++ptr;
//        }
//        std::string_view city(start, ptr - start);
//
//        ++ptr;
//
//        double value = 0.0;
//        bool negative = false;
//
//        if (*ptr == '-') {
//            negative = true;
//            ++ptr;
//        }
//
//        while (ptr < end && *ptr != '.') {
//            value = value * 10.0 + (*ptr - '0');
//            ++ptr;
//        }
//
//        ++ptr;
//
//        double frac = 0.1;
//
//        while (ptr < end && *ptr >= '0' && *ptr <= '9') {
//            value += (*ptr - '0') * frac;
//            frac /= 10.0;
//            ++ptr;
//        }
//
//        if (negative) {
//            value = -value;
//        }
//
//        while (ptr < end && *ptr != '\n') {
//            ++ptr;
//        }
//        ++ptr;
//
//        stats &s = m[city];
//        s.max = std::max(s.max, value);
//        s.min = std::min(s.min, value);
//    }
//
////    for (const auto& [city, stats] : m) {
////        std::cout << city << " -> Min: " << stats.min << ", Max: " << stats.max << "\n";
////    } std::cout << std::endl;
//}

// load into inmem buf + absl flat map + manual parsing
//void foo2(const std::string &f) {
//    struct stats {
//        double min = std::numeric_limits<double>::max();
//        double max = std::numeric_limits<double>::lowest();
//    };
//
//    std::ifstream file(f, std::ios::binary);
//    if (!file) {
//        std::cerr << "error reading .csv file!" << std::endl;
//        return;
//    }
//
//    file.seekg(0, std::ios::end);
//    std::streamsize fsize = file.tellg();
//    file.seekg(0, std::ios::beg);
//
//    std::unique_ptr<char[]> buf = std::make_unique<char[]>(fsize);
//
//    file.read(buf.get(), fsize);
//
//    absl::flat_hash_map<std::string_view, stats> m;
//
//    const char* ptr = buf.get();
//    const char* const end = buf.get() + fsize;
//    while (ptr < end) {
//        const char* start = ptr;
//        while (ptr < end && *ptr != ',') {
//            ++ptr;
//        }
//        std::string_view city(start, ptr-start);
//
//        ++ptr;
//
//        double value = 0.0;
//        bool negative = false;
//
//        if (*ptr == '-') {
//            negative = true;
//            ++ptr;
//        }
//
//        while (ptr < end && *ptr != '.') {
//            value = value * 10.0 + (*ptr - '0');
//            ++ptr;
//        }
//
//        ++ptr;
//
//        double frac = 0.1;
//
//        while (ptr < end && *ptr >= '0' && *ptr <= '9') {
//            value += (*ptr - '0') * frac;
//            frac /= 10.0;
//            ++ptr;
//        }
//
//        if (negative) {
//            value = -value;
//        }
//
//        while (ptr < end && *ptr != '\n') {
//            ++ptr;
//        }
//        ++ptr;
//
//        stats &s = m[city];
//        s.max = std::max(s.max, value);
//        s.min = std::min(s.min, value);
//    }
//
////    for (const auto& [city, stats] : m) {
////        std::cout << city << " -> Min: " << stats.min << ", Max: " << stats.max << "\n";
////    } std::cout << std::endl;
//}

// mmap + absl flat map + manual parsing + advising
//void foo3(const std::string &f) {
//    struct stats {
//        double min = std::numeric_limits<double>::max();
//        double max = std::numeric_limits<double>::lowest();
//    };
//
//    int fd;
//    if ((fd=open(f.c_str(), O_RDONLY)) == -1) {
//        std::cerr << "couldn't open the file!" << std::endl;
//        return;
//    }
//
//    struct stat file_info;
//
//    if (fstat(fd, &file_info) != 0) {
//        std::cerr << "couldn't get stats on the file!" << std::endl;
//        close(fd);
//        return;
//    }
//
//    auto memneed = static_cast<size_t>(file_info.st_size);
//
//    const char* ptr = static_cast<const char*>(mmap(nullptr, memneed, PROT_READ, MAP_SHARED, fd, 0));
//
//    madvise(const_cast<char*>(ptr), memneed, MADV_SEQUENTIAL);
//    madvise(const_cast<char*>(ptr), memneed, MADV_WILLNEED);
//
//    absl::flat_hash_map<std::string_view, stats> m;
//
//    const char* const end = ptr + memneed;
//
//    while (ptr < end) {
//        const char *start = ptr;
//        while (ptr < end && *ptr != ',') {
//            ++ptr;
//        }
//        std::string_view city(start, reinterpret_cast<std::size_t>(ptr) - reinterpret_cast<std::size_t>(start));
//
//        ++ptr;
//
//        double value = 0.0;
//        bool negative = false;
//
//        if (*ptr == '-') {
//            negative = true;
//            ++ptr;
//        }
//
//        while (ptr < end && *ptr != '.') {
//            value = value * 10.0 + (*ptr - '0');
//            ++ptr;
//        }
//
//        ++ptr;
//
//        double frac = 0.1;
//
//        while (ptr < end && *ptr >= '0' && *ptr <= '9') {
//            value += (*ptr - '0') * frac;
//            frac /= 10.0;
//            ++ptr;
//        }
//
//        if (negative) {
//            value = -value;
//        }
//
//        while (ptr < end && *ptr != '\n') {
//            ++ptr;
//        }
//        ++ptr;
//
//        stats &s = m[city];
//        s.max = std::max(s.max, value);
//        s.min = std::min(s.min, value);
//    }
//
//    close(fd);
//
//    munmap(static_cast<void*>(const_cast<char*>(ptr)), memneed);
//
////    for (const auto& [city, stats] : m) {
////        std::cout << city << " -> Min: " << stats.min << ", Max: " << stats.max << "\n";
////    } std::cout << std::endl;
//
//}


// multithreaded + mmap + absl flat map + manual parsing
//void foo4(const std::string &f) {
//    struct stats {
//        double min = std::numeric_limits<double>::max();
//        double max = std::numeric_limits<double>::lowest();
//    };
//
//    int fd;
//    if ((fd=open(f.c_str(), O_RDONLY)) == -1) {
//        std::cerr << "couldn't open the file!" << std::endl;
//        return;
//    }
//
//    struct stat file_info;
//
//    if (fstat(fd, &file_info) != 0) {
//        std::cerr << "couldn't get stats on the file!" << std::endl;
//        close(fd);
//        return;
//    }
//
//    auto memneed = static_cast<size_t>(file_info.st_size);
//
//    auto p = mmap(nullptr, memneed, PROT_READ, MAP_SHARED, fd, 0);
//    if (p == MAP_FAILED) {
//        std::cerr << "unable to memory map!" << std::endl;
//        close(fd);
//        return;
//    }
//
//    const char* ptr = static_cast<const char*>(p);
//    const char* const eof = ptr + memneed;
//
////    madvise(const_cast<char*>(ptr), memneed, MADV_SEQUENTIAL);
////    madvise(const_cast<char*>(ptr), memneed, MADV_WILLNEED);
//
//    auto readThread = [](size_t i, absl::flat_hash_map<std::string_view, stats> &m, const char* ptr, const char* end, const char* const eof)->void {
//        if (i > 0) {
//            while (ptr < eof && *ptr != '\n') ++ptr;
//            if (ptr < eof) ++ptr;
//        }
//
//        while (end < eof && *end != '\n') {
//            ++end;
//        }
//
//        while (ptr < end) {
//            const char *start = ptr;
//            while (ptr < end && *ptr != ',') {
//                ++ptr;
//            }
//            std::string_view city(start, reinterpret_cast<std::size_t>(ptr) - reinterpret_cast<std::size_t>(start));
//
////            {
////                std::lock_guard<std::mutex> lock(cout_mutex);
////                std::cout << city << std::endl;
////            }
//
//            ++ptr;
//
//            double value = 0.0;
//            bool negative = false;
//
//            if (*ptr == '-') {
//                negative = true;
//                ++ptr;
//            }
//
//            while (ptr < end && *ptr != '.') {
//                value = value * 10.0 + (*ptr - '0');
//                ++ptr;
//            }
//
//            ++ptr;
//
//            double frac = 0.1;
//
//            while (ptr < end && *ptr >= '0' && *ptr <= '9') {
//                value += (*ptr - '0') * frac;
//                frac /= 10.0;
//                ++ptr;
//            }
//
//            if (negative) {
//                value = -value;
//            }
//
//            while (ptr < end && *ptr != '\n') {
//                ++ptr;
//            }
//            ++ptr;
//
//            stats &s = m[city];
//            s.max = std::max(s.max, value);
//            s.min = std::min(s.min, value);
//        }
//    };
//
//    const size_t NUM_THREADS = 8;
//
//    absl::flat_hash_map<std::string_view, stats> m;
//
//    std::vector<std::thread> threads; threads.reserve(NUM_THREADS);
//    std::vector<absl::flat_hash_map<std::string_view, stats>> maps; maps.resize(NUM_THREADS);
//    for (size_t i=0; i<NUM_THREADS; ++i) {
//        const char* start = ptr + (memneed/8 * i);
//        const char* end = ptr + (memneed/8 * (i+1));
//        std::thread t(readThread, i, std::ref(maps[i]), start, end, eof);
//        threads.push_back(std::move(t));
//    }
//
//    for (auto &t : threads) {
//        t.join();
//    }
//
//    for (int i=0; i<maps.size(); i++) {
//        for (const auto& [city, temp] : maps[i]) {
//            stats &s = m[city];
//            s.max = std::max(s.max, temp.max);
//            s.min = std::min(s.min, temp.min);
//        }
//    }
//
////    for (const auto& [city, stats] : m) {
////        std::cout << city << " -> Min: " << stats.min << ", Max: " << stats.max << "\n";
////    } std::cout << std::endl;
//
//    close(fd);
//
//    munmap(static_cast<void*>(const_cast<char*>(ptr)), memneed);
//
//}

// multithreaded + mmap + manual hashmap + manual parsing; Terrible!!
//void foo5(const std::string &f) {
//    struct stats {
//        double min = std::numeric_limits<double>::max();
//        double max = std::numeric_limits<double>::lowest();
//    };
//
//    int fd;
//    if ((fd=open(f.c_str(), O_RDONLY)) == -1) {
//        std::cerr << "couldn't open the file!" << std::endl;
//        return;
//    }
//
//    struct stat file_info;
//
//    if (fstat(fd, &file_info) != 0) {
//        std::cerr << "couldn't get stats on the file!" << std::endl;
//        close(fd);
//        return;
//    }
//
//    auto memneed = static_cast<size_t>(file_info.st_size);
//
//    auto p = mmap(nullptr, memneed, PROT_READ, MAP_SHARED, fd, 0);
//    if (p == MAP_FAILED) {
//        std::cerr << "unable to memory map!" << std::endl;
//        close(fd);
//        return;
//    }
//
//    const char* ptr = static_cast<const char*>(p);
//    const char* const eof = ptr + memneed;
//
////    madvise(const_cast<char*>(ptr), memneed, MADV_SEQUENTIAL);
////    madvise(const_cast<char*>(ptr), memneed, MADV_WILLNEED);
//
//    auto readThread = [](size_t i, flatmap<std::string_view, stats> &m, const char* ptr, const char* end, const char* const eof)->void {
//        // 43 distinct cities
//        m.reserve(200);
//
//        if (i > 0) {
//            while (ptr < eof && *ptr != '\n') ++ptr;
//            if (ptr < eof) ++ptr;
//        }
//
//        while (end < eof && *end != '\n') {
//            ++end;
//        }
//
//        while (ptr < end) {
//            const char *start = ptr;
//
//
//            size_t hashVal = 0;
//            while (ptr < end && *ptr != ',') {
//                hashVal = hashVal * 31 + *ptr;
//                ++ptr;
//            }
//
//            std::string_view city(start, reinterpret_cast<std::size_t>(ptr) - reinterpret_cast<std::size_t>(start));
//
////            {
////                std::lock_guard<std::mutex> lock(cout_mutex);
////                std::cout << city << std::endl;
////            }
//
//            ++ptr;
//
//            double value = 0.0;
//            bool negative = false;
//
//            if (*ptr == '-') {
//                negative = true;
//                ++ptr;
//            }
//
//            while (ptr < end && *ptr != '.') {
//                value = value * 10.0 + (*ptr - '0');
//                ++ptr;
//            }
//
//            ++ptr;
//
//            // garaunteed to always have one digit of decimal
//            value += (*ptr - '0') * 0.1;
//
//            ++ptr;
//
//            if (negative) {
//                value = -value;
//            }
//
//            while (ptr < end && *ptr != '\n') {
//                ++ptr;
//            }
//            ++ptr;
//
//            stats &s = m.getOrMake(hashVal, city);
////            stats &s = m[city];
//            s.max = std::max(s.max, value);
//            s.min = std::min(s.min, value);
//        }
//    };
//
//    const size_t NUM_THREADS = 8;
//
//    absl::flat_hash_map<std::string_view, stats> m;
//
//    std::vector<std::thread> threads; threads.reserve(NUM_THREADS);
//    std::vector<flatmap<std::string_view, stats>> maps;
//    maps.resize(NUM_THREADS);
//    for (size_t i=0; i<NUM_THREADS; ++i) {
//        const char* start = ptr + (memneed/8 * i);
//        const char* end = ptr + (memneed/8 * (i+1));
//        std::thread t(readThread, i, std::ref(maps[i]), start, end, eof);
//        threads.push_back(std::move(t));
//    }
//
//    for (auto &t : threads) {
//        t.join();
//    }
//
//    for (int i=0; i<maps.size(); i++) {
//        for (auto j = maps[i].begin(); j != maps[i].end(); j = maps[i].next(j)) {
//            auto [city, temp] = maps[i].getIter(j);
//            stats &s = m[city];
//            s.max = std::max(s.max, temp.max);
//            s.min = std::min(s.min, temp.min);
//        }
//    }
//
//    for (const auto& [city, stats] : m) {
//        std::cout << city << " -> Min: " << stats.min << ", Max: " << stats.max << "\n";
//    } std::cout << std::endl;
//
//    close(fd);
//
//    munmap(static_cast<void*>(const_cast<char*>(ptr)), memneed);
//
//}

// multithreaded + mmap + array + manual parsing
// 13.69s user 16.41s system 180% cpu 16.704 total
void foo6(const std::string &f) {
    std::ios_base::sync_with_stdio(false); std::cin.tie(NULL);
    struct stats {
        int min = 1e5;
        int max = -1e5;
    };

    struct Entry {
        std::string_view city;
        int min = 1e5;
        int max = -1e5;
    };

    // there are 43 cities and found that with n=10, no collisions occur so no checks are necessary
    constexpr size_t fixedArrBits = 10;
    constexpr size_t mask = (1 << fixedArrBits)-1;
    constexpr size_t table_size = (1<<fixedArrBits);

    constexpr char DELIMITER = ';';

    int fd;
    if ((fd=open(f.c_str(), O_RDONLY)) == -1) {
        std::cerr << "couldn't open the file!" << std::endl;
        return;
    }

    struct stat file_info;

    if (fstat(fd, &file_info) != 0) {
        std::cerr << "couldn't get stats on the file!" << std::endl;
        close(fd);
        return;
    }

    auto memneed = static_cast<size_t>(file_info.st_size);

    auto p = mmap(nullptr, memneed, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        std::cerr << "unable to memory map!" << std::endl;
        close(fd);
        return;
    }

    const char* ptr = static_cast<const char*>(p);
    const char* const eof = ptr + memneed;


    auto readThread = [](size_t i, std::array<Entry, table_size> &m, const char* ptr, const char* end, const char* const eof)->void {
        int result = pthread_set_qos_class_self_np(QOS_CLASS_UTILITY, 0);
        if (result != 0) {
            std::cerr << "Failed to set thread QoS class" << std::endl;
        }

        madvise(const_cast<char*>(ptr), end-ptr, MADV_SEQUENTIAL);
        madvise(const_cast<char*>(ptr), end-ptr, MADV_WILLNEED);

        if (i > 0) {
            while (ptr < eof && *ptr != '\n') ++ptr;
            if (ptr < eof) ++ptr;
        }

        while (end < eof && *end != '\n') {
            ++end;
        }

        while (ptr < end) {
            const char *start = ptr;
            size_t hashvalue = 1009;
            while (*ptr != DELIMITER) {
                hashvalue = ((hashvalue << 5) + hashvalue) ^ (*ptr);
                ++ptr;
            }
            hashvalue &= mask;

            const char* const cityEnd = ptr;

//            {
//                std::lock_guard<std::mutex> lock(cout_mutex);
//                std::cout << city << std::endl;
//            }

            ++ptr;

            int value = 0;
            int negative = (*ptr == '-');
            ptr += negative;

            while (*ptr != '.') {
                value = value * 10 + (*ptr - '0');
                ++ptr;
            }

            ++ptr;

            value = value * 10 + (*ptr - '0');

            ++ptr;

            // invert value if negative == 1
            int tmpMask = -negative;
            value = (value ^ tmpMask) - tmpMask;

            Entry &s = m[hashvalue];
            if (s.city.empty()){
                s.city = std::string_view(start, cityEnd-start);
            }
            s.max = std::max(s.max, value);
            s.min = std::min(s.min, value);

            // past \n
            ++ptr;
        }
    };

//    const size_t NUM_THREADS = std::thread::hardware_concurrency();
    constexpr size_t NUM_THREADS = 16;

    std::vector<std::thread> threads; threads.reserve(NUM_THREADS);
    std::vector<std::array<Entry, table_size>> maps(NUM_THREADS);
    for (size_t i=0; i<NUM_THREADS; ++i) {
        const char* start = ptr + (memneed/NUM_THREADS * i);
        const char* end = (i == NUM_THREADS - 1) ? eof : ptr + (memneed/NUM_THREADS * (i+1));
        std::thread t(readThread, i, std::ref(maps[i]), start, end, eof);
        threads.push_back(std::move(t));
    }

    for (auto &t : threads) {
        t.join();
    }

    std::map<std::string_view, stats> m;

    for (size_t i=0; i<maps.size(); ++i) {
        for (size_t j=0; j<maps[i].size(); ++j) {
            if (!maps[i][j].city.empty()) {
                m[maps[i][j].city].min = std::min(m[maps[i][j].city].min, maps[i][j].min);
                m[maps[i][j].city].max = std::max(m[maps[i][j].city].max, maps[i][j].max);
            }
        }
    }

    for (const auto& [city, stats] : m) {
        std::cout << city << " -> Min: " << static_cast<double>(stats.min)/10.0 << ", Max: " << static_cast<double>(stats.max)/10.0 << "\n";
    } std::cout << std::endl;

    close(fd);

    munmap(static_cast<void*>(const_cast<char*>(ptr)), memneed);
}

//static void BM(benchmark::State& state) {
//    for (auto _ : state) {
//        foo6("100mil.csv");
//    }
//}
//
//BENCHMARK(BM)->Iterations(100);
//
//BENCHMARK_MAIN();

int main() {
    foo6("measurements.txt");
    return 0;
}

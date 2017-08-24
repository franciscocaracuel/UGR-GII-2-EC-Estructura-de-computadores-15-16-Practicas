//------------------------------------------------------------------------------
// popcount.cc
// http://www.dalkescientific.com/writings/diary/archive/2011/11/02/faster_popcount_update.html
//------------------------------------------------------------------------------

#include <x86intrin.h> // __rdtscp

#include <algorithm>   // generate
#include <array>       // array
#include <bitset>      // bitset
#include <chrono>      // now
#include <functional>  // bind
#include <iomanip>     // setw
#include <iostream>    // cout endl
#include <random>      // uniform_int_distribution

//------------------------------------------------------------------------------

const unsigned GAP = 12; // gap between columns
const unsigned REP = 50; // how many times we repeat the experiment

std::array<unsigned, 1 << 20 > list;

//------------------------------------------------------------------------------

unsigned popcount00(unsigned elem) {

    return elem;

}

//------------------------------------------------------------------------------

unsigned popcount01(unsigned elem) {

    // Se inicializa el contador
    unsigned count = 0;

    // Se recorren todos los bits del número desplazándolo a la izquierda
    for (unsigned int i = 0; i < 8 * sizeof (decltype(elem)); i++) {

        count += elem & 0x1;
        elem >>= 1;

    }

    // Se devuelve la suma de los 1
    return count;

}

//------------------------------------------------------------------------------

unsigned popcount02(unsigned elem) {

    // Se inicializa el contador
    unsigned count = 0;

    do {

        count += elem & 0x1;
        elem >>= 1;

    } while (elem);

    // Se devuelve la suma de los 1
    return count;

}

//------------------------------------------------------------------------------

unsigned popcount03(unsigned elem) {

    unsigned int count = 0;

    for (unsigned int i = 0; i < 8 * (sizeof (decltype(elem))); i++) {

        asm("shr $1, %[e] \n" // Se desplaza 1 bit a la derecha
                    "adc $0, %[s] \n" // Se suma el acarreo en s
                    : [s] "+r" (count) // Salidas
                    : [e] "r" (elem)); // Entradas

    }

    return count;

}

//------------------------------------------------------------------------------

unsigned popcount04(unsigned elem) {

    unsigned int count = 0;

    // Se recorre elem hasta que el desplazamiento a la derecha lo deja
    // en 0
    while (elem) {

        asm("shr $1, %0 \n"
                    "adc $0, %1 \n"
                    : "+r" (elem), "+r" (count));

    }

    return count;

}

//------------------------------------------------------------------------------

unsigned popcount05(unsigned elem) { // from o'hallaron

    int val = 0;
    int i;

    for (i = 0; i < 8; i++) {
        val += elem & 0x0101010101010101L;
        elem >>= 1;
    }

    val += (val >> 16);
    val += (val >> 8);

    return val & 0xFF;

}

//------------------------------------------------------------------------------
// http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer

unsigned popcount06(unsigned elem) {

    elem = elem - ((elem >> 1) & 0x55555555);
    elem = (elem & 0x33333333) + ((elem >> 2) & 0x33333333);
    return (((elem + (elem >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;

}

//------------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/Hamming_weight

unsigned popcount07(unsigned elem) { // from wikipedia


    // En binario 01010101...
    elem = (elem & 0x5555555555555555) + ((elem >> 1) & 0x5555555555555555); //put count of each  2 bits into those  2 bits 

    // En binario 00110011...
    elem = (elem & 0x3333333333333333) + ((elem >> 2) & 0x3333333333333333); //put count of each  4 bits into those  4 bits 

    // En binario 0000111100001111...
    elem = (elem & 0x0f0f0f0f0f0f0f0f) + ((elem >> 4) & 0x0f0f0f0f0f0f0f0f); //put count of each  8 bits into those  8 bits

    // En binario 0000000011111111...
    elem = (elem & 0x00ff00ff00ff00ff) + ((elem >> 8) & 0x00ff00ff00ff00ff); //put count of each 16 bits into those 16 bits 

    // En binario 00000000000000001111111111111111...
    elem = (elem & 0x0000ffff0000ffff) + ((elem >> 16) & 0x0000ffff0000ffff); //put count of each 32 bits into those 32 bits

    return elem;

}

//------------------------------------------------------------------------------

unsigned popcount08(unsigned elem) { // by Wegner

    int count;

    for (count = 0; elem; count++)
        elem &= elem - 1;

    return count;

}


//------------------------------------------------------------------------------

unsigned popcount14(unsigned elem) {

    return __builtin_popcount(elem);

}

//------------------------------------------------------------------------------

unsigned popcount15(unsigned elem) {

    std::bitset<sizeof (elem) * 8 > bits(elem);

    return bits.count();

}

//------------------------------------------------------------------------------

unsigned popcount16(unsigned elem) {

    if (elem == 0)
        return 0;
    else
        return (elem & 0x1) + popcount16(elem >> 0x1);

}

// https://en.wikipedia.org/wiki/Hamming_weight

unsigned popcount17(unsigned elem) {

    // En binario: 0101...
    elem -= (elem >> 1) & 0x5555555555555555; //put count of each 2 bits into those 2 bits

    // En binario: 00110011...
    elem = (elem & 0x3333333333333333) + ((elem >> 2) & 0x3333333333333333); //put count of each 4 bits into those 4 bits 

    // En binario: 0000111100001111
    elem = (elem + (elem >> 4)) & 0x0f0f0f0f0f0f0f0f; //put count of each 8 bits into those 8 bits 

    elem += elem >> 8; //put count of each 16 bits into their lowest 8 bits

    elem += elem >> 16; //put count of each 32 bits into their lowest 8 bits

    return elem & 0x7f;

}

// https://en.wikipedia.org/wiki/Hamming_weight

unsigned popcount18(unsigned elem) {

    elem -= (elem >> 1) & 0x5555555555555555; //put count of each 2 bits into those 2 bits
    elem = (elem & 0x3333333333333333) + ((elem >> 2) & 0x3333333333333333); //put count of each 4 bits into those 4 bits 
    elem = (elem + (elem >> 4)) & 0x0f0f0f0f0f0f0f0f; //put count of each 8 bits into those 8 bits 
    return (elem * 0x0101010101010101) >> 56; //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ... 

    return elem & 0x7f;

}

unsigned popcount19(unsigned elem) {
    
    int val;

    asm("popcnt %[elem], %[val] \n"
                : [val]"=r"(val)
                : [elem] "r" (elem));
    return val;

}

//------------------------------------------------------------------------------

template <class _F> void test(_F& __f, const char* msg) {

    using namespace std::chrono;

    unsigned id; // needed by __rdtscp()

    unsigned long long cycle_overhead = std::numeric_limits<unsigned long long>::max();

    for (unsigned i = 0; i < REP; ++i) {
        unsigned long long t1 = __rdtscp(&id);
        unsigned long long t2 = __rdtscp(&id);
        cycle_overhead = std::min(cycle_overhead, t2 - t1);
    }

    unsigned long long cycles = std::numeric_limits<unsigned long long>::max();

    for (unsigned i = 0; i < REP; ++i) {
        unsigned long long t1 = __rdtscp(&id);
        __f(list[0]);
        unsigned long long t2 = __rdtscp(&id);
        cycles = std::min(cycles, t2 - t1);
    }
    cycles -= cycle_overhead;

    auto time_overhead = duration<high_resolution_clock::rep, std::nano>::max();

    for (unsigned i = 0; i < REP; ++i) {
        auto t1 = high_resolution_clock::now();
        auto t2 = high_resolution_clock::now();
        time_overhead = std::min(time_overhead, t2 - t1);
    }

    unsigned result;
    auto time = duration<high_resolution_clock::rep, std::nano>::max();

    for (unsigned i = 0; i < REP; ++i) {
        result = 0;
        auto t1 = high_resolution_clock::now();
        std::for_each(list.begin(), list.end(), [&](unsigned x) {
            result += __f(x); });
        auto t2 = high_resolution_clock::now();
        time = std::min(time, t2 - t1);
    }
    time -= time_overhead;

    std::cout << '"' << std::setw(GAP * 2 - 2) << msg << '"'
            << std::setw(GAP) << result
            << std::setw(GAP) << cycles
            << std::setw(GAP) << std::fixed << std::setprecision(2)
            << duration_cast<nanoseconds>(time).count() / static_cast<double> (list.size())
            << std::setw(GAP) << duration_cast<nanoseconds>(time).count() / 1e6
            << std::endl;

}

//------------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<unsigned> distribution;
    auto generator = std::bind(distribution, engine);
    std::generate(list.begin(), list.end(), generator);

    std::cout << "#" << std::setw(GAP * 2 - 1) << "popcount"
            << std::setw(GAP) << "value"
            << std::setw(GAP) << "cycles"
            << std::setw(GAP) << "time(ns)"
            << std::setw(GAP) << "total(ms)"
            << std::endl;

    test(popcount00, "popcount00(empty)");
    test(popcount01, "popcount01(for)");
    test(popcount02, "popcount02(while)");
    test(popcount03, "popcount03(inline)");
    test(popcount04, "popcount04(inline v2)");
    test(popcount05, "popcount05(0'hallaron)");
    test(popcount06, "popcount06(stacko.)");
    test(popcount07, "popcount07(wikipedia)");
    test(popcount08, "popcount08(Wegner)");
    //test(popcount09, "popcount09(lt8)");
    //test(popcount10, "popcount10(lt8 v2)");
    //test(popcount11, "popcount11(lt8 v3)");
    //test(popcount12, "popcount12(lt8 v4)");
    //test(popcount13, "popcount13(lt16)");
    test(popcount14, "popcount14(builtin)");
    test(popcount15, "popcount15(bitset)");
    test(popcount16, "popcount16(recursive)");
    test(popcount17, "popcount17(wikipedia v2)");
    test(popcount18, "popcount18(wikipedia v3)");
    test(popcount19, "popcount19(SSE 4.2)");

}

//------------------------------------------------------------------------------

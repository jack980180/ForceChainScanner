#include <iostream>
#include <chrono>
#include <random>
#include <fstream>
#include "../src/DisjointSet.h"

using namespace std;
using namespace std::chrono;

int main() {

    ofstream out("dsu_unite_results.csv");
    out << "n_elements,num_unions,total_time_seconds,time_per_union\n";

    // Start at 100 elements
    int n = 100;

    // Fixed random generator
    mt19937 rng(42);

    for (int iter = 0; iter < 20; ++iter) {

        cout << "Benchmarking n = " << n << endl;

        DisjointSet dsu(n);

        // Perform 5*n union operations
        int num_unions = 5 * n;

        uniform_int_distribution<int> dist(0, n - 1);

        auto start = high_resolution_clock::now();

        for (int i = 0; i < num_unions; ++i) {
            int a = dist(rng);
            int b = dist(rng);
            dsu.unite(a, b);
        }

        auto end = high_resolution_clock::now();

        double elapsed =
            duration_cast<duration<double>>(end - start).count();

        double time_per_union = elapsed / num_unions;

        out << n << ","
            << num_unions << ","
            << elapsed << ","
            << time_per_union << "\n";

        cout << "Total time: " << elapsed << " s\n";
        cout << "Time per union: " << time_per_union << " s\n";
        cout << "------------------------------------\n";

        n *= 2;  // exponential scaling
    }

    out.close();

    cout << "Benchmark complete. Results written to dsu_unite_results.csv\n";

    return 0;
}
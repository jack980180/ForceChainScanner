#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <fstream>
#include <sstream>
#include "../src/SimulationDriver.h"

// Tiny helper to print headers
void print_test_header(const char* name) {
    printf("\n================ %s ================\n", name);
}

// Helper to create a specific deterministic particle configuration
void create_dummy_csv(const std::string& filename, bool create_jammed_state) {
    std::ofstream file(filename);
    file << "id,x,y,radius\n";

    if (create_jammed_state) {
        // Create a perfect vertical chain of particles (Box H=10, R=1.0)
        // They are spaced 1.5 units apart, so they will overlap/touch if threshold > 1.5
        file << "0,5.0,0.5,1.0\n";
        file << "1,5.0,2.0,1.0\n";
        file << "2,5.0,3.5,1.0\n";
        file << "3,5.0,5.0,1.0\n";
        file << "4,5.0,6.5,1.0\n";
        file << "5,5.0,8.0,1.0\n";
        file << "6,5.0,9.5,1.0\n";
    } else {
        // Create a dilute system with only two isolated particles
        file << "0,5.0,1.0,1.0\n";
        file << "1,5.0,9.0,1.0\n";
    }
    file.close();
}

// Helper to check the output CSV to see if the system percolated (is_spanning == 1)
bool check_jam_status_in_csv(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip header

    bool is_jammed = false;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;

        // Loop through commas to get to the final column (is_spanning)
        while (std::getline(ss, token, ',')) {}

        if (std::stoi(token) == 1) {
            is_jammed = true;
        }
    }
    file.close();
    return is_jammed;
}

// ============================================================
// SIMULATION DRIVER TESTS
// ============================================================
void test_simulation_driver() {
    print_test_header("TESTING SIMULATION DRIVER");

    // Test 1: Initialization & Random Generation
    // W=10, H=10, R=1.0, phi=0.5
    SimulationDriver driver_init(10.0, 10.0, 1.0, 0.5);
    driver_init.initialize_and_relax(5, 0.1);
    // We can't easily assert private variables, but surviving this without a segfault
    // verifies memory allocation and the relaxation loop logic.
    printf("[PASS] Initialization and Relaxation executed successfully\n");

    // Test 2: File I/O Roundtrip
    std::string io_file = "test_io.csv";
    driver_init.save_particles_to_csv(io_file);

    SimulationDriver driver_io(10.0, 10.0, 1.0, 0.1);
    bool loaded = driver_io.load_particles_from_csv(io_file);
    assert(loaded == true);
    printf("[PASS] Particle CSV Save and Load logic\n");

    // Test 3: Connectivity Scan - Unjammed State
    std::string unjammed_in = "test_unjammed_in.csv";
    std::string unjammed_out = "test_unjammed_out.csv";
    create_dummy_csv(unjammed_in, false);

    SimulationDriver driver_unjammed(10.0, 10.0, 1.0, 0.0);
    driver_unjammed.load_particles_from_csv(unjammed_in);
    driver_unjammed.perform_connectivity_scan(1.0);
    driver_unjammed.save_particles_to_csv(unjammed_out);

    assert(check_jam_status_in_csv(unjammed_out) == false);
    printf("[PASS] Connectivity Scan correctly identifies UNJAMMED state\n");

    // Test 4: Connectivity Scan - Jammed State
    std::string jammed_in = "test_jammed_in.csv";
    std::string jammed_out = "test_jammed_out.csv";
    create_dummy_csv(jammed_in, true);

    SimulationDriver driver_jammed(10.0, 10.0, 1.0, 0.0);
    driver_jammed.load_particles_from_csv(jammed_in);
    driver_jammed.perform_connectivity_scan(1.0); // 1.0 force factor
    driver_jammed.save_particles_to_csv(jammed_out);

    assert(check_jam_status_in_csv(jammed_out) == true);
    printf("[PASS] Connectivity Scan correctly identifies JAMMED state\n");

    // Cleanup temporary test files
    std::remove(io_file.c_str());
    std::remove(unjammed_in.c_str());
    std::remove(unjammed_out.c_str());
    std::remove(jammed_in.c_str());
    std::remove(jammed_out.c_str());
    printf("[PASS] Temporary test files cleaned up\n");

    printf("Simulation Driver Tests Complete!\n");
}

int main() {
    test_simulation_driver();

    printf("\n\nAll Simulation Driver Tests Passed! Your pipeline is solid.\n");
    return 0;
}
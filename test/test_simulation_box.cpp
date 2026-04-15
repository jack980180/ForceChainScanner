#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include "../src/SimulationBox.h"
#include "../src/Particle.h"

// Tiny helper to print headers
void print_test_header(const char* name) {
    printf("\n================ %s ================\n", name);
}

// ============================================================
// SIMULATION BOX TESTS
// ============================================================
void test_simulation_box() {
    print_test_header("TESTING SIMULATION BOX");

    // Initialize a 10x10 box with 100 particles
    double W = 10.0;
    double H = 10.0;
    int N = 100;
    SimulationBox box(W, H, N);

    // Test 1: Boundary IDs
    assert(box.get_top_id() == N);
    assert(box.get_bottom_id() == N + 1);
    printf("[PASS] Boundary IDs reserved correctly at end of DSU\n");

    // Test 2: Periodic Boundary dx (X-axis wrap-around)
    // If width is 10, the maximum distance between any two X points is 5.
    
    // Normal case: points close together
    double dx_normal = box.get_periodic_dx(6.0, 4.0);
    assert(std::abs(dx_normal - 2.0) < 1e-6);

    // Wrap-around case 1: Going right to left is shorter
    double dx_wrap1 = box.get_periodic_dx(9.0, 1.0); 
    // 9 - 1 = 8. Since 8 > 5, it wraps to 8 - 10 = -2.
    assert(std::abs(dx_wrap1 - (-2.0)) < 1e-6);

    // Wrap-around case 2: Going left to right is shorter
    double dx_wrap2 = box.get_periodic_dx(1.0, 9.0);
    // 1 - 9 = -8. Since -8 < -5, it wraps to -8 + 10 = 2.
    assert(std::abs(dx_wrap2 - 2.0) < 1e-6);
    printf("[PASS] Periodic boundary X-axis calculations (wrap-around)\n");

    // Test 3: Wall Touching Logic (Y-axis hard boundaries)
    // Assuming Particle constructor is: Particle(id, x, y, radius)
    Particle p_top(0, 5.0, 9.5, 1.0);    // y + r = 10.5 (Touches top)
    Particle p_bottom(1, 5.0, 0.5, 1.0); // y - r = -0.5 (Touches bottom)
    Particle p_mid(2, 5.0, 5.0, 1.0);    // y +/- r = 4.0 to 6.0 (Touches neither)

    assert(box.is_touching_top(p_top) == true);
    assert(box.is_touching_bottom(p_top) == false);

    assert(box.is_touching_bottom(p_bottom) == true);
    assert(box.is_touching_top(p_bottom) == false);

    assert(box.is_touching_top(p_mid) == false);
    assert(box.is_touching_bottom(p_mid) == false);
    printf("[PASS] Hard wall touching detection (Top/Bottom)\n");

    // Test 4: Distance Calculation (Combining periodic X and normal Y)
    Particle p1(0, 9.0, 5.0, 1.0);
    Particle p2(1, 1.0, 5.0, 1.0);
    // Periodic dx is 2.0. dy is 0.0. Distance should be 2.0, not 8.0.
    double dist = box.get_distance(p1, p2);
    assert(std::abs(dist - 2.0) < 1e-6);
    printf("[PASS] Periodic distance calculation\n");

    // Test 5: Particle Relaxation (Overlap Resolution)
    std::vector<Particle> particles;
    // Put two particles dangerously close to each other (overlapping)
    particles.push_back(Particle(0, 5.0, 5.0, 1.0)); // P0 at (5,5)
    particles.push_back(Particle(1, 5.5, 5.0, 1.0)); // P1 at (5.5, 5)
    
    // Initial distance is 0.5, but sum of radii is 2.0. They overlap by 1.5!
    double initial_dist = box.get_distance(particles[0], particles[1]);
    assert(initial_dist < 2.0);

    // Relax them (50 iterations, step size 0.1)
    box.relax_particles(particles, 50, 0.1);

    // They should have pushed each other apart
    double final_dist = box.get_distance(particles[0], particles[1]);
    assert(final_dist > initial_dist); 
    printf("[PASS] Particle relaxation successfully pushes overlapping particles apart\n");

    printf("Simulation Box Tests Complete!\n");
}

int main() {
    test_simulation_box();

    printf("\n\nAll Simulation Box Tests Passed! Your geometry and boundaries are working.\n");
    return 0;
}
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include "bindings.h"

int main(int argc, char **argv)
{
    int runs = 1;
    bool log_mana = false;
    Simulation *sim = new Simulation();

    if (argc > 1)
        runs = atoi(argv[1]);
    if (argc > 2)
        sim->settings->duration = atoi(argv[2]);

    auto start = std::chrono::high_resolution_clock::now();

    if (runs < 2) {
        sim->logging = true;
        SimulationResult result = sim->run();

        for (auto itr = sim->log.begin(); itr != sim->log.end(); itr++) {
            if ((*itr)->type == LOG_MANA && !log_mana)
                continue;
            printf("%.2f %s\n", (*itr)->t, (*itr)->text.c_str());
        }

        printf("Damage: %d\n", result.dmg);
        printf("DPS: %.2f\n", result.dps);
    }
    else {
        double min_dps = 0, avg_dps = 0, max_dps = 0;
        sim->logging = false;

        for (int i=0; i<runs; i++) {
            SimulationResult result = sim->run();

            if (i == 0 || result.dps < min_dps)
                min_dps = result.dps;
            if (result.dps > max_dps)
                max_dps = result.dps;
            avg_dps+= ((result.dps - avg_dps) / (i+1));
        }

        printf("Sims: %d\n", runs);
        printf("DPS: %.2f (%.2f - %.2f)\n", avg_dps, min_dps, max_dps);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    printf("Exec time: %ldms\n", duration.count());

    return 0;
}
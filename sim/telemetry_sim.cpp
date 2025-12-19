#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

using namespace std;

double clamp(double low, double high, double x) {
    if (x < low) {
        return low;
    }
    if (x > high) {
        return high;
    }
    return x;
}

int main() {
    int totalSeconds = 1200;
    string outPath = "data/telemetry_data.csv";
    ofstream out(outPath);

    if (!out) {
        cout << "Failed to open: " << outPath << endl;
        return 1;
    }

    mt19937 rng(67);
    normal_distribution<double> voltageNoiseND(0, 0.01);
    normal_distribution<double> tempNoiseND(0, 0.01);

    double panel_v = 28.0;
    double battery_pct = 100.0;
    double temp_c = 20.0;

    double nominalLoad = 0.02;
    double degradedLoad = 0.01;
    double safeLoad = 0.001;

    double load = nominalLoad;

    int heartbeat = 1;

    bool heartbeat_fail = true;
    int heartbeat_fail_time_s = 450;

    bool power_loss = true;
    int fault_start_time_s = 300;
    double v_loss = 0.02;
    double v_min = 20;

    out << "time_s,panel_v,battery_pct,temp_c,heartbeat_ok\n";
    out << fixed << setprecision(3);

    for (int t = 0; t < totalSeconds; t++) {
        double voltageNoise = voltageNoiseND(rng);
        panel_v += voltageNoise;
        if (power_loss && t >= fault_start_time_s) {
            panel_v -= v_loss;
            panel_v = clamp(v_min, 30, panel_v);
        }
        else {
            if (panel_v > 28) {
            panel_v -= 0.01;
            }
            else if (panel_v < 28) {
                panel_v += 0.01;
            }
            panel_v = clamp(26, 30, panel_v);
        }
        if (power_loss && t >= fault_start_time_s) {
            battery_pct = clamp(0, 100, battery_pct - load * 6);
        }
        else {
            battery_pct = clamp(0, 100, battery_pct - load * 2);
        }

        double tempNoise = tempNoiseND(rng);
        double targetTemp_c = 20.0 + 2.0 * (load / nominalLoad);
        double alpha = 0.01;
        temp_c += alpha * (targetTemp_c - temp_c) + tempNoise;
        temp_c = clamp(10, 30, temp_c);

        if (heartbeat_fail && t >= heartbeat_fail_time_s) {
            heartbeat = 0;
        }

        out << t << "," << panel_v << "," << battery_pct << "," << temp_c << "," << heartbeat << "\n"; 
    }

    out.flush();
    out.close();

    return 0;
}
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

struct Row {
    std::string sale_date;
    std::string country;
    std::string region;
    std::string manufacturer;
    double sale_price_usd = 0.0;
};

//extracts the year
int extractYear(const std::string& date) {
    return std::stoi(date.substr(0, 4));
}

Row parseRow(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    int col = 0;

    Row row;

    while (std::getline(ss, token, ',')) {
        switch (col) {
            case 1: row.sale_date = token; break;
            case 2: row.country = token; break;
            case 3: row.region = token; break;
            case 8: row.manufacturer = token; break;
            case 20: row.sale_price_usd = std::stod(token); break;
            default: break;
        }
        col++;
    }
    return row;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./car_sales_analysis <sales.csv>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    std::string line;
    std::getline(file, line);

    size_t audiChinaCount2025 = 0;
    double bmwTotalRevenue2025 = 0.0;
    std::unordered_map<std::string, double> bmwEuropeRevenue;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        Row row = parseRow(line);

        if (extractYear(row.sale_date) != 2025)
            continue;

        // 1. Audi sales in China (2025)
        if (row.manufacturer == "Audi" && row.country == "China") {
            audiChinaCount2025++;
        }

        // 2. BMW total revenue (2025)
        if (row.manufacturer == "BMW") {
            bmwTotalRevenue2025 += row.sale_price_usd;

            // 3. BMW revenue in Europe (by country)
            if (row.region == "Europe") {
                bmwEuropeRevenue[row.country] += row.sale_price_usd;
            }
        }
    }

    // Sort BMW European revenue
    std::vector<std::pair<std::string, double>> sortedRevenue(
        bmwEuropeRevenue.begin(), bmwEuropeRevenue.end()
    );

    std::sort(sortedRevenue.begin(), sortedRevenue.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });

    std::cout << "\n===== RESULTS =====\n\n";

    std::cout << "1. Audi cars sold in China (2025): "
              << audiChinaCount2025 << "\n\n";

    std::cout << "2. Total BMW revenue (2025): $"
              << bmwTotalRevenue2025 << "\n\n";

    std::cout << "3. BMW Revenue by European Country (2025):\n";
    for (const auto& [country, revenue] : sortedRevenue) {
        std::cout << "   " << country << ": $" << revenue << "\n";
    }

    return 0;
}

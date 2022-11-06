using namespace std;

#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>

int main(int argc, char const *argv[]) {
  if (argc < 4) {
    std::exit(1);
  }

  const int num_stations = stoi(argv[1]);
  int num_trains = stoi(argv[2]);
  int num_ticks = stoi(argv[3]);

  std::cout << num_stations << std::endl;

  // stations
  std::stringstream ss;
  for (int i = 0; i < num_stations; i++) {
    ss << 'a' << i;
    if (i != num_stations - 1) {
      ss << ' ';
    }
  }
  std::cout << ss.str() << std::endl;

  // weight of each station
  srand (0);
  std::stringstream ss2;
  for (int i = 0; i < num_stations; i++) {
    int temp = rand() % 9 + 1;
    ss2 << temp;
    if (i != num_stations - 1) {
      ss2 << ' ';
    }
  }
  std::cout << ss2.str() << std::endl;

  struct IncGenerator {
    int current_;
    IncGenerator (int start) : current_(start) {}
    int operator() () { return current_++; }
  };

  std::vector<int> g(num_stations / 2);
  IncGenerator gen1 (0);
  std::generate(g.begin(), g.end(), gen1);
  std::random_shuffle(g.begin(), g.end());

  std::vector<int> y(num_stations / 2);
  IncGenerator gen2 (num_stations / 3);
  std::generate(y.begin(), y.end(), gen2);
  std::random_shuffle(y.begin(), y.end());

  std::vector<int> b(num_stations / 2);
  IncGenerator gen3 (num_stations / 2);
  std::generate(b.begin(), b.end(), gen3);
  std::random_shuffle(b.begin(), b.end());

  // weight of each link
  vector<vector<int>> link(num_stations , vector<int> (num_stations, 0));
  for (std::vector<int>::size_type i = 0; i < g.size() - 1; i++) {
    link[g[i]][g[i + 1]] = rand() % 9 + 1;
    link[g[i + 1]][g[i]] = link[g[i]][g[i + 1]];
  }

  for (std::vector<int>::size_type i = 0; i < y.size() - 1; i++) {
    link[y[i]][y[i + 1]] = rand() % 9 + 1;
    link[y[i + 1]][y[i]] = link[y[i]][y[i + 1]];
  }

  for (std::vector<int>::size_type i = 0; i < b.size() - 1; i++) {
    link[b[i]][b[i + 1]] = rand() % 9 + 1;
    link[b[i + 1]][b[i]] = link[b[i]][b[i + 1]];
  }

  for (int i = 0; i < num_stations; i++) {
    std::stringstream ss3;
    for (int j = 0; j < num_stations; j++) {
      ss3 << link[i][j];
      if (j != num_stations - 1) {
        ss3 << ' ';
      }
    }
    std::cout << ss3.str() << std::endl;
  }

  // stations in each line
  std::stringstream ss4;
  for (int i = 0; i < num_stations / 2; i++) {
    ss4 << 'a' << g[i];
    if (i != num_stations / 2 - 1) {
      ss4 << ' ';
    }
  }
  std::cout << ss4.str() << std::endl;

  std::stringstream ss5;
  for (int i = 0; i < num_stations / 2; i++) {
    ss5 << 'a' << y[i];
    if (i != num_stations / 2 - 1) {
      ss5 << ' ';
    }
  }
  std::cout << ss5.str() << std::endl;

  std::stringstream ss6;
  for (int i = 0; i < num_stations / 2; i++) {
    ss6 << 'a' << b[i];
    if (i != num_stations / 2 - 1) {
      ss6 << ' ';
    }
  }
  std::cout << ss6.str() << std::endl;

  std::cout << num_ticks << std::endl;
  std::cout << num_trains << ' ' << num_trains << ' ' << num_trains << std::endl;
  int num_lines_to_be_printed = 16;
  std::cout << num_lines_to_be_printed << std::endl;
}
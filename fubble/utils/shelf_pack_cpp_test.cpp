#include <gtest/gtest.h>
#include <iostream>
#include <rectanglebinpack/GuillotineBinPack.h>

TEST(RectangleBinPack, GuillotineBinPack) {
  rbp::GuillotineBinPack test{200, 100};
  bool merge{true};
  const auto choice = rbp::GuillotineBinPack::RectBestAreaFit;
  const auto split = rbp::GuillotineBinPack::SplitMaximizeArea;
  for (int counter{}; counter < 4; ++counter) {
    auto result = test.Insert(100, 100, merge, choice, split);
    if (result.width == 0 || result.height == 0)
      std::cout << "could not insert" << std::endl;
    else
      std::cout << "inserted" << std::endl;
  }
  std::cout << "Occupancy():" << test.Occupancy() << std::endl;
}

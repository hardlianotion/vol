#include <iostream>

#include "market/market.h"
#include "sim/simulation.h"


int main (int argc, char* argv[]) {

  std::cout << "This is a start for a vol demo." << std::endl;

  /**
  * setup is an arithmetic asian option.
  */
  using namespace vol::market;
  using namespace vol::proc;
  using namespace vol;

  double T = 1.;
  double dt = 1. / 365.;
  double strike = 100.;
  double fut = 100.;
  double rate = 0.01;
  double vol = 0.2;
  
  //set up arthmetic and geometric asian process
  auto tmp = norm(rate, vol);
  auto asianNorm = asian::asianing(tmp, 0., T, dt);
  auto geoAsian = [asianNorm] (double t) {return exp (asianNorm(t));};
  auto asian = asian::asianing(lognorm(rate, vol), 0., T, dt);

  for (double t = 0.; t < 2.; t += 0.4) {
    std::cout << asian(t) << ", ";
  }
  std::cout << " asian " << std::endl;

  for (double t = 0.; t < 2.; t += 0.4) {
    std::cout << geoAsian(t) << ", "; 
  }
  std::cout << " geoAsian " << std::endl;

  //calculate the geometric asiam price
  double geoPrice = asian::geomAsian(option::CALL, rate, fut, T, vol, strike, dt);
  
  //create payoffs with price processes
  auto call = vanilla::payoff(option::CALL, strike);
  auto asianCall = [call, asian](double t) {return call(asian(t));};
  auto geoAsianCall = [call, geoAsian](double t) {return call(geoAsian(t));};
  
  std::vector<std::pair<double, double>> paired_sample = ranges::views::generate_n(
    [T, asianCall, geoAsianCall]() {
      return std::make_pair(asianCall(T), geoAsianCall(T)); }, 10000);
  
  auto summary = vol::stats::summary(
      paired_sample.begin(), 
      paired_sample.end(), 
      geoPrice, 10000);

  std::cout << " count = " << std::get<0>(summary)
             << ", mean = " << std::get<1>(summary)
             << ", variance = "<< std::get<2>(summary) << std::endl; 
  return 0;
}


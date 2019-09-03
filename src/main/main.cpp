#include <iostream>

#include <range/v3/range/conversion.hpp>

#include "market/market.h"
#include "sim/simulation.h"
#include "utility/interval.h"


int main (int argc, char* argv[]) {

  std::cout << "This is a start for a vol demo." << std::endl;

  /**
  * setup is an arithmetic asian option.
  */
  using namespace vol::market;
  using namespace vol::proc;
  using namespace vol;

  double T = 1.;
  double dt = 1. / 5.;
  double strike = 100.;
  double fut = 100.;
  double rate = 0.01;
  double vol = 0.2;
  
  typedef vol::utility::interval<double> interval_d;

  interval_d tm(utility::iterator<double>(1., 0.1), utility::iterator<double>(2., 0.1));
  const interval_d::iterator b = tm.begin();
  const interval_d::iterator e = tm.end();
  //const interval_d::iterator e = tm.end();

  bool result = b < e;

  //set up arthmetic and geometric asian process
  auto tmp = norm(rate, vol);
  auto asianNorm = asian::asianing(tmp, 0., T, dt);
  auto geoAsian = [fut, asianNorm] (double t) mutable {return fut * exp (asianNorm(t));};
  auto asian = asian::asianing(lognorm(fut, rate, vol), 0., T, dt);

  //calculate the geometric asiam price
  double geoPrice = asian::geomAsian(option::CALL, rate, fut, T, vol, strike, dt);
  
  //create payoffs with price processes
  auto call = vanilla::payoff(option::CALL, strike);
  auto asianCall = [call, asian](double t) mutable {double result = call(asian(t));std::cout<<"asc " << result<< std::endl;  return result;};
  auto geoAsianCall = [call, geoAsian](double t) mutable {double result = call(geoAsian(t)); std::cout<<"gsc " << result<< std::endl;  return result;};
  
  auto paired_sample = ranges::to<std::vector>(
    ranges::views::generate_n(
    [T, asianCall, geoAsianCall]() mutable -> std::tuple<double, double> {
      return std::make_tuple(asianCall(T), geoAsianCall(T)); }, 10));
  
  auto summary = vol::stats::summary(
      paired_sample.begin(), 
      paired_sample.end(), 
      geoPrice, 10000);

  std::cout << " count = " << std::get<0>(summary)
             << ", mean = " << std::get<1>(summary)
             << ", variance = "<< std::get<2>(summary) << std::endl; 
  return 0;
}


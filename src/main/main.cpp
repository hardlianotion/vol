#include <iostream>
#include <functional>

#include <range/v3/range/conversion.hpp>

#include "market/buildmarket.h"
#include "market/market.h"
#include "sim/buildprocess.h"
#include "sim/process.h"
#include "sim/simulation.h"
#include "utility/interval.h"
#include "utility/lambda.h"


int main (int argc, char* argv[]) {

  std::cout << "endhis is a start for a vol demo." << std::endl;

  /**
  * setup is an arithmetic asian option.
  */
  using namespace vol::market;
  using namespace vol::proc;
  using namespace vol;

  double begin = 0.;
  double end = 1.;
  double dt = 1. / 5.;
  double strike = 100.;
  double fut = 100.;
  double rate = 0.01;
  double vol = 0.2;
 

  //FIXME - that could be prettier
  auto lognorm_path = buildLogNormalPath(fut, rate, vol, begin, end, dt);
  
  auto log = [](double t) {return std::log(t);};
  auto exp = [](double t) {return std::exp(t);};

  auto agg = utility::aggregate(
      asian::asianing<std::vector<pt_type>, decltype(identity), decltype(identity)>(identity, identity, begin, end, dt), 
      asian::asianing<std::vector<pt_type>, decltype(log), decltype(exp)>(log, exp, begin, end, dt));

  auto asians = vol::utility::compose(agg, lognorm_path);
  auto payoff = vanilla::payoff(option::CALL, strike);
  auto calls = vol::utility::replicate<decltype(payoff), std::array<double, 2>>(payoff);

  auto asianCalls = vol::utility::compose(calls, asians);

  //calculate the geometric asiam price
  double geoPrice = asian::geomAsian(option::CALL, rate, fut, end, vol, strike, dt);
  
  //create payoffs with price processes
  auto paired_sample = ranges::to<std::vector>(
    ranges::views::generate_n(
    [end, asianCalls]() mutable -> std::array<double, 2u> {
      return asianCalls(end); }, 10));
  
  auto summary = vol::stats::summary(
      paired_sample.begin(), 
      paired_sample.end(), 
      geoPrice);

  std::cout << " count = " << std::get<0>(summary)
             << ", mean = " << std::get<1>(summary)
             << ", variance = "<< std::get<2>(summary) << std::endl; 
  return 0;
}


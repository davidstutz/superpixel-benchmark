/*
 * Benchmark.h
 *
 *  Created on: 04.02.2010
 *  Changed on: 22.09.2010
 *      Author: david
 */
#ifndef DANVIL_TOOLS_BENCHMARK_H_
#define DANVIL_TOOLS_BENCHMARK_H_
//---------------------------------------------------------------------------
#include <Danvil/Tools/Timer.h>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------

class Benchmark
{
public:
	static constexpr double cDecayMeanFactor = 0.05;

	struct Data {
		Data()
		: //useDetail(false),
		  n(0),
		  sum(0.0f),
		  last(0.0f),
		  decay_mean(0.0f)
		{}

//		bool useDetail;

		size_t n;

		double sum;

		double last;

		double decay_mean;

//		std::vector<double> detailed;

		void add(double t, size_t w=1) {
//			if(useDetail) {
//				detailed.push_back(t);
//				throw 0; // FIXME implement
//			} else {
				n += w;
				sum += t;
//			}
			last = t;
			if(n==1) decay_mean = t;
			decay_mean = cDecayMeanFactor*t + (1.0 - cDecayMeanFactor)*decay_mean;
		}

		uint count() const {
//			if(useDetail) {
//				return detailed.size();
//			} else {
				return n;
//			}
		}

		double total() const
		{
//			if(useDetail) {
//				return Danvil::memops::sum(detailed.begin().base(), detailed.size());
//			} else {
				return sum;
//			}
		}

		double mean() const
		{
//			if(useDetail) {
//				return Danvil::memops::mean(detailed.begin().base(), detailed.size());
//			} else {
				return sum / (double)n;
//			}
		}

		double sigma() const
		{
//			if(useDetail) {
//				return Danvil::memops::rootMeanSquareDeviation<double,double>(detailed.begin().base(), detailed.size(), mean());
//			} else {
				return 0; // undefined
//			}
		}
	};

	typedef boost::function<void(const std::string& name, const Data& data)> FunctorOnUpdate;

	Benchmark()
	: tag_name_max_length_(0)
	{}

	void setOnUpdate(FunctorOnUpdate f) {
		on_update_ = f;
	}

	void add(const std::string& tag, double time, size_t weight=1) {
		boost::mutex::scoped_lock lock(mutex_);
		tag_data_[tag].add(time, weight);
		tag_name_max_length_ = std::max<unsigned int>(tag_name_max_length_, tag.length());
		if(on_update_) {
			on_update_(tag, tag_data_[tag]);
		}
	}

	void print(std::ostream& os, const std::string& tag) {
		boost::mutex::scoped_lock lock(mutex_);
		os << std::left << std::setw(tag_name_max_length_ + 2) << ("'"+tag+"'") << std::right;
		Data& a = tag_data_[tag];
		os 		<< std::setw(12) << a.count()
				<< std::setw(12) << std::fixed << std::setprecision(2) << a.total()
				<< std::setw(12) << std::fixed << std::setprecision(2) << a.mean()
				<< std::setw(12) << std::fixed << std::setprecision(2) << a.last;
//		if(a.useDetail) {
//			os << ", sigma=" << a.sigma();
//		}
		os << std::setw(0) << std::endl;
	}

	void printAll(std::ostream& os) {
		os << "BENCHMARK RESULTS" << std::endl;
		os		<< std::left
				<< std::setw(tag_name_max_length_ + 2) << "TAG"
				<< std::right
				<< std::setw(12) << "COUNT"
				<< std::setw(12) << "TOTAL"
				<< std::setw(12) << "MEAN"
				<< std::setw(12) << "LAST"
				<< std::setw(0) << std::endl;
		for(TagContainer::const_iterator it=tag_data_.begin(); it!=tag_data_.end(); ++it) {
			print(os, it->first);
		}
		os << "BENCHMARK RESULTS" << std::endl;
	}

public:
	static Benchmark& Instance() {
		static Benchmark* benchmark = 0;
		if(benchmark == 0) {
			benchmark = new Benchmark();
		}
		return *benchmark;
	}

//	static Benchmark& Instance() {
//		static Benchmark benchmark;
//		return benchmark;
//	}

private:
	boost::mutex mutex_;

	typedef std::map<std::string, Data> TagContainer;

	TagContainer tag_data_;

	unsigned int tag_name_max_length_;

	FunctorOnUpdate on_update_;

};

//---------------------------------------------------------------------------

#ifdef DANVIL_ENABLE_BENCHMARK
	#define DANVIL_BENCHMARK_START(TOKEN) \
		Danvil::Timer timer##TOKEN; timer##TOKEN.start();
	#define DANVIL_BENCHMARK_STOP(TOKEN) \
		timer##TOKEN.stop(); \
		Danvil::Benchmark::Instance().add(#TOKEN, timer##TOKEN.getElapsedTimeInMilliSec());
	#define DANVIL_BENCHMARK_STOP_WITH_NAME(TOKEN,NAME) \
		timer##TOKEN.stop(); \
		Danvil::Benchmark::Instance().add(NAME, timer##TOKEN.getElapsedTimeInMilliSec());
#define DANVIL_BENCHMARK_STOP_WITH_NAME_WITH_WEIGHT(TOKEN,NAME,WEIGHT) \
	timer##TOKEN.stop(); \
	Danvil::Benchmark::Instance().add(NAME, timer##TOKEN.getElapsedTimeInMilliSec(), WEIGHT);
#else
	#define DANVIL_BENCHMARK_START(TOKEN)
	#define DANVIL_BENCHMARK_STOP(TOKEN)
	#define DANVIL_BENCHMARK_STOP_WITH_NAME(TOKEN,NAME)
#endif

#define DANVIL_BENCHMARK_PRINTALL_COUT\
		Danvil::Benchmark::Instance().printAll(std::cout);

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif

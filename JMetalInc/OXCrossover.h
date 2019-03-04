/*
 * OXCrossover.h
 *
 *  Created on: 14 Feb 2019
 *      Author: Emad Alharbi
 */

#ifndef SRC_OPERATORS_CROSSOVER_OCCROSSOVER_H_
#define SRC_OPERATORS_CROSSOVER_OCCROSSOVER_H_
#include <JMetalInc/Crossover.h>
#include <JMetalInc/Solution.h>
#include <JMetalInc/Int.h>
#include <math.h>
class OXCrossover: public Crossover {
public:
	OXCrossover(map<string, void *> parameters);
	virtual ~OXCrossover();
	void *execute(void *);
private:
	 double crossoverProbability_;
		 Solution ** doCrossover(double probability,Solution * parent1, Solution * parent2);
};

#endif /* SRC_OPERATORS_CROSSOVER_OCCROSSOVER_H_ */

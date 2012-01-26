/*
 * @file GDE3.cpp
 * @author Esteban L�pez
 * @date 16 January 2012
 */

#include <GDE3.h>


/*
 * This class implements the GDE3 algorithm.
 */


/**
 * Constructor
 * @param problem Problem to solve
 */
GDE3::GDE3(Problem *problem) : Algorithm(problem) {

  //problem_ = problem; // MODIFIED BY AJNEBRO
  cout << "GDE3: Inicializado por segunda vez..." << endl;

} // GDE3


/*
 * Runs the GDE3 algorithm.
 * @return a <code>SolutionSet</code> that is a set of non dominated solutions
 * as a result of the algorithm execution
 */
SolutionSet * GDE3::execute() {

  int populationSize;
  int maxIterations;
  int evaluations;
  int iterations;

  SolutionSet * population;
  SolutionSet * offspringPopulation;
  SolutionSet * unionSolution;

  Distance * distance;
  Comparator * dominance;

  Operator * crossoverOperator;
  Operator * selectionOperator;

  distance  = new Distance();
  dominance = new DominanceComparator();

  Solution ** parent;

  //Read the parameters
  populationSize = *(int *) getInputParameter("populationSize");
  maxIterations  = *(int *) getInputParameter("maxIterations");

  cout << "GDE3: populationSize = " << populationSize << endl;
  cout << "GDE3: maxIterations = " << maxIterations << endl;

  //Initialize the variables
  population = new SolutionSet(populationSize);
  evaluations = 0;
  iterations  = 0;

  cout << "GDE3: Poblacion inicializada con maxsize = " << population->getMaxSize() << endl;
  cout << "GDE3: Poblacion inicializada con size = " << population->size() << endl;

  //Read the operators
  crossoverOperator = operators_["crossover"];
  selectionOperator = operators_["selection"];

  cout << "GDE3: Problema: " << problem_->getName() << endl;

  // Create the initial solutionSet
  Solution * newSolution;
  for (int i = 0; i < populationSize; i++) {
    newSolution = new Solution(problem_);
    problem_->evaluate(newSolution);
    problem_->evaluateConstraints(newSolution);
    evaluations++;
    population->add(newSolution);
  } //for

  cout << "GDE3: Comienzan las generaciones." << endl;

  // Generations ...
  while (iterations < maxIterations) {
    // Create the offSpring solutionSet
    offspringPopulation  = new SolutionSet(populationSize * 2);

    for (int i = 0; i < populationSize; i++){
      // Obtain parents. Two parameters are required: the population and the
      //                 index of the current individual
      void ** object1 = new void*[2];
      object1[0] = population;
      object1[1] = &i;
      parent = (Solution **) (selectionOperator->execute(object1));
      delete[] object1;

      Solution * child ;
      // Crossover. Two parameters are required: the current individual and the
      //            array of parents
      void ** object2 = new void*[2];
      object2[0] = population->get(i);
      object2[1] = parent;
      child = (Solution *) (crossoverOperator->execute(object2));
      delete[] object2;

      problem_->evaluate(child) ;
      problem_->evaluateConstraints(child);
      evaluations++ ;

      // Dominance test
      int result  ;
      result = dominance->compare(population->get(i), child) ;
      if (result == -1) { // Solution i dominates child
        offspringPopulation->add(population->get(i)) ;
      } // if
      else if (result == 1) { // child dominates
        offspringPopulation->add(child) ;
      } // else if
      else { // the two solutions are non-dominated
        offspringPopulation->add(child) ;
        offspringPopulation->add(population->get(i)) ;
      } // else
    } // for

    // Ranking the offspring population
    Ranking * ranking = new Ranking(offspringPopulation);

    int remain = populationSize;
    int index  = 0;
    SolutionSet * front = NULL;
    population->clear();

    // Obtain the next front
    front = ranking->getSubfront(index);

    while ((remain > 0) && (remain >= front->size())){
      //Assign crowding distance to individuals
      distance->crowdingDistanceAssignment(front,problem_->getNumberOfObjectives());
      //Add the individuals of this front
      for (int k = 0; k < front->size(); k++ ) {
        population->add(front->get(k));
      } // for

      //Decrement remain
      remain = remain - front->size();

      //Obtain the next front
      index++;
      if (remain > 0) {
        front = ranking->getSubfront(index);
      } // if
    } // while

    // remain is less than front(index).size, insert only the best one
    if (remain > 0) {  // front contains individuals to insert
      while (front->size() > remain) {
         distance->crowdingDistanceAssignment(front,problem_->getNumberOfObjectives());
         front->remove(front->indexWorst(new CrowdingComparator()));
      }
      for (int k = 0; k < front->size(); k++) {
        population->add(front->get(k));
      }

      remain = 0;
    } // if

    iterations ++ ;
  } // while

  // Return the first non-dominated front
  Ranking * ranking = new Ranking(population);
  return ranking->getSubfront(0);
} // execute
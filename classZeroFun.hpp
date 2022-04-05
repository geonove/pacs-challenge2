#ifndef _CLASS_ZERO_FUN_HPP_
#define _CLASS_ZERO_FUN_HPP_

#include "SolverTraits.hpp"
#include <cmath>
#include <limits>
#include <tuple>

/* * * * * * * * *
 *  Base class	 *
 * * * * * * * * */
class SolverBase
{
public:
	using T = SolverTraits;
	
	SolverBase(const T::FunType& f_, const double& tol_) : f(f_), tol(tol_) {}

	virtual T::Real solve() = 0;
		
	virtual ~SolverBase() = default;

protected:
	// Function 
	T::FunType f;
	// Tolerance 
	T::Real tol;

};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Base class for solvers that rely on interval to find the zero in  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
class SolverWithInterval : public SolverBase
{
public:
	// Constructor for when the interval extremes are provided by the user
	SolverWithInterval(const T::FunType& f_, const T::Real& a_, const T::Real& b_, const T::Real& tol_) : 
		SolverBase(f_, tol_), a(a_), b(b_), x1((a_ + b_) / 2.), h_interval(0.), maxIter(0u) 
	{
		if (f(a) * f(b) > 0)
		{
			std::cout << "The provided interval is not valid... Trying to find a new one that it is" << std::endl;

			auto result = bracketInterval(f, x1, h_interval, maxIter);
			if (std::get<2>(result))
			{
				a = std::get<0>(result);
				b = std::get<1>(result);

				std::cout << "The new interval will be [" << a << ", " << b << "] instead" << std::endl;
			}
			else 
			{
				std::cout << "Could not find an interval, the function has no zero. Interval will be set to [-Nan, Nan]" << std::endl;
				a = -std::numeric_limits<T::Real>::quiet_NaN();
				b = std::numeric_limits<T::Real>::quiet_NaN();
			}
		}
	}

	virtual ~SolverWithInterval() = default;

protected:	
	// Interval lower bound
	T::Real a;
	// Interval upper bound
	T::Real b;
	// Initial guess for bracketInterval function
	T::Real x1;
	// Step to be used in the bracketInterval function
	T::Real h_interval;
	// Maximum number of iterations to be used in the bracketInterval function
	T::Real maxIter;

	// Function to find an interval that contains the zero
	 std::tuple<T::Real, T::Real, bool> bracketInterval(const T::FunType &f, T::Real x1, T::Real h, unsigned int maxIter);

};

/* * * * * * * * * * * *
 * Regula Falsi method *
 * * * * * * * * * * * */
class RegulaFalsi final : public SolverWithInterval
{
public:
	// Constructor for when the interval is provided by the user
	RegulaFalsi(const T::FunType& f_, const T::Real& a_, const T::Real& b_, const T::Real& tol_, const T::Real& tola_) : 
		SolverWithInterval(f_, a_, b_, tol_), tola(tola_) {}

	T::Real solve() override;

protected:
	// Absolute tolerance
	T::Real tola;
};

/* * * * * * * * * * *
 * Bisection method  *
 * * * * * * * * * * */
class Bisection final: public SolverWithInterval
{
public:
	// Constructor used when interval extremes are provided by the user
	Bisection(const T::FunType& f_, const T::Real& a_, const T::Real& b_, const T::Real& tol_) : 
		SolverWithInterval(f_, a_, b_, tol_) {}

	T::Real solve() override;	

};

/* * * * * * * * *
 * Secant method *
 * * * * * * * * */
class Secant final : public SolverWithInterval
{
public:
	// Constructor for when interval extremes are provided by the user
	Secant(const T::FunType& f_, const T::Real& a_, const T::Real& b_, const T::Real& tol_, const T::Real& tola_, const unsigned int& maxIt_) : 
		SolverWithInterval(f_, a_, b_, tol_), tola(tola_), maxIt(maxIt_) {}
	
	T::Real solve() override;

protected:
	// Absolute tolerance
	T::Real tola;
	// Maximum number of iterations
	unsigned int maxIt; 	
};

/* * * * * * * * *
 * Brent method  *
 * * * * * * * * */
class Brent final : public SolverWithInterval
{
public:
	// Constructor for when interval extremes are provided by the user
	Brent(const T::FunType& f_, const T::Real& a_, const T::Real& b_, const T::Real tol_, const unsigned int& maxIt_) : 
		SolverWithInterval(f_, a_, b_, tol_), maxIt(maxIt_) {}

	T::Real solve() override;

protected:
	// Maximum number of iterations
	unsigned int maxIt;
};

/* * * * * * * * *
 * Newton method *
 * * * * * * * * */
class Newton : public SolverBase{
public:
	// Constructor
	Newton(const T::FunType& f_, const T::FunType& df_, const T::Real& x0_, const T::Real& tol_, const T::Real& tola_, const unsigned int& maxIt_) : 
		SolverBase(f_, tol_), df(df_), x0(x0_), tola(tola_), maxIt(maxIt_) {}

	T::Real solve() override;

protected:
	// Derivative of the function
	T::FunType df;
	// Initial point
	T::Real x0;
	// Absolute tolerance
	T::Real tola;
	// Maximum number of iterations
	unsigned int maxIt;
};

/* * * * * * * * * * * *
 * QuasiNewton method  *
 * * * * * * * * * * * */
class QuasiNewton final : public Newton
{
public:
	// Constructor
	QuasiNewton(const T::FunType& f_, const T::Real& a_, const T::Real& h_, const T::Real& tol_, const T::Real& tola_, const unsigned int& maxIt_) : 
		Newton(f_, [f_, h_](const T::Real& x){return (f_(x + h_) - f_(x - h_)) / (2 * h_);}, a_, tol_, tola_, maxIt_), h(h_) {}

protected:
	// Step for computing the derivative
	T::Real h;
};

#endif

#include "stdfx.hpp"

using namespace stdfx;
using namespace stdfx::examples;

Functional::Functional(void* mFunc, void* (*executeCall) (void*, std::vector<void*>&), int argCount) 
		: mFunction(mFunc),
		  mExecuteFunctional(executeCall),
		  mCount(argCount),
		  mFilled(0),
		  mIsEnabled(false) { }

Functional::Functional(Functional const &f)
		: mFunction(f.mFunction),
		  mExecuteFunctional(f.mExecuteFunctional),
		  mCount(f.mCount),
		  mFilled(0),
		  mIsEnabled(true) { }

Functional::~Functional() 
{ 
	void* pointer = Execute();
		
	// Cleanup
	if(pointer != nullptr)
		delete pointer;
}

void* Functional::Execute()
{
	if(CanExecute())
	{
		mIsEnabled = false;
		return mExecuteFunctional(mFunction, mArguments);
	}
	return nullptr;
}

void Functional::Push (void* arg)
{
	mArguments.push_back(arg);
	++mFilled;
}

bool Functional::CanExecute() 
{ 
	return mFilled >= mCount && mIsEnabled; 
}

Functional&& stdfx::operator , ( Functional&& a, Functional&& c )
{
	void* result = c.Execute();
	a.Push(result);
	return std::move(a);
}

Functional FXObject::operator [] (std::string name)
{
	return *FXObjectMethodMap[name];
}

std::ostream& operator << (std::ostream& out, Number const &c)
{
	out << c.value;
	return out;
}

Number& NumberOperator::Get()
{
	return state;
}

void NumberOperator::Acc(Number& a, Number const & b)
{
	a.value += b.value;
}

NumberOperator::NumberOperator() : state(0)
{
	EXPOSE_METHOD_ARG2(NumberOperator, Add, Number, Number const &, Number const & );
	EXPOSE_METHOD_ARG0(NumberOperator, Pi, Number );
	EXPOSE_METHOD_ARG1(NumberOperator, Sqr, Number, Number const & );
	EXPOSE_VOID_METHOD_ARG1(NumberOperator, Print, Number const & );

	EXPOSE_METHOD_ARG0(NumberOperator, Get, Number &);
	EXPOSE_VOID_METHOD_ARG2(NumberOperator, Acc, Number & , Number const &);
}

Number NumberOperator::Sqr(Number const & c)
{
	return Number(c.value *c.value);
}

Number NumberOperator::Pi()
{
	return Number(3.14159265);
}

Number NumberOperator::Add(Number const &  a, Number const & b)
{
	return Number(a.value + b.value);
}

void NumberOperator::Print(Number const &b)
{
	std::cout << b << std::endl;
}

void stdfx::examples::Print(Number const &b)
{
	std::cout << b << std::endl;
}

Functional stdfx::Call(std::shared_ptr<Functional> f)
{
	return *f;
}

std::shared_ptr<Functional> stdfx::examples::PrintNumber = 
	std::make_shared<Functional>( 
		new std::function<void(Number const &)>(&stdfx::examples::Print),
		&ExecuteVoidFunctional1<Number const &>, 
		1);

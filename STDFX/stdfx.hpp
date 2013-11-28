#ifndef PLUS_PLUS_STDFX_HEADER
#define PLUS_PLUS_STDFX_HEADER

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <memory>
#include <iostream>
#include <type_traits>

namespace stdfx
{

class Functional;

/* USAGE
 * Inherit from FXObject.
 * In your constructor use the above macros to expose functions to the Indexing operator.
 * For example, if my class is MyNewClass inheriting from ExObject and has a Sum method of the following
 * signature:
 * int MyNewClass::Sum(double a, double b)
 * Then my constructor will look like:
 * MyNewClass() 
 * {
 *     EXPOSE_METHOD_ARG2(MyNewClass, Sum, int, double, double);
 * }
 */
class FXObject
{
public:
	Functional operator [] (std::string name);

protected:
	FXObject() { }

	std::map< std::string, Functional* > FXObjectMethodMap;
};

class Functional
{
public:
	Functional() : mIsEnabled(false) { }

	Functional(void* mFunc, void* (*executeCall) (void*, std::vector<void*>&), int argCount);

	Functional(Functional const &f);

	virtual ~Functional();

	void* Execute();
	void Push (void* arg);

protected:
	bool CanExecute();

	std::vector<void*> mArguments;
	void* mFunction;
	void* (*mExecuteFunctional) (void*, std::vector<void*>&);

private:
	int mFilled;
	int mCount;
	bool mIsEnabled;
	size_t mReturnType;
};

template<typename R>
void* ConstructPointer(R& obj, std::false_type const &)
{
	return new R(obj);
}
template<typename R>
void* ConstructPointer(R& obj, std::true_type const &)
{
	return &obj;
}

template<typename Arg1, typename Arg2>
void* ExecuteVoidFunctional2(void* f, std::vector<void*>& arg)
{
	(*(std::function<void(Arg1,Arg2)>*)f)(*(std::remove_reference<Arg1>::type*)(arg[0]),*(std::remove_reference<Arg2>::type*)(arg[1]));
	return nullptr;
}

template<typename Arg1>
void* ExecuteVoidFunctional1(void* f, std::vector<void*>& arg)
{
	(*(std::function<void(Arg1)>*)f)(*(std::remove_reference<Arg1>::type*)(arg[0]));
	return nullptr;
}

template<typename R>
void* ExecuteFunctional0(void* f, std::vector<void*>& arg)
{
	return ConstructPointer( (*(std::function<R()>*)f)(), std::is_lvalue_reference<R>() );
}

template<typename R, typename A1>
void* ExecuteFunctional1(void* f, std::vector<void*>& arg)
{
	return ConstructPointer( (*(std::function<R(A1)>*)f)(*(std::remove_reference<A1>::type*)arg[0]), std::is_lvalue_reference<R>() );
}

template<typename R, typename A1, typename A2>
void* ExecuteFunctional2(void* f, std::vector<void*>& arg)
{
	return ConstructPointer( (*(std::function<R(A1, A2)>*)f)(*(std::remove_reference<A1>::type*)arg[0], *(std::remove_reference<A2>::type*)arg[1]), std::is_lvalue_reference<R>() );
}

template<typename B>
Functional&& operator , ( Functional&& f, B const & val )
{
	f.Push((void*)(&val));
	return std::move(f);
}

Functional&& operator , ( Functional&& a, Functional&& c );

#define EXPOSE_VOID_METHOD_ARG1(Class, FunctionName, Arg1) \
	FXObjectMethodMap.insert( std::pair< std::string, Functional* >( #FunctionName, \
			new Functional( new std::function<void(Arg1)>(std::bind(& ## Class ## :: ## FunctionName ##, this, std::placeholders::_1)), \
							&ExecuteVoidFunctional1<Arg1>, \
							1 ) ) ); 

#define EXPOSE_VOID_METHOD_ARG2(Class, FunctionName, Arg1, Arg2) \
	FXObjectMethodMap.insert( std::pair< std::string, Functional* >( #FunctionName, \
			new Functional( new std::function<void(Arg1,Arg2)>(std::bind(& ## Class ## :: ## FunctionName ##, this, std::placeholders::_1, std::placeholders::_2)), \
							&ExecuteVoidFunctional2<Arg1, Arg2>, \
							2 ) ) ); 

#define EXPOSE_METHOD_ARG0(Class, FunctionName, ReturnType) \
	FXObjectMethodMap.insert( std::pair< std::string, Functional* >( #FunctionName, \
			new Functional( new std::function<ReturnType()>(std::bind(& ## Class ## :: ## FunctionName ##, this)), \
							&ExecuteFunctional0<ReturnType>, \
							0 ) ) ); 

#define EXPOSE_METHOD_ARG1(Class, FunctionName, ReturnType, Arg1) \
	FXObjectMethodMap.insert( std::pair< std::string, Functional* >( #FunctionName, \
			new Functional( new std::function<ReturnType(Arg1)>(std::bind(& ## Class ## :: ## FunctionName ##, this, std::placeholders::_1)), \
							&ExecuteFunctional1<ReturnType, Arg1>, \
							1 ) ) ); 

#define EXPOSE_METHOD_ARG2(Class, FunctionName, ReturnType, Arg1, Arg2) \
	FXObjectMethodMap.insert( std::pair< std::string, Functional* >( #FunctionName, \
			new Functional( new std::function<ReturnType(Arg1, Arg2)>(std::bind(& ## Class ## :: ## FunctionName ##, this, std::placeholders::_1, std::placeholders::_2)), \
							&ExecuteFunctional2<ReturnType, Arg1, Arg2>, \
							2 ) ) ); 


Functional Call(std::shared_ptr<Functional> f);

	namespace examples
	{
	
	extern std::shared_ptr<Functional> PrintNumber;


	class Number
	{
	public:
		Number(double a) : value(a) { }

		double value;
	};
	
	void Print(Number const &);

	class NumberOperator : public FXObject
	{
	public:
		NumberOperator();
	
	private:
		Number Sqr(Number const & c);
		Number Pi();
		Number Add(Number const &  a, Number const & b);
		void Acc(Number& a, Number const & b);
		void Print(Number const & b);
		Number& Get();
	
		Number state;
	};
	
	}	

}



std::ostream& operator << (std::ostream& out, stdfx::examples::Number const &c);


#endif
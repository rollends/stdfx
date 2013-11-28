#include "stdfx.hpp"

int main(int argc, char* argv[])
{
	stdfx::examples::NumberOperator numOp;
	(numOp["Print"], (numOp["Add"], (numOp["Sqr"], numOp["Pi"]), stdfx::examples::Number(20)));

	// Accumalates 10 into the NumberOperator state twice nad prints current state
	(numOp["Acc"], numOp["Get"], stdfx::examples::Number(10));
	(numOp["Acc"], numOp["Get"], stdfx::examples::Number(10));
	(numOp["Print"], numOp["Get"]);

	(stdfx::Call(stdfx::examples::PrintNumber), numOp["Get"]);

	system("PAUSE");
}
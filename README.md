stdfx
=====

This project adds a new method of calling member functions of classes using new tools/language features found in C++11.
The example code below prints the result of ( pi^2 + 20 ).

Example:

stdfx::examples::NumberOperator numOp;
(numOp["Print"], (numOp["Add"], (numOp["Sqr"], numOp["Pi"]), stdfx::examples::Number(20)));


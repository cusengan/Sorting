#include "sorting.h"
#include "threads.h"

int main(){

	oneProcessSort();
	oneThreadSort();

	twoProcessSort();
	twoThreadSort();

	fourProcessSort();
	fourThreadSort();

	tenProcessSort();
	tenThreadSort();
}



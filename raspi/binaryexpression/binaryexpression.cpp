#include <iostream>
#include <stdlib.h>
#include <string>
using namespace std;
int flipbits(int n){

return~n;

}
string String(bool a){

	if(a==1){
		return "1";
	}else{
		return "0";

	}
}


string bin (long n){
	using namespace std;
	string test="";
	
	if(n==0){
		test="0";
		return test;
	}

	for(;n>=1;n/=2){
	
		test+=String(n%2);
	
	}
	string reversed(test.rbegin(),test.rend());
	return reversed ;
}

int main(){
	using namespace std;

	
	int se;
	
	int a[100];
	cin>>se;
	int *ptr=&se;


	cout<<"se: "<<se<<endl;
	for(int i=0;i<se+1;i++){
		cout<<bin(i);
		cout<<endl;
	}
	cout<<endl;
	cout<<"ptr: "<<ptr<<endl;

	 return 0;
}

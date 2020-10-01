#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "TGenPsi2S.h"

using namespace std;

//_____________________________________________________________________________
int main(int argc, char* argv[]) {

	std::string inFile, outFile;

	if(argc==1){
		inFile = "/afs/cern.ch/user/s/shuaiy/public/starlight/decayPsi2S/testFiles/slight_CohPsi2S_4Feeddown_0001.out";
		outFile = "test";
	}
	else if(argc==3){
		inFile  = std::string(argv[1]);
		outFile = std::string(argv[2]);
	}
	else{
		cout<<"arge should be equal to 1 or 3 !"<<endl;
		return -1;
	}

	TGenPsi2S *gen = new TGenPsi2S(inFile, outFile, 0);

	//gen->SetEtaRange(-2.5, 2.5);

	gen->EventLoop();

	delete gen;

	//TGenPsi2S gen("/afs/cern.ch/user/s/shuaiy/public/starlight/decayPsi2S/testFiles/slight_CohPsi2S_4Feeddown_0001.out", "test", 0);
	//gen.SetEtaRange(-1.2, 1.2);
	//gen.EventLoop();
	
	return 0;

}//main


/* Routine for the conversion of starlight data format
 * into something readable by CMSSW_1_4_5
 *
 * Modification by X. Rouby on a routine kindly offered by J. Hollar
 * Sept. 28, 2007

https://twiki.cern.ch/twiki/pub/CMS/UpsilonPhotoproduction/convert_starlight.C
https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideHeavyIonExclusiveUpsilonDileptonsAnalysis
http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/kumarv/Input/StarToHepmc.C?view=markup

 * Modifications by IK:
 * -- N_particles derived from starlight EVENT record
 * -- (-evtIdx) written as barcode for HepMC V record
 * -- TParticle used to derive masses from PDG codes

*/ 

#include <TStyle.h>
#include <TROOT.h>
#include <TApplication.h>

#include "TParticle.h"

#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <stdio.h>
#include <vector>

using namespace std;

void convert_SL2LHE(string infilename = "test.tx", string outfilename = "starlight_LHEtest", double beamE1 = 2510, double beamE2 = 2510) //makeEventsFile
{
	char ofName[100];
	sprintf(ofName,"%s.lhe", outfilename.c_str());
	ofstream output(ofName);

	string temp_string, temp;
	istringstream curstring;

	int NTrk = 0;

	int evtIdx=0; // event_number, read from the input-file
	int nEvts=0; // event_counter
	int nAccEvts=0; // accepted event_counter in the output file

	//do the header
	output << "<LesHouchesEvents version=\"1.0\">" << endl;
	output << "<!-- " << endl << " #Converted from STARLIGHT generator output " << endl << "-->" << endl;
	output << "<header>" << endl << "</header>" << endl;

	//put generic initialization-level info since STARLIGHT doesn't save this
	output << "<init>" << endl;
	//beam particle 1, 2, beam energy 1, 2, author group, beam 1, 2, PDFSET beam 1, 2,
	output << "22 " << "22 " << beamE1 << " " << beamE2 << " 0 " << "0 " << "0 " << "0 " << "3 " << "1" << endl;
	output << "1.0 " << "0.0 " << "3.0 " << "81" << endl;
	output << "</init>" << endl;

	ifstream infile(infilename.c_str());
	if (!infile.is_open()) { cout << "\t convert_starlight ERROR: I can not open \"" << infilename << "\"" << endl; return; }

	int useless;
	int pdg_id_temp=-1;
	double px_temp=0, py_temp=0, pz_temp=0;

	std::vector<double> px, py, pz, mass, e;
	std::vector<int>    pdg_id;
	while (getline(infile, temp_string)) {

		curstring.clear(); // needed when using several times istringstream::str(string)
		curstring.str(temp_string);

		//for each event, write Nparticles, process ID, event weight, event scale, and alpha_EM alpha_s
		if(strstr(temp_string.c_str(), "EVENT")) {
			nEvts++;
			if(nEvts%100000 == 0) cout<<"Working on "<< nEvts/100000 <<"-th 100k event ..."<<endl;

			// EVENT:          1       2       1
			curstring >> temp >> evtIdx >> NTrk >> useless; //assuming that EVENT always preceeds VERTEX/TRACK so that NTrk is set correctly

			pdg_id.clear();
			px.clear();
			py.clear();
			pz.clear();
			mass.clear();
			e.clear();
		}
		else if(strstr(temp_string.c_str(), "TRACK")) {
			curstring >> temp >> useless >> px_temp >> py_temp >> pz_temp >> useless >> useless >> useless >> pdg_id_temp;

			TParticle particle_temp(pdg_id_temp, 0, 0, 0, 0, 0, px_temp, py_temp, pz_temp, 0.0, 0.0, 0.0, 0.0, 0.0);
			double mass_temp = particle_temp.GetMass();
			double e_temp    = TMath::Sqrt(pow(mass_temp, 2) + pow(particle_temp.P(), 2));

			px.push_back(px_temp);
			py.push_back(py_temp);
			pz.push_back(pz_temp);
			mass.push_back(mass_temp);
			e.push_back(e_temp);
			pdg_id.push_back(pdg_id_temp);
		}

		//TParticle particle(pdg_id_temp, 0, 0, 0, 0, 0, px_temp, py_temp, pz_temp, 0.0, 0.0, 0.0, 0.0, 0.0);
		//TRACK:      6   2.9797       3.1399       84.461          1      1      0    -13
		if(NTrk == (int)px.size()){
			if(TMath::Abs(pdg_id[0])==13 && TMath::Abs(pdg_id[1])==13){
				TLorentzVector fourMom1(px[0], py[0], pz[0], e[0]);
				TLorentzVector fourMom2(px[1], py[1], pz[1], e[1]);

				TLorentzVector motherFourMom = fourMom1 + fourMom2;
				double y = motherFourMom.Rapidity();

				if(TMath::Abs(y)>1.45 && TMath::Abs(y)<2.45){
					if(nAccEvts) output << "</event>" << endl;
					output << "<event>" << endl;

					output << NTrk << " 81" << " 1.0 -1.0 -1.0 -1.0" << endl;
					for(size_t itrk=0; itrk<px.size(); itrk++){
						output << pdg_id[itrk] << " 1" << " 0 0 0 0 " << px[itrk] << " " << py[itrk] << " " << pz[itrk] << " " << e[itrk] << " " << mass[itrk] << " 0.0 9.0" << endl; 
					}

					nAccEvts++;
				}
			}
			else{
				cout<<"The first two tracks are not muons !"<<endl;
			}
		}

	} // reading loop of the input file

	output << "</event>" << endl;
	output << "</LesHouchesEvents>" << endl;
	output.close();

	infile.close();

	cout << nAccEvts << " events written in " << ofName << endl;
	return;
}

/* Explanation of the format :
 * +++ Event +++
 * E 1 -1.0000000000000000e+00 -1.0000000000000000e+00 -1.0000000000000000e+00 20 0 1 0 0
 *   1 : event number  			<-------
 *   -1 : event scale
 *   -1 : alpha_QCD
 *   -1 : alpha_QED
 *   20 : signal process ID
 *   0 : signal process vertex barcode
 *   1 : number of vertices 		<-------
 *   0 : list of vertices
 *   0 : ?
 *
 * +++ Vertex +++
 * V -1 0 0 0 0 0 0 4 0
 *   -1 : vertex barcode (unique)       <-------
 *    0 : vertex id
 *    0 0 0 0 : vertex x,y,z,t
 *    0 : number of orphans
 *    4 : number of out particles       <-------
 *    0 : weights
 *
 * +++ Particle +++
 * P 5 2212 -2.0 1.2 8.1 5.2 1 0 0 0 0   
 *    5 : barcode			<-------
 *    0 : pdg_id			<-------
 *   -2.0 : px				<-------
 *    1.2 : py				<-------
 *    8.1 : pz				<-------
 *    5.2 : e				<-------
 *    1 : status			<-------
 *    0 0  : polarization eta , phi
 *    0 0  : vertex and ?
 */

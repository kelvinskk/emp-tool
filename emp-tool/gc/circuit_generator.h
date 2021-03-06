#ifndef CIRCUIT_GENEARTOR_H__
#define CIRCUIT_GENEARTOR_H__
#include <emp-tool.h>
#include "circuit_file_generator.h"
#include <iostream>
#include <fstream>

void CircuitGenerator_gen_feed(Backend* be, EmpParty party, block * label, const bool*, int length);
void CircuitGenerator_gen_reveal(Backend* be, bool* clear, EmpParty party, const block * label, int length);

class CircuitGenerator: public Backend { public:
	CircuitFileGenerator * gc;
	std::ofstream fout;
	std::ofstream tmp;
	CircuitGenerator(CircuitFileGenerator* gc): Backend(PUBLIC) {
		this->gc = gc;	
		Feed_internal = CircuitGenerator_gen_feed;
		Reveal_internal = CircuitGenerator_gen_reveal;
	}
	void finalize() {
		tmp.close();
		fout<<gc->gates<<" "<<gc->gid<< std::endl;
		fout<<n1<<" "<<n2<<" "<<n3<< std::endl;
		std::ifstream fin("emp-toolkit_tmpfile");
		fout << fin.rdbuf();
		fout.close();
		fin.close();
	}
	int n1=0,n2=0,n3=0;
};

inline void CircuitGenerator_gen_feed(Backend* be, EmpParty party, block * label, const bool* b, int length) {
	CircuitGenerator * backend = (CircuitGenerator*)(be);
	for(int i = 0; i < length; ++i) {
		label[i] = backend->gc->private_label(b[i]);
	}
	if (party == ALICE)backend->n1+=length;
	else backend->n2+=length;
}

inline void CircuitGenerator_gen_reveal(Backend* be, bool* b, EmpParty party, const block * label, int length) {
	CircuitGenerator * backend = (CircuitGenerator*)(be);
	for (int i = 0; i < length; ++i) {
		b[i] = backend->gc->get_value(label[i]);
	}
	backend->n3+=length;
}

static void setup_circuit_generator(bool print, string filename) {
	local_backend = (Backend *) new CircuitGenerator(nullptr); 
	CircuitGenerator * backend = (CircuitGenerator * )local_backend;
	backend->tmp.open("emp-toolkit_tmpfile");
	backend->fout.open(filename);
	local_gc = new CircuitFileGenerator(print, backend->tmp);
	backend->gc = (CircuitFileGenerator *)local_gc;
}
static void finalize_circuit_generator (){
	CircuitGenerator * backend = (CircuitGenerator * )local_backend;
	backend ->finalize();
}
#endif //CIRCUIT_GENEARTOR_H__
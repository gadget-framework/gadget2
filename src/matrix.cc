#include "matrix.h"
#include "mathfunc.h"
#include "gadget.h"

Matrix::Matrix(int nr, int nc, double value) {

	m = new double[nr * nc];
	nrow = nr;
	ncol = nc;
	int i, j;
	for (i = 0; i < nr; i++) {
		for (j = 0; j < nc; j++)
			m[i*ncol+j] = value;
	}
}

Matrix::~Matrix() {
	int i;
	if (m != 0) {
		delete[] m;
		m = 0;
		ncol = nrow = 0;
	}
}

void Matrix::AddRows(int add, int length, double value) {
	if (add <= 0)
		return;

	int i, j;
	if (m == 0) {
		nrow = add;
		ncol = length;
		m = new double[nrow*ncol];
		for (i = 0; i < nrow; i++) {
			for (j = 0; j < length; j++)
				m[i*ncol+j] = value;
		}

	} else {
		int aux = nrow + add;
		double* vnew = new double[aux*length];
		for (i = 0; i < nrow; i++)
			for (j = 0; j < length; j++)
				vnew[i*ncol+j] = m[i*ncol+j];

		for (i = nrow; i < nrow + add; i++) {
			for (j = 0; j < length; j++)
				vnew[i*ncol+j] = value;
		}
		delete[] m;
		m = vnew;
		nrow = aux;
	}
}

void Matrix::Reset() {
	if (nrow > 0) {
		int i;
		delete[] m;
		m = 0;
		nrow = 0;
		ncol = 0;
	}
}

void Matrix::Initialize(int nr, int nc, double initial) {
	if (m != 0)
		delete[] m;

	m = new double[nr * nc];
	nrow = nr;
	ncol = nc;
	int i, j;
	for (i = 0; i < nr; i++) {
		for (j = 0; j < nc; j++)
			m[i*ncol+j] = initial;
		}
}

void Matrix::Print(ofstream& outfile) const {
	int i, j;
	for (i = 0; i < nrow; i++) {
		outfile << TAB;
		for (j = 0; j < ncol; j++)
			outfile << setw(smallwidth) << setprecision(smallprecision)
					<< m[i*ncol+j] << sep;
		outfile << endl;
	}
}

void Matrix::setToZero() {
	int i, j;
	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++)
			m[i*ncol+j] = 0.0;
}

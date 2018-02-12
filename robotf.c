/**************************************************************************//**
 * @file      robotf.c
 * @brief     Contains functions for representing our robotic arm
 * @version   V0.01
 * @date      6. Feb. 2018
 * @lastdate  6. Feb. 2018
 * @note      Implements the linear algebra necessary for representing and
 *            implementing the position and movement of a robotic arm
 *            
 *            This might be unnecessary depending on how much functionallity 
 *            we implement the final procedure of the robot.
 ******************************************************************************/


//********************************************************************************
//*****************   Matrix structures and asignments section   *****************
//********************************************************************************

// Structure Tm for holding the values of a T matrix
// because C is terrible at returning 2D matricies
struct Tm {
	double R[3][3];
	double P[3];
};

struct Tm makeTm(double r[3][3], double p[3]) {
	struct Tm tmp;
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			tmp.R[i][j] = r[i][j];
		}
		tmp.P[i] = p[i];
	}
	return tmp;
}

//********************************************************************************
//**************     Matrix structures and asignments section ENDs   *************
//********************************************************************************


//********************************************************************************
//********************      Matrix operation functions section   *****************
//********************************************************************************
// Using the Tm sturcture




//********************************************************************************
//****************     Matrix operation functions section ENDs   *****************
//********************************************************************************


//********************************************************************************
//********************      T matricies for our arm section      *****************
//********************************************************************************

// T matrix 1 from 0

// T matrix 2 from 1

// T matrix 3 from 2

// T matrix 4 from 3

// T matrix 5 from 4

// T matrix 5 from 0

// etc.

//********************************************************************************
//****************      T matricies for our arm section ENDs     *****************
//********************************************************************************

#include "Kalman.hh"

#define ROLL 0
#define PITCH 1
#define YAW 2

KalmanAHRS::KalmanAHRS(){}

void KalmanAHRS::countErrorForNewPreditedState(double result[6][6])
{
	int i, j;
	double tmp[6][6];
	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++)
			tmp[i][j] =
				A[i][0] * P[0][j] +
				A[i][1] * P[1][j] +
				A[i][2] * P[2][j] +
				A[i][3] * P[3][j] +
				A[i][4] * P[4][j] +
				A[i][5] * P[5][j];

	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++)
			if (i == j)
				result[i][j] = Q[i] +
							   tmp[i][0] * A[j][0] +
							   tmp[i][1] * A[j][1] +
							   tmp[i][2] * A[j][2] +
							   tmp[i][3] * A[j][3] +
							   tmp[i][4] * A[j][4] +
							   tmp[i][5] * A[j][5];
			else
				result[i][j] =
					tmp[i][0] * A[j][0] +
					tmp[i][1] * A[j][1] +
					tmp[i][2] * A[j][2] +
					tmp[i][3] * A[j][3] +
					tmp[i][4] * A[j][4] +
					tmp[i][5] * A[j][5];
}

void KalmanAHRS::countNewInputData(const double akc[3], const double gyro[3], double result[6])
{
	for (int i = 0; i < 3; i++)
		result[i] = akc[i] + z[i];
	for (int i = 3; i < 6; i++)
		result[i] = gyro[i - 3] + z[i];
}

void KalmanAHRS::countInverseCovariance(double Pkp[6][6])
{
	double tmp[6][12], ratio;
	int i, j, k;
	// t[1| ]=Pkp+R
	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++)
			if (i == j)
			{
				if(i < 3)
					tmp[i][j] = Pkp[i][j] + R[i];
				else
					tmp[i][j] = Pkp[i][j] + gyroskopeVariation;
				tmp[i][j + 6] = 1;
			}
			else
			{
				tmp[i][j] = Pkp[i][j];
				tmp[i][j + 6] = 0;
			}
	// t[ |2]=1(Pkp+R)
	for (i = 0; i < 6; i++)
		for (j = 6; j < 12; j++)
			if (i == (j - 6))
				tmp[i][j] = 1.0;
			else
				tmp[i][j] = 0.0;

	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++)
			if (i != j)
			{
				ratio = tmp[j][i] / tmp[i][i];
				for (k = 0; k < 12; k++)
					tmp[j][k] -= ratio * tmp[i][k];
			}

	for (i = 0; i < 6; i++)
		for (j = 6; j < 12; j++)
			P[i][j - 6] = tmp[i][j] / tmp[i][i];
}

void KalmanAHRS::countKalmanGain(double Pkp[6][6], double result[6][6])
{
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 6; j++)
			result[i][j] =
				Pkp[i][0] * P[0][j] +
				Pkp[i][1] * P[1][j] +
				Pkp[i][2] * P[2][j] +
				Pkp[i][3] * P[3][j] +
				Pkp[i][4] * P[4][j] +
				Pkp[i][5] * P[5][j];
}

void KalmanAHRS::countNewState(double K[6][6], double Yk[6])
{
	double Xp[6] = { X[0], X[1], X[2], X[3], X[4], X[5]};
	for (int i = 0; i < 6; i++)
		X[i] = Xp[i] +
			   K[i][0] * (Yk[0] - Xp[0]) +
			   K[i][1] * (Yk[1] - Xp[1]) +
			   K[i][2] * (Yk[2] - Xp[2]) +
			   K[i][3] * (Yk[3] - Xp[3]) +
			   K[i][4] * (Yk[4] - Xp[4]) +
			   K[i][5] * (Yk[5] - Xp[5]);
}

void KalmanAHRS::countNewErrorForState(double K[6][6], double Pkp[6][6])
{
	int i, j;
	// K=(eye-K)
	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++)
			if (i == j)
				K[i][j] = 1 - K[i][j];
			else
				K[i][j] = -K[i][j];
	// Pk=(eye-K)*Pkp;
	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++)
			P[i][j] =
				K[i][0] * Pkp[0][j] +
				K[i][1] * Pkp[1][j] +
				K[i][2] * Pkp[2][j] +
				K[i][3] * Pkp[3][j] +
				K[i][4] * Pkp[4][j] +
				K[i][5] * Pkp[5][j];
}

void KalmanAHRS::countMatrixA(const double acceleromiterMesurements[3], double roolAngle, double pitchAngle)
{
	double sa = sin(roolAngle);
	double ca = cos(roolAngle);
	double sb = sin(pitchAngle);
	double cb = cos(pitchAngle);
	A[0][0] = (acceleretionVectorLengthSquared - acceleromiterMesurements[0] * acceleromiterMesurements[0] ) * g / (acceleretionVectorLengthSquared * acceleretionVectorLength);
	A[0][1] = (-acceleromiterMesurements[0] * acceleromiterMesurements[1] ) * g / (acceleretionVectorLengthSquared * acceleretionVectorLength);
	A[0][2] = (-acceleromiterMesurements[0] * acceleromiterMesurements[2] ) * g / (acceleretionVectorLengthSquared * acceleretionVectorLength);
	A[0][4] = ca * cb * g * dt;
	A[0][5] = -sa * cb * g * dt;
	A[1][0] = (-acceleromiterMesurements[1] * acceleromiterMesurements[0] ) * g / (acceleretionVectorLengthSquared * acceleretionVectorLength);
	A[1][1] = (acceleretionVectorLengthSquared - acceleromiterMesurements[1] * acceleromiterMesurements[1] ) * g / (acceleretionVectorLengthSquared * acceleretionVectorLength);
	A[1][2] = (-acceleromiterMesurements[1] * acceleromiterMesurements[2] ) * g / (acceleretionVectorLengthSquared * acceleretionVectorLength);
	A[1][3] = -ca * cb * g * dt;
	A[1][5] = -sb * g * dt;
	A[2][0] = (-acceleromiterMesurements[2] * acceleromiterMesurements[0] ) * g / (acceleretionVectorLengthSquared * acceleretionVectorLength);
	A[2][1] = (-acceleromiterMesurements[2] * acceleromiterMesurements[1] ) * g / (acceleretionVectorLengthSquared * acceleretionVectorLength);
	A[2][2] = (acceleretionVectorLengthSquared - acceleromiterMesurements[2] * acceleromiterMesurements[2] ) * g / (acceleretionVectorLengthSquared * acceleretionVectorLength);
	A[2][3] = sa * cb * g * dt;
	A[2][4] = sb * g * dt;
}

void KalmanAHRS::countPreditedState()
{
	double tmpX = X[0];
	double tmpY = X[1];
	double tmpZ = X[2];
	double phiVelocity = X[3];
	double thetaVelocity = X[4];
	double psiVelocity = X[5];
	X[0] = tmpX / acceleretionVectorLength * g + A[0][4] * thetaVelocity + A[0][5] * psiVelocity;
	X[1] = tmpY / acceleretionVectorLength * g + A[1][3] * phiVelocity + A[1][5] * psiVelocity;
	X[2] = tmpZ / acceleretionVectorLength * g + A[2][3] * phiVelocity + A[2][4] * thetaVelocity;
}

double KalmanAHRS::operator[](unsigned int i)
{
	return X[i];
}

double *KalmanAHRS::run(const double akcelerometer[3], const double gyroskope[3], double angleRollInRadians, double anglePitchInRadians)
{
	double Pp[6][6], Yk[6], K[6][6];
	acceleretionVectorLengthSquared = akcelerometer[0] * akcelerometer[0] + akcelerometer[1] * akcelerometer[1] + akcelerometer[2] * akcelerometer[2];
	if(acceleretionVectorLengthSquared < 0.05)
		acceleretionVectorLengthSquared = 0.05;
	acceleretionVectorLength = sqrt(acceleretionVectorLengthSquared);
	countMatrixA(akcelerometer, angleRollInRadians, anglePitchInRadians);
	countPreditedState();
	// Xp=A*X+w
	// Pp=A*P*A'+Q
	countErrorForNewPreditedState(Pp);
	// Yk=Y+z
	countNewInputData(akcelerometer, gyroskope, Yk);
	// P=1/(Pp+R)
	countInverseCovariance(Pp);
	// K=Pp/(Pp+R)
	countKalmanGain(Pp, K);
	// X=Xp+K*(Yk-Xp);
	countNewState(K, Yk);
	// P=(eye-K)*Pp;
	countNewErrorForState(K, Pp);
	return X;
}

void KalmanAHRS::changeQ(const double newQ[6]){
	for(unsigned int i = 0; i < 6; i++)
		Q[i] = newQ[i];
}

void KalmanAHRS::changeR(const double newR[3]){
	for (unsigned int i = 0; i < 3; i++)
		R[i] = newR[i];
}

//
//  ml_log.cpp
//  MachineLearning
//
//  Created by Venkat Srinivasan on 6/28/17.
//  Copyright © 2017 Venkat Srinivasan. All rights reserved.
//

#include <math.h>
#include <cassert>
#include <complex>
#include "include/ml_log.hpp"

/* Performs the sigmoid function on each item in the provided matrix */
Matrix *ML_LogOps::sigmoid(Matrix &z)
{
	Matrix &Sigmoid_Matrix = *(new Matrix(z));
	Sigmoid_Matrix.MultiplyScalar(-1.0);

	double exponent = exp((double) 1.0);
	Sigmoid_Matrix.MatrixPower(exponent);
	Sigmoid_Matrix.AddScalar(1);
	Sigmoid_Matrix.ReciprocalMultiply(1.0);
	
	return &Sigmoid_Matrix;
}

/* Assumptions: Features in columns, examples in rows */
double ML_LogOps::computeCost(Matrix &training_X, Matrix &training_y, Matrix &training_theta)
{
	/* cost = (1/m) * sum((-1 * y) .* log(hyp) - ((1-y) .* log(1 - hyp))); */

	int numTrainingExamples = training_y.numRows();
	int c_idx = 0;
	int r_idx = 0;

	Matrix &X = *(new Matrix(training_X));
	X.AddBiasCol();

	Matrix *hypothesis = sigmoid(*(X * training_theta));

	Matrix &scale = *(new Matrix(training_y));
	scale.MultiplyScalar(-1);

	Matrix &scale_2 = *(new Matrix(training_y));
	scale_2.SubtractFromScalar(1);

	Matrix &hypothesis2 = *(new Matrix(*hypothesis));

	hypothesis->Log_e();

	hypothesis2.SubtractFromScalar(1);
	hypothesis2.Log_e();

	scale.Transpose();
	scale_2.Transpose();

	Matrix *result_1 = scale * (*hypothesis);

	Matrix *result_2 = scale_2 * (hypothesis2);

	Matrix &result = *((*result_1) - (*result_2));

	Matrix &sum_result = *(new Matrix(1, result.numCols()));
	for (c_idx = 0; c_idx < result.numCols(); c_idx++)
	{
		double runningColCount = 0;
		for (r_idx = 0; r_idx < result.numRows(); r_idx++)
		{
			Indexer *currentIndex = new Indexer(r_idx, c_idx);
			runningColCount = runningColCount + result[currentIndex];
			delete currentIndex;
		}
		Indexer *currentMean = new Indexer(0, c_idx);
		sum_result[currentMean] = runningColCount;
	}

	sum_result.MultiplyScalar((double) ((double)1.0 / (double) numTrainingExamples));

	assert(sum_result.numCols() == 1 && sum_result.numRows() == 1);

	/* FIXME: Add Regularization Expression */

	return sum_result[0];
}

Matrix *ML_LogOps::gradientCalculate(Matrix &training_X, Matrix &training_y, Matrix &theta)
{
	/* 1/m * sum((hyp - y) .* X) */
	int numTrainingSet = training_X.numRows();
	Matrix &X = *(new Matrix(training_X));
	X.AddBiasCol();
	int c_idx, r_idx = 0;
	double constant = ((double) 1.0) / ((double) numTrainingSet);
	Matrix *hypothesis = sigmoid(*(X * theta));
	Matrix *TermOne = (*hypothesis) - training_y;
	Matrix *TermTwo = new Matrix(X.numRows(), X.numCols());
	Matrix *gradient = new Matrix(1, theta.numRows());

	delete hypothesis;

	assert(TermOne->numCols() == 1);
	for (r_idx = 0; r_idx < X.numRows(); r_idx++)
	{
		for (c_idx = 0; c_idx < X.numCols(); c_idx++)
		{
			Indexer *currentIndex = new Indexer(r_idx, c_idx);
			(*TermTwo)[currentIndex] = (*TermOne)[r_idx] * X[currentIndex];
			delete currentIndex;
		}
	}

	for (c_idx = 0; c_idx < TermTwo->numCols(); c_idx++)
	{
		double runningColCount = 0;
		for (r_idx = 0; r_idx < TermTwo->numRows(); r_idx++)
		{
			Indexer *currentIndex = new Indexer(r_idx, c_idx);
			runningColCount = runningColCount + (*TermTwo)[currentIndex];
			delete currentIndex;
		}
		Indexer *currentMean = new Indexer(0, c_idx);
		(*gradient)[currentMean] = runningColCount;
	}

	gradient->MultiplyScalar(constant);

	gradient->Transpose();

	return gradient;
}

Matrix *ML_LogOps::GradientDescent(Matrix &training_X, Matrix &training_y, Matrix &theta, double alpha, int num_iterations)
{
	int iteration_idx = 0;
	Matrix *result = new Matrix(theta);
	Matrix *X = new Matrix(training_X);
	X->AddBiasCol();

	for (iteration_idx = 0; iteration_idx < num_iterations; iteration_idx++)
	{
		Matrix *nextGradient = ML_LogOps::gradientCalculate(training_X, training_y, *result);
		nextGradient->MultiplyScalar(alpha);
		result = (*result) - (*nextGradient);
	}

	delete X;
	return result;
}

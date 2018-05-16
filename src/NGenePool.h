#ifndef NGENETIC_H
#define NGENETIC_H

#include <vector>
#include <algorithm>
#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/IO/Serializer.h>
#include <Urho3D/Resource/XMLFile.h>//for calling the writing out of the nn serilization
#include <Urho3D/Resource/XMLElement.h>
#include "NGenome.h"

#pragma once

//  Author: Mat Buckland 2002
//  Desc: Genetic algorithm class.This is based for manipulating std::vectors 
//  of *real* numbers. Used to adjust the weights in a feedforward neural
//  network.-

///okay later I need to make it not a vector of pointers
//so dont use NEW
//and return it like this
//NGenome* GetSpecimen(){return &population_[index];}

using namespace Urho3D;

//-----------------------------------------------------------------------
//	create a structure to hold each genome
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
//
//	the genetic algorithm class
//-----------------------------------------------------------------------
class NGenePool : public Serializable
{
	URHO3D_OBJECT(NGenePool, Serializable);
public:
	NGenePool(Context* context, unsigned PopMaxSize=10, float MutRate=0.1, float CrossRate=0.7, float MutPeturb = 0.3);//unsigned numWeights,
	static void RegisterObject(Context* context);

	void GiveNumberOfWeights(unsigned num);
	void Add(SharedPtr<NGenome> genome);//add genome to the population
	//void Remove(NGenome* genome);//remove genome from the population
	SharedPtr<NGenome> GetSpecimen();//get a fresh evolved specimen
	
	void Save();
	bool Save(Serializer& dest) const override;
	bool SaveXML(XMLElement& dest) const override;
	//Vector<NGenome> Epoch(Vector<NGenome> &old_pop); //this runs the GA for one generation.
	//-------------------accessor methods
	//Vector<NGenome> GetChromos()const{return pop_;}
	//float AverageFitness()const{return totalFitness_ / float(popSize_);}
	//float BestFitness()const{return bestFitness_;}

private:
	
	Vector< SharedPtr<NGenome> > population_;//this holds the entire population of chromosomes	
	unsigned popSize_; //size of population
	unsigned popMaxSize_; //max size of population
	unsigned chromoLength_; //amount of weights per chromo
	float totalFitness_; //total fitness of population
	float bestFitness_; //best fitness this population
	float averageFitness_; //average fitness
	float worstFitness_;//worst
	unsigned fittestGenome_; //keeps track of the best genome
	unsigned weakestGenome_; //keeps track of the weak genome
	float mutationRate_; //probability that a chromosones bits will mutate. //Try figures around 0.05 to 0.3 ish
	float mutationPerturb_;
	float crossoverRate_; //probability of chromosones crossing over bits //0.7 is pretty good
	unsigned generation_;//generation counter

	void Crossover(const std::vector<float> &mum, const std::vector<float> &dad, std::vector<float> &baby1, std::vector<float> &baby2);
	void Mutate(std::vector<float> &chromo);
	SharedPtr<NGenome> GetChromoRoulette();

  	//void GrabNBest(unsigned NBest, const unsigned NumCopies, Vector<NGenome*> &Pop);//use to introduce elitism
	void CalculateBestWorstAvTot();
	//void Reset();

	VariantMap vars_;
};

#endif


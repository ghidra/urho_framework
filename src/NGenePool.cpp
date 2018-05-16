#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Container/Sort.h>
#include <Urho3D/IO/File.h>

#include <Urho3D/IO/Log.h>

#include "NGenePool.h"

//-----------------------------------constructor-------------------------
//
//	sets up the population with random floats
//
//-----------------------------------------------------------------------
NGenePool::NGenePool(Context* context, unsigned PopMaxSize, float MutRate, float CrossRate, float MutPeturb)	
	: Serializable(context)
	, popSize_(0)
	, popMaxSize_(PopMaxSize)
	, mutationRate_(MutRate)
	, mutationPerturb_(MutPeturb)
	, crossoverRate_(CrossRate)
	, chromoLength_(0)//chromoLength_(numWeights),
	, totalFitness_(0)
	, generation_(0)
	, fittestGenome_(0)
	, bestFitness_(0)
	, worstFitness_(99999999)
	, averageFitness_(0)
{//unsigned numWeights taking out the need to know this right now
	//initialise population with chromosomes consisting of random
	//weights and all fitnesses set to zero
	/*for (unsigned i=0; i<popSize_; ++i)
	{
		pop_.Push( NGenome() );
		for (unsigned j=0; j<chromoLength_; ++j)
		{
			pop_[i].weights_.Push( Random()-Random() );
		}
	}*/

	
}

void NGenePool::RegisterObject(Context* context)
{
	context->RegisterFactory<NGenePool>();
	URHO3D_ATTRIBUTE("Variables", VariantMap, vars_, Variant::emptyVariantMap, AM_DEFAULT);

	URHO3D_ATTRIBUTE("generation_", unsigned, generation_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("popSize_", unsigned, popSize_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("popMaxSize_ ", unsigned, popMaxSize_, 32, AM_DEFAULT);
	URHO3D_ATTRIBUTE("chromoLength_", unsigned, chromoLength_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("totalFitness_", float, totalFitness_, 0.0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("bestFitness_", float, bestFitness_, 0.0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("averageFitness_", float, averageFitness_, 0.0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("worstFitness_", float, worstFitness_, 99999999.0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("fittestGenome_", unsigned, fittestGenome_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("weakestGenome_", unsigned, weakestGenome_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("mutationRate_", float, mutationRate_, 0.05, AM_DEFAULT);
	URHO3D_ATTRIBUTE("mutationPerturb_", float, mutationPerturb_, 1.0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("crossoverRate_", float, crossoverRate_, 0.7, AM_DEFAULT);
}
void NGenePool::Save()
{
	Urho3D::SharedPtr<Urho3D::File> file(new Urho3D::File(context_, "nn.xml", Urho3D::FILE_WRITE));
	// Ensure file is open
	//if (file->IsOpen()) {
	//	genePoolCoPilots_->Save(*file);
	//}
	//make xml
	SharedPtr<XMLFile> xml(new XMLFile(context_));
	XMLElement rootElem = xml->CreateRoot("NeuralNet");
	bool saved = SaveXML(rootElem);
	if (saved)
		xml->Save(*file);
}
//this is saving a binary file
bool NGenePool::Save(Serializer& dest) const
{
	// Write node ID
	if (!dest.WriteUInt(1))
		return false;

	//// Write attributes
	//if (!Animatable::Save(dest))
	//	return false;

	//// Write components
	//dest.WriteVLE(GetNumPersistentComponents());
	//for (unsigned i = 0; i < components_.Size(); ++i)
	//{
	//	Component* component = components_[i];
	//	if (component->IsTemporary())
	//		continue;

	//	// Create a separate buffer to be able to skip failing components during deserialization
	//	VectorBuffer compBuffer;
	//	if (!component->Save(compBuffer))
	//		return false;
	//	dest.WriteVLE(compBuffer.GetSize());
	//	dest.Write(compBuffer.GetData(), compBuffer.GetSize());
	//}

	//// Write child nodes
	//dest.WriteVLE(GetNumPersistentChildren());
	//for (unsigned i = 0; i < children_.Size(); ++i)
	//{
	//	Node* node = children_[i];
	//	if (node->IsTemporary())
	//		continue;

	//	if (!node->Save(dest))
	//		return false;
	//}

	return true;
}
bool NGenePool::SaveXML(XMLElement& dest) const
{
	// Write node ID
	//if (!dest.SetUInt("id", id_))
	if (!dest.SetUInt("id", 12345))
		return false;

	//// Write attributes
	if (!Serializable::SaveXML(dest))
		return false;

	if (!dest.SetUInt("ENDED", 9000))
		return false;

	//// Write components
	//for (unsigned i = 0; i < components_.Size(); ++i)
	//{
	//	Component* component = components_[i];
	//	if (component->IsTemporary())
	//		continue;

	//	XMLElement compElem = dest.CreateChild("component");
	//	if (!component->SaveXML(compElem))
	//		return false;
	//}

	//// Write child nodes
	//for (unsigned i = 0; i < children_.Size(); ++i)
	//{
	//	Node* node = children_[i];
	//	if (node->IsTemporary())
	//		continue;

	//	XMLElement childElem = dest.CreateChild("node");
	//	if (!node->SaveXML(childElem))
	//		return false;
	//}

	return true;
}

//-----------------------------------Epoch()-----------------------------
//
//	takes a population of chromosones and runs the algorithm through one
//	 cycle.
//	Returns a new population of chromosones.
//
//-----------------------------------------------------------------------
/*
Vector<NGenome> NGenePool::Epoch(Vector<NGenome> &old_pop)
{
	//assign the given population to the classes population
  	population_ = old_pop;

  	//reset the appropriate variables
  	Reset();

  	//sort the population (for scaling and elitism)
  	//std::sort(pop_.Begin(), pop_.End());
  	Sort( population_.Begin(), population_.End() );
  	//http://urho3d.prophpbb.com/topic1330.html


  	//calculate best, worst, average and total fitness
	CalculateBestWorstAvTot();
  
  	//create a temporary vector to store new chromosones
	Vector<NGenome> vecNewPop;

	//Now to add a little elitism we shall add in some copies of the
	//fittest genomes. Make sure we add an EVEN number or the roulette
  	//wheel sampling will crash
	if (!(4 % 2))//CParams::iNumCopiesElite * CParams::iNumElite
	{
		GrabNBest(4, 1, vecNewPop);
	}
	

	//now we enter the GA loop
	
	//repeat until a new population is generated
	while (vecNewPop.Size() < popSize_)
	{
		//grab two chromosones
		NGenome mum = GetChromoRoulette();
		NGenome dad = GetChromoRoulette();

		//create some offspring via crossover
		Vector<float> baby1, baby2;

		Crossover(mum.weights_, dad.weights_, baby1, baby2);

		//now we mutate
		Mutate(baby1);
		Mutate(baby2);

		//now copy into vecNewPop population
		vecNewPop.Push( NGenome(baby1, 0));
		vecNewPop.Push( NGenome(baby2, 0));
	}

	//finished so assign new pop back into m_vecPop
	population_ = vecNewPop;

	return population_;
}
*/

void NGenePool::GiveNumberOfWeights(unsigned num)
{
	//here we are given a chromosome length for mutation later... its currently set everytime we make an enemy, so we only need to change it once
	if(chromoLength_<=0)
		chromoLength_=num;
}
void NGenePool::Add(SharedPtr<NGenome> genome)
{
	population_.Push(genome);
	popSize_++;
}
/*void NGenePool::Remove(NGenome* genome)
{
	//maybe we can just mark it for removal, to clean it up after ward... so that I have a certain populations sticking around
	//population_.Remove(genome);
	population_.Erase(population_.Find(genome));
	popSize_--;
	//URHO3D_LOGWARNING("removed a genome");
}*/

SharedPtr<NGenome> NGenePool::GetSpecimen()
{
	//NGenome* specimen;
	SharedPtr<NGenome> g;
	//this is for a fresh gen 1 specimen
	if(population_.Size()<popMaxSize_)//10 is the minimum sample set hard coded at the moment
	{
		std::vector<float> weights;
		for (int i=0; i<chromoLength_; ++i)
		{
			weights.push_back(Random()-Random());
		}

		g = new NGenome(context_,weights);
		//URHO3D_LOGWARNING("THIS IS A RANDOM BABY:"+String(population_.Size())+":"+String(popSize_));
	}
	else
	{//we have at least x to sample from
		//now we can do the epoch
		//Vector<NGenome*> SamplePopulation;//first make a temp vector to hold our population, and add in some of our stringest
		CalculateBestWorstAvTot();
		//GrabNBest(4, 1, SamplePopulation);//top x number, number of copies, the population to append to
		SharedPtr<NGenome> parent1 = population_.At(fittestGenome_);
		SharedPtr<NGenome> parent2 = GetChromoRoulette();
		std::vector<float> baby1, baby2;
		Crossover(parent1->weights_, parent2->weights_, baby1, baby2);
		Mutate(baby1);
		Mutate(baby2);
		unsigned gen = parent1->generation_>parent2->generation_ ? parent1->generation_ : parent2->generation_;
		unsigned fit = parent1->fitness_>parent2->fitness_ ? parent1->fitness_ : parent2->fitness_;
		g = new NGenome(context_,baby1,fit*0.9,gen+1);
		//URHO3D_LOGWARNING("THIS IS A Evolved BABY:"+String(population_.Size()));
		//SINCE we CALCULATED THE BEST AND WORSE, NOW WE CAN DO OUR REMOVAL
		if(weakestGenome_>=0)
			population_.Erase(weakestGenome_);
	}

	Add(g);
	//
	//i need to send back evolved genomes to use
	return population_.Back();

}

//-------------------------------------Crossover-----------------------
//  given parents and storage for the offspring this method performs
//	crossover according to the GAs crossover rate
//-----------------------------------------------------------------------
void NGenePool::Crossover(const std::vector<float> &mum, const std::vector<float> &dad, std::vector<float> &baby1, std::vector<float> &baby2)
{
	//just return parents as offspring dependent on the rate
	//or if parents are the same
	if ( (Random() > crossoverRate_) || (mum == dad)) 
	{
		baby1 = mum;
		baby2 = dad;

		return;
	}

	//determine a crossover point
	unsigned cp = (unsigned)(Random(0, chromoLength_ - 1));

	//create the offspring
	for (unsigned i=0; i<cp; ++i)
	{
		baby1.push_back(mum[i]);
		baby2.push_back(dad[i]);
	}

	for (unsigned i=cp; i<mum.size(); ++i)
	{
		baby1.push_back(dad[i]);
		baby2.push_back(mum[i]);
	}
	
	return;
}

//---------------------------------Mutate--------------------------------
//	mutates a chromosome by perturbing its weights by an amount not 
//	greater than CParams::dMaxPerturbation
//-----------------------------------------------------------------------
void NGenePool::Mutate(std::vector<float> &chromo)
{
	//traverse the chromosome and mutate each weight dependent
	//on the mutation rate
	for (unsigned i=0; i<chromo.size(); ++i)
	{
		if (Random() < mutationRate_)//do we perturb this weight?
		{
			chromo[i] += ( (Random()-Random()) * mutationPerturb_);//add or subtract a small value to the weight
		}
	}
}

//----------------------------------GetChromoRoulette()------------------
//
//	returns a chromo based on roulette wheel sampling
//
//-----------------------------------------------------------------------
SharedPtr<NGenome> NGenePool::GetChromoRoulette()
{
	//generate a random number between 0 & total fitness count
	float Slice = Random() * totalFitness_;
	SharedPtr<NGenome> TheChosenOne;//this will be set to the chosen chromosome
	float FitnessSoFar = 0;//go through the chromosones adding up the fitness so far
	
	for (unsigned i=0; i<popSize_; ++i)
	{
		FitnessSoFar += population_[i]->fitness_;
		
		//if the fitness so far > random number return the chromo at 
		//this point
		if (FitnessSoFar >= Slice)
		{
			TheChosenOne = population_[i];

      		break;
		}
		
	}

	return TheChosenOne;
}

//-------------------------GrabNBest----------------------------------
//	This works like an advanced form of elitism by inserting NumCopies
//  copies of the NBest most fittest genomes into a population vector
//--------------------------------------------------------------------
/*void NGenePool::GrabNBest(unsigned NBest, const unsigned NumCopies, Vector<NGenome*> &Pop)
{
  	//add the required amount of copies of the n most fittest 
	//to the supplied vector
	while(NBest--)
	{
		for (unsigned i=0; i<NumCopies; ++i)
		{
			Pop.Push(population_[(popSize_ - 1) - NBest]);
	  }
	}
}*/

//-----------------------CalculateBestWorstAvTot-----------------------	
//
//	calculates the fittest and weakest genome and the average/total 
//	fitness scores
//---------------------------------------------------------------------
void NGenePool::CalculateBestWorstAvTot()
{
	totalFitness_ = 0;
	
	float HighestSoFar = 0;
	float LowestSoFar  = 9999999;
	unsigned weakest = -1;
	
	for (unsigned i=0; i<population_.Size(); ++i)
	{
		SharedPtr< NGenome > g = population_.At(i); 
		//update fittest if necessary
		if (g->fitness_ > HighestSoFar)
		{
			HighestSoFar = g->fitness_;
			fittestGenome_ = i;
			bestFitness_ = HighestSoFar;
		}
		
		//update worst if necessary
		if (g->fitness_ < LowestSoFar)
		{
			LowestSoFar = g->fitness_;
			weakest = i;	
			worstFitness_ = LowestSoFar;
		}
		
		totalFitness_ += population_.At(i)->fitness_;
		
		
	}//next chromo

	//here we are removing genomes... so I need to hold out for a moment
	//URHO3D_LOGWARNING("size:"+String(population_.Size())+" max size:"+String(popMaxSize_)+"weakest:"+String(weakest)+":"+String(population_.At(weakest)->MarkedForRemoval())+"fittest:"+String(fittestGenome_));
	if(population_.Size()>popMaxSize_ && population_.At(weakest)->MarkedForRemoval() && weakest>=0)
	{
		//URHO3D_LOGWARNING("WE WANT TO ERASE:"+String(weakest));
		//population_.Erase(weakest);
		weakestGenome_=weakest;
	}
	
	averageFitness_ = totalFitness_ / population_.Size();
}

/*
//-------------------------Reset()------------------------------
//
//	resets all the relevant variables ready for a new generation
//--------------------------------------------------------------
void NGenePool::Reset()
{
	totalFitness_ = 0;
	bestFitness_ = 0;
	worstFitness_ = 9999999;
	averageFitness_	= 0;
}
*/
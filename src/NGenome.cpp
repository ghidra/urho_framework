#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>

#include <Urho3D/IO/Log.h>

#include "NGenome.h"

//NGenome::NGenome(Context* context) 
//	: Serializable(context)
//	, markedForRemoval_(false)
//	, weightsVariant_(Variant::emptyVariantMap)
//{
//
//}
NGenome::NGenome(Context* context, unsigned id, std::vector<float> w, float f, unsigned g)
	: Serializable(context)
	, weights_(w)
	, fitness_(f)
	, generation_(g)
	, markedForRemoval_(false)
	, weightsVariant_(Variant::emptyVariantVector)
	, id_(id)
{
	//URHO3D_ATTRIBUTE("weightsVariant_", VariantMap, weightsVariant_, Variant::emptyVariantMap, AM_DEFAULT);

}
NGenome::NGenome(Context* context, const XMLElement& source)
	: Serializable(context)
	, fitness_(0.0)
	, generation_(0)
	, markedForRemoval_(false)
	, weightsVariant_(Variant::emptyVariantVector)
	, weights_(std::vector<float>{})
{
	LoadXML(source);
}

void NGenome::RegisterObject(Context* context)
{
	context->RegisterFactory<NGenome>();

	URHO3D_ATTRIBUTE("fitness_", float, fitness_, 0.0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("generation_", unsigned, generation_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("weightsVariant_", VariantVector, weightsVariant_, Variant::emptyVariantVector, AM_DEFAULT);
}

String NGenome::DebugString()
{
	String s;
	if (!weights_.empty())
  	{
  		
  		s=String("gen:"+String(generation_)+" fit:" +String(roundf(fitness_*100)/100) +"\n");
      //s=String("fit:" +String(roundf(fitness_*100)/100) +"\n");
  		//s+=String( "w:"+String( roundf(weights_[0]*100)/100 ) );
      /*
    s+=String( "w:" );
      for(unsigned i=0; i<weights_.size(); i++)
      {
        s+=String( roundf(weights_[i]*100)/100 )+",";
      }
      s+=String("\n");
      */
  		/*std::ostringstream s;
    	// Convert all but the last element to avoid a trailing ","
    	std::copy(weights_.begin(), weights_.end()-1,
        	std::ostream_iterator<float>(s, ","));

    	// Now add the last element with no delimiter
    	s << weights_.back();*/
  }
  return s;
  //return String(s.str().c_str());
}

bool NGenome::Save(Serializer& dest) const
{
	// Write node ID
	if (!dest.WriteUInt(1))
		return false;
	return true;
}
bool NGenome::SaveXML(XMLElement& dest) const
{
	// Write node ID
	//if (!dest.SetUInt("id", id_))
	if (!dest.SetUInt("Id", id_))
		return false;

	///save attributes
	// if (!Serializable::SaveXML(dest))
	// 	return false;

	//save our weights in thier own thing
	for (unsigned i=0; i< weights_.size(); i++)
	{
		weightsVariant_.Push(Variant(weights_[i]));
	}

	//// Write attributes
	if (!Serializable::SaveXML(dest))
		return false;

	return true;
}
bool NGenome::LoadXML(const XMLElement& source)
{
	URHO3D_LOGWARNING("----------------------------------- WE ARE TRYING TO LOAD GENOME DATA");
	String nnID = source.GetAttribute("Id");
	id_ = ToUInt(nnID);//give the id over
	URHO3D_LOGWARNING("----------------------------------- ID: "+String(nnID));
	XMLElement attrElem = source.GetChild("attribute");// .GetChild("variant");
	while (attrElem)
	{
		if (attrElem.GetAttribute("name") == "weightsVariant_")
		{
			XMLElement wElem = attrElem.GetChild("variant");
			while (wElem)
			{
				URHO3D_LOGWARNING("----------------------------------- w: " + wElem.GetAttribute("value") );
				weights_.push_back( ToFloat(wElem.GetAttribute("value")) );
				wElem = wElem.GetNext("variant");
			}
		}
		else
		{
			//load other attributes like normal
			SetAttribute(attrElem.GetAttribute("name"), attrElem.GetAttribute("value"));
		}
		attrElem = attrElem.GetNext("attribute");
	}
	return true;
}

void NGenome::MarkForRemoval()
{
  markedForRemoval_=true;;
}
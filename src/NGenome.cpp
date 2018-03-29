#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>

#include "NGenome.h"

NGenome::NGenome(Context* context, std::vector<float> w, float f, unsigned g) :	
	Object(context),
	weights_(w), 
	fitness_(f),
  generation_(g),
  markedForRemoval_(false)
{
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
void NGenome::MarkForRemoval()
{
  markedForRemoval_=true;;
}
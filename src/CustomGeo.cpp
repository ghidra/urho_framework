#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/IndexBuffer.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include "CustomGeo.h"

#include <Urho3D/DebugNew.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Engine/DebugHud.h>

CustomGeo::CustomGeo(Context* context):
    Object(context)
{
}
CustomGeo::~CustomGeo(){}

void CustomGeo::AddPoint(const Vector3 p)
{
	points_.Push(p);
	FitBB(p);
	//add empty value to shared_normals_ids
	shared_normal_ids_.Push(PODVector<unsigned>());
}
void CustomGeo::AddPointUV(const Vector2 uv)
{
	uvs_.Push(uv);
}
void CustomGeo::AddPointColor(const Vector3 c)
{
	colors_.Push(c);
}

/*void CustomGeo::SetPoint(const unsigned short i, const Vector3 p)
{
	points_[i] = p;
	FitBB(p);
}*/

void CustomGeo::AddTriangle(const unsigned p1, const unsigned p2, const unsigned p3,const bool t)
{
	ids_.Push(p1);
	ids_.Push(p2);
	ids_.Push(p3);
	//for smoothing, hand over to the shared_normal_ids_
	unsigned si = normals_.Size();
	shared_normal_ids_[p1].Push(si);
	shared_normal_ids_[p2].Push(si);
	shared_normal_ids_[p3].Push(si);

	normals_.Push(Normal(points_[p1],points_[p2],points_[p3]));
	if( t && uvs_.Size()>0 )//we have to have us to figure out the tangents
		tangents_.Push(Tangent(p1,p2,p3));//tangents_.Push(Tangent(normals_[normals_.Size()-1]));//
}
//http://stackoverflow.com/questions/12662891/c-passing-member-function-as-argument
void CustomGeo::Surface(const unsigned slices, const unsigned stacks, Vector3 (CustomGeo::*fptr)(void*, float, float), void* context)
{
	for(unsigned i=0; i<slices+1; ++i)
	{
		float theta = float(i)*3.1415/float(slices);
		for(unsigned j=0; j<stacks; ++j)
		{
			float phi = float(j)*2.0*3.1415/float(stacks);
			Vector3 p = (this->*fptr)(context,theta,phi);
			AddPoint(p);
		}
	}
	unsigned v = 0;
	for(unsigned i=0; i<slices; ++i)
	{
		for(unsigned j=0; j<stacks; ++j)
		{
			unsigned next = (j+1)%stacks;
			AddTriangle(v+j,v+next,v+j+stacks);
			AddTriangle(v+next,v+next+stacks,v+j+stacks);
			//AddTriangle(v+j+stacks,v+next+stacks,v+next);
		}
		v+=stacks;
	}
}
void CustomGeo::Subdivide(const unsigned short depth){
	for(unsigned i=0; i<depth; ++i)
	{
		DoSubdivide();
	}
}
void CustomGeo::DoSubdivide(){
	unsigned tris = ids_.Size()/3;
	for(unsigned fi=0; fi<tris; ++fi)
	{
		//make new points
		unsigned fii = fi*3;
		Vector3 a = points_[ids_[fii]];
		Vector3 b = points_[ids_[fii+1]];
		Vector3 c = points_[ids_[fii+3]];
		points_.Push( a+(Vector3(b-a)*0.5f) );
		points_.Push( b+(Vector3(c-b)*0.5f) );
		points_.Push( c+(Vector3(a-c)*0.5f) );
		
		//add new ids
		unsigned i = points_.Size()-3;
		unsigned j = i+1;
		unsigned k = i+2;
		AddTriangle(i,j,k);
		AddTriangle(ids_[fii],i,k);
		AddTriangle(i,ids_[fii+1],j);

		ids_[fii]=k;
		ids_[fii+1]=j;
		normals_[fi] = Normal(points_[k],points_[j],points_[ids_[fii+2]]);

	}
}
//https://github.com/MonkeyFirst/urho3d-component-tail-generator/blob/master/TailGenerator.cpp
void CustomGeo::Build(Node* node, const bool smooth, const bool rigid, const unsigned layer, const unsigned mask)
{
	node_ = node;

	unsigned num = ids_.Size();
	const unsigned numVertices = num;
	
	unsigned skip = 6;//need to make sure I set up the vertex buffers right skipping the right number of values
	skip+=(colors_.Size()>0)?3:0;
	skip+=(uvs_.Size()>0)?2:0;
	skip+=(tangents_.Size()>0)?4:0;

	float vertexData[num*skip];
	unsigned short indexData[num];

	for(unsigned i = 0; i < numVertices; ++i)
	{
		
		unsigned ii = i*skip;

		vertexData[ii] = points_[ids_[i]].x_;
		vertexData[ii+1] = points_[ids_[i]].y_;
		vertexData[ii+2] = points_[ids_[i]].z_;

		//the normals a poly based.. so I need to grab them in orger the polys were made
		if(smooth)
		{
			Vector3 avg = GetSmoothNormal(ids_[i]);
			vertexData[ii+3] = avg.x_;
			vertexData[ii+4] = avg.y_;
			vertexData[ii+5] = avg.z_;
		}
		else
		{
			vertexData[ii+3] = normals_[i/3].x_;
			vertexData[ii+4] = normals_[i/3].y_;
			vertexData[ii+5] = normals_[i/3].z_;
			//vertexData[ii+3] = normals_[ids_[i/3]].x_;
			//vertexData[ii+4] = normals_[ids_[i/3]].y_;
			//vertexData[ii+5] = normals_[ids_[i/3]].z_;
		}

		//colors
		if(colors_.Size()>0)
		{
			vertexData[ii+6] = colors_[ids_[i]].x_;
			vertexData[ii+7] = colors_[ids_[i]].y_;
			vertexData[ii+8] = colors_[ids_[i]].z_;
		}
		//uvss
		if(uvs_.Size()>0)
		{
			unsigned ioff = (colors_.Size()>0)?9:6;
			vertexData[ii+ioff] = uvs_[ids_[i]].x_;
			vertexData[ii+ioff+1] = uvs_[ids_[i]].y_;
		}
		if(tangents_.Size()>0)
		{
			unsigned ioff = (colors_.Size()>0)?9:6;
			ioff+=(uvs_.Size()>0)?2:0;
			vertexData[ii+ioff] = tangents_[i/3].x_;
			vertexData[ii+ioff+1] = tangents_[i/3].y_;
			vertexData[ii+ioff+2] = tangents_[i/3].z_;
			vertexData[ii+ioff+3] = 1.0f;
		}

		indexData[i]=i;
	}
	/////
	/////
	/////

	SharedPtr<Model> fromScratchModel(new Model(context_));
	SharedPtr<VertexBuffer> vb(new VertexBuffer(context_));
	SharedPtr<IndexBuffer> ib(new IndexBuffer(context_));
	SharedPtr<Geometry> geom(new Geometry(context_));

	// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
	vb->SetShadowed(true);
	if(uvs_.Size()>0 && colors_.Size()>0 && tangents_.Size()>0)
	{
		vb->SetSize(numVertices, MASK_POSITION|MASK_NORMAL|MASK_COLOR|MASK_TEXCOORD1|MASK_TANGENT);
	}
	else if(uvs_.Size()>0 && colors_.Size()==0 && tangents_.Size()>0)
	{
		vb->SetSize(numVertices, MASK_POSITION|MASK_NORMAL|MASK_TEXCOORD1|MASK_TANGENT);
	}
	else if(uvs_.Size()==0 && colors_.Size()>0 && tangents_.Size()>0)
	{
		vb->SetSize(numVertices, MASK_POSITION|MASK_NORMAL|MASK_COLOR|MASK_TANGENT);
	}
	else if(uvs_.Size()==0 && colors_.Size()==0 && tangents_.Size()>0)
	{
		vb->SetSize(numVertices, MASK_POSITION|MASK_NORMAL|MASK_TANGENT);
	}
	//no tangents 
	else if(uvs_.Size()>0 && colors_.Size()>0 && tangents_.Size()==0)
	{
		vb->SetSize(numVertices, MASK_POSITION|MASK_NORMAL|MASK_COLOR|MASK_TEXCOORD1);
	}
	else if(uvs_.Size()>0 && colors_.Size()==0 && tangents_.Size()==0)
	{
		vb->SetSize(numVertices, MASK_POSITION|MASK_NORMAL|MASK_TEXCOORD1);
	}
	else if(uvs_.Size()==0 && colors_.Size()==0 && tangents_.Size()==0)
	{
		vb->SetSize(numVertices, MASK_POSITION|MASK_NORMAL);
	}
	vb->SetData(vertexData);

	ib->SetShadowed(true);
	ib->SetSize(numVertices, false);
	ib->SetData(indexData);

	geom->SetVertexBuffer(0, vb);
	geom->SetIndexBuffer(ib);
	geom->SetDrawRange(TRIANGLE_LIST, 0, numVertices);

	fromScratchModel->SetNumGeometries(1);
	fromScratchModel->SetGeometry(0, 0, geom);
	fromScratchModel->SetBoundingBox(BoundingBox(bbmin_, bbmax_));

	//Node* newnode = node_->GetScene()->CreateChild("FromScratchObject");
	//newnode->SetPosition(Vector3(0.0f, 3.0f, 0.0f));
	StaticModel* object = node_->CreateComponent<StaticModel>();
	object->SetModel(fromScratchModel);
	object->SetCastShadows(true);

	//make a triangle mesh rigid shape if flagged
	if(rigid)
	{
		RigidBody* body_ = node_->CreateComponent<RigidBody>();
    	body_->SetCollisionLayer(layer);
    	body_->SetCollisionMask(mask);
    	body_->SetMass(0.0f);
    	//body_->SetPosition(Vector3::ZERO);
    	//body_->SetRotation(Quaternion::IDENTITY);
    	//body_->SetFriction(friction);
    	CollisionShape* shape_ = node_->CreateComponent<CollisionShape>();
    	shape_->SetTriangleMesh(fromScratchModel);
    	//shape_->SetPosition(Vector3::ZERO);
    	//shape_->SetRotation(Quaternion::IDENTITY);
	}
}

void CustomGeo::FitBB(const Vector3 p)
{
	if(p.x_>bbmax_.x_)bbmax_.x_=p.x_;
	if(p.y_>bbmax_.y_)bbmax_.y_=p.y_;
	if(p.z_>bbmax_.z_)bbmax_.z_=p.z_;
	if(p.x_<bbmin_.x_)bbmin_.x_=p.x_;
	if(p.y_<bbmin_.y_)bbmin_.y_=p.y_;
	if(p.z_<bbmin_.z_)bbmin_.z_=p.z_;
}

Vector3 CustomGeo::Normal(const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
	Vector3 edge1 = p1 - p2;
           	Vector3 edge2 = p1 - p3;
	return edge1.CrossProduct(edge2).Normalized();
}
Vector3 CustomGeo::GetSmoothNormal(const unsigned i)
{
	Vector3 avg;
	unsigned si = shared_normal_ids_[i].Size();
	for(unsigned j = 0; j < si; ++j)
	{
		avg+=normals_[shared_normal_ids_[i][j]];
	}
	return avg/float(si);
}
Vector3 CustomGeo::Tangent(Vector3 n)
{
	//tis method from urho3d::terrain
	return (Vector3::RIGHT - n * n.DotProduct(Vector3::RIGHT)).Normalized();
}
Vector3 CustomGeo::Tangent(const unsigned p1, const unsigned p2, const unsigned p3)
{
	//tis method from here: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
	Vector3 deltaPos1 = points_[p2]-points_[p1];
    Vector3 deltaPos2 = points_[p3]-points_[p1];

    Vector2 deltaUV1 = uvs_[p2]-uvs_[p1];
    Vector2 deltaUV2 = uvs_[p3]-uvs_[p1];

    float r = 1.0f / (deltaUV1.x_ * deltaUV2.y_ - deltaUV1.y_ * deltaUV2.x_);
    
    return (deltaPos1 * deltaUV2.y_   - deltaPos2 * deltaUV1.y_)*r;
    //bitangent can be calced too: bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;
}
///-----surfaces
//http://prideout.net/blog/?p=44
Vector3 CustomGeo::Sphere(void* context, const float u, const float v)
{
	float x = sin(u)*cos(v);
	float y = cos(u);
	float z = -sin(u)*sin(v);
	return Vector3(x,y,z);
}
void CustomGeo::GetSphere(const unsigned short u, const unsigned short v)
{
	Surface(u,v,&CustomGeo::Sphere,0);
}
Vector3 CustomGeo::Klein(void* context, const float u, const float v)
{
	float x,y,z;
	float nu = u*2.0f;
	if(u<3.14){
		x = 3.0f * cos(nu) * (1.0f + sin(nu)) + (2.0f * (1.0f - cos(nu) / 2.0f)) * cos(nu) * cos(v);
		z = -8.0f * sin(nu) - 2.0f * (1.0f - cos(nu) / 2.0f) * sin(nu) * cos(v);
	}
	else
	{
		x = 3.0f * cos(nu) * (1.0f + sin(nu)) + (2.0f * (1.0f - cos(nu) / 2.0f)) * cos(v + 3.14);
		z = -8.0f * sin(nu);
	}
	y = -2.0f * (1.0f - cos(nu) / 2.0f) * sin(v);
	return Vector3(x,y,z);
}
void CustomGeo::GetKlein(const unsigned short u, const unsigned short v)
{
	Surface(u,v,&CustomGeo::Klein,0);
}
//------debug
void CustomGeo::Debug(const String label, const String value){
    GetSubsystem<DebugHud>()->SetAppStats(label+" : ", value );
}
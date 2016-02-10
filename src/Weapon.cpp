#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>

#include "Weapon.h"
#include "Projectile.h"
#include "Controller.h"//for the constants
//#include "../piece/Character.h"


Weapon::Weapon(Context* context) :
    Actor(context),
    firing_(false),
    fire_velocity_(50.0f),
    firing_timer_(0.0f),
    firing_interval_(0.2f),
    num_projectiles_(1),
    projectile_speed_(80.0f),
    projectile_range_(20.0f),
    continuous_(false),
    //continuous_spawned_(false),
    mag_size_(16),
    mag_remains_(16),
    reload_timer_(0.0f),
    reload_interval_(0.25f),
    reloading_(false),
    fire_off_(Vector3(0.0f,0.0f,2.0f)),
    lefthand_off_(Vector3(-0.4f,0.8f,1.1f))
{
    mesh_ = String("Man/MAN_gun.mdl");
}
Weapon::~Weapon(){}

/*void Weapon::FixedUpdate(float timeStep)
{
    Actor::FixedUpdate(timeStep);
    //something
    //SetLeftHandOffset();
}*/
void Weapon::Update(Controls& ctrl, float timeStep)
{
    lefthand_target_=node_->GetWorldTransform()*lefthand_off_;
    //node_->SetPosition(Vector3(0.2f, 0.2f, 0.2f));//objectNode
    //this is called from the pawn controlling it, fron thier fixed update, like a state
    if(reloading_)
        Reload(timeStep);

    if (ctrl.IsDown(CTRL_FIRE))
        Fire(timeStep);
    else
        ReleaseFire();
}

void Weapon::Setup()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    node_->SetPosition(Vector3(0.2f, 0.2f, 0.2f));//objectNode

    // Create the rendering component + animation controller
    //AnimatedModel* object = node_->CreateComponent<AnimatedModel>();
    StaticModel* object = node_->CreateComponent<StaticModel>();
    object->SetModel(cache->GetResource<Model>("Models/"+mesh_));
    object->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    object->SetCastShadows(true);

    lefthand_grip_ = node_->CreateChild("lefthand_grip");
    lefthand_grip_->SetPosition(lefthand_off_); 

}

////firing mechanisms
void Weapon::Reload(const float timeStep)
{
    reload_timer_+=timeStep;
    if(reload_timer_ > reload_interval_)
    {
        mag_remains_=mag_size_;
        reloading_ = false;
        if(continuous_)//have to set this to false to make it fire again
            firing_=false;
    }
}
//updateremains
//when reload is overriddedn and we want to update this information
/*void Weapon::UpdateMagRemains(const unsigned amount)
{
    mag_remains_+=amount;
}*/

void Weapon::StartReload()
{
    reloading_ = true;
    reload_timer_ = 0.0f;//reset the timer
}

void Weapon::SetProjectileRate(const unsigned rate){num_projectiles_=rate;}
void Weapon::SetFireRate(const float fireRate){firing_interval_ = 1000.0f/(fireRate*1000.0f);}
void Weapon::SetProjectileSpeed(const float speed){projectile_speed_=speed;}
void Weapon::SetProjectileRange(const float range){projectile_range_=range;}
void Weapon::SetProjectileContinuous(const bool continuous){continuous_=continuous;}

void Weapon::SetMagSize(const unsigned size,  const float rate)
{
    mag_size_=size;
    reload_interval_=1000.0f/(rate*1000.0f);
    mag_remains_=size;
    reload_timer_=0.0f;
}
void Weapon::Fire(float timeStep)
{
    
    //check the mag_remains_ first to see if we need to reload
    if(mag_remains_<=0 && !reloading_)//auto reload
        StartReload();

    //do the firing part
    if(!reloading_)
    //if(mag_remains_>0)
    {
        if(firing_ )//if we are firing, just deal with the timer
        {
            firing_timer_ += timeStep;

            //this is a lame attempt at setting it back
            //node_->SetRotation(Quaternion());
            //node_->Translate(-kick_off_);
            //kick_off_=Vector3(0.0f,0.0f,0.0f);
            
            if(firing_timer_ > firing_interval_ )
            {
                firing_timer_=0.0f;
                mag_remains_-=1;
                //we dont need to spawn if we are continuous
                //debug_->Hud("PROJECTILE",String(continuous_));
                if(!continuous_)
                {
                    SpawnProjectile();
                    firing_ = true;//need to make sure that we set this so we dont immediatly delete the projectile, since its looking for this
                }
                else
                {
                    Recoil();//just do the recoil if we are continuous
                }
            }
        }
        else
        {
            firing_ = true;
            firing_timer_ = timeStep;
            mag_remains_-=1;
            SpawnProjectile();
        }
    }
    //SpawnProjectile();   
}
void Weapon::ReleaseFire()
{
    firing_ = false;
    firing_timer_ = 0.0f;
    node_->SetTransform(Vector3(),Quaternion());
    //kick_rot_ = Quaternion();
    //node_->SetRotation(Quaternion());
    //lefthand_target_=lefthand_off_;
    //temporaily put this here, since its being called all the time if we are not firing.. and the target value needs to be set
    //lefthand_target_=node_->GetWorldTransform()*lefthand_off_;
}
void Weapon::Recoil()
{
    //I think that before I make the projectile, I should move the gun, to straighted it, as well as create kick
    Quaternion rot = node_->GetWorldRotation();
    Vector3 pos = node_->GetWorldPosition();

    //initlal rotation of fire_direction
    Vector3 rotoff = rot*fire_off_;
    ProjectileSpawnPosition_ = pos+rotoff;
    ProjectileSpawnDirection_ = rotoff.Normalized()*Vector3(1.0f,1.0f,0.0f);//make sure its stays on plane

    //get rotation axis
    //Vector3 rotaxis = dir.CrossProduct(Vector3(0.0f,1.0f,0.0f));//local to the gun
    Vector3 rotaxis = Vector3(0.0f,1.0f,0.0f);//local to the gun
    kick_rot_ = Quaternion(Random(4.0f),rotaxis);
    kick_off_ = Vector3(Random(0.1f),Random(0.1f),Random(0.1f));

    node_->SetTransform(kick_off_,kick_rot_);

    lefthand_target_=lefthand_grip_->GetWorldPosition();
}
void Weapon::SpawnProjectile()
{
    Recoil();
}

/*Vector3 Weapon::GetLeftHandTarget()
{
    //get the updated world position to update target positions for IK
    //Matrix3x4 updated_trans = node_->GetWorldTransform();
    //GetSubsystem<DebugHud>()->SetAppStats("gun_pos:", updated_trans*lefthand_off_ );
    //return updated_trans * lefthand_off_ ;
    return lefthand_grip_->GetWorldPosition();
}*/
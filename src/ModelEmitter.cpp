//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "ModelEffect.h"
#include "ModelEmitter.h"

#include <Urho3D/Precompiled.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Profiler.h>
#include <Urho3D/Graphics/DrawableEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>

#include <Urho3D/DebugNew.h>

// From Urho3D/Graphics/Drawable.cpp
const char* GEOMETRY_CATEGORY = "Geometry";

extern const char* faceCameraModeNames[];

static const unsigned MAX_PARTICLES_IN_FRAME = 100;

extern const char* autoRemoveModeNames[];

ModelEmitter::ModelEmitter(Context* context) :
    BillboardSet(context),
    periodTimer_(0.0f),
    emissionTimer_(0.0f),
    lastTimeStep_(0.0f),
    lastUpdateFrameNumber_(M_MAX_UNSIGNED),
    emitting_(true),
    needUpdate_(false),
    serializeModels_(true),
    sendFinishedEvent_(true),
    autoRemove_(REMOVE_DISABLED)
{
    SetNumModels(DEFAULT_NUM_PARTICLES);
}

ModelEmitter::~ModelEmitter()
{
}

void ModelEmitter::RegisterObject(Context* context)
{
    context->RegisterFactory<ModelEmitter>(GEOMETRY_CATEGORY);

    URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Effect", GetEffectAttr, SetEffectAttr, ResourceRef, ResourceRef(ModelEffect::GetTypeStatic()),
        AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Can Be Occluded", IsOccludee, SetOccludee, bool, true, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Cast Shadows", bool, castShadows_, false, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Draw Distance", GetDrawDistance, SetDrawDistance, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Shadow Distance", GetShadowDistance, SetShadowDistance, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Animation LOD Bias", GetAnimationLodBias, SetAnimationLodBias, float, 1.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Is Emitting", bool, emitting_, true, AM_FILE);
    URHO3D_ATTRIBUTE("Period Timer", float, periodTimer_, 0.0f, AM_FILE | AM_NOEDIT);
    URHO3D_ATTRIBUTE("Emission Timer", float, emissionTimer_, 0.0f, AM_FILE | AM_NOEDIT);
    URHO3D_ENUM_ATTRIBUTE("Autoremove Mode", autoRemove_, autoRemoveModeNames, REMOVE_DISABLED, AM_DEFAULT);
    URHO3D_COPY_BASE_ATTRIBUTES(Drawable);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Models", GetModelsAttr, SetModelsAttr, VariantVector, Variant::emptyVariantVector,
        AM_FILE | AM_NOEDIT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Billboards", GetModelBillboardsAttr, SetBillboardsAttr, VariantVector, Variant::emptyVariantVector,
        AM_FILE | AM_NOEDIT);
    URHO3D_ATTRIBUTE("Serialize Models", bool, serializeModels_, true, AM_FILE);
}

void ModelEmitter::OnSetEnabled()
{
    BillboardSet::OnSetEnabled();

    Scene* scene = GetScene();
    if (scene)
    {
        if (IsEnabledEffective())
            SubscribeToEvent(scene, E_SCENEPOSTUPDATE, URHO3D_HANDLER(ModelEmitter, HandleScenePostUpdate));
        else
            UnsubscribeFromEvent(scene, E_SCENEPOSTUPDATE);
    }
}

void ModelEmitter::Update(const FrameInfo& frame)
{
    if (!effect_)
        return;

    // Cancel update if has only moved but does not actually need to animate the models
    if (!needUpdate_)
        return;

    // If there is an amount mismatch between models and billboards, correct it
    if (models_.Size() != billboards_.Size())
        SetNumBillboards(models_.Size());

    bool needCommit = false;

    // Check active/inactive period switching
    periodTimer_ += lastTimeStep_;
    if (emitting_)
    {
        float activeTime = effect_->GetActiveTime();
        if (activeTime && periodTimer_ >= activeTime)
        {
            emitting_ = false;
            periodTimer_ -= activeTime;
        }
    }
    else
    {
        float inactiveTime = effect_->GetInactiveTime();
        if (inactiveTime && periodTimer_ >= inactiveTime)
        {
            emitting_ = true;
            sendFinishedEvent_ = true;
            periodTimer_ -= inactiveTime;
        }
        // If emitter has an indefinite stop interval, keep period timer reset to allow restarting emission in the editor
        if (inactiveTime == 0.0f)
            periodTimer_ = 0.0f;
    }

    // Check for emitting new models
    if (emitting_)
    {
        emissionTimer_ += lastTimeStep_;

        float intervalMin = 1.0f / effect_->GetMaxEmissionRate();
        float intervalMax = 1.0f / effect_->GetMinEmissionRate();

        // If emission timer has a longer delay than max. interval, clamp it
        if (emissionTimer_ < -intervalMax)
            emissionTimer_ = -intervalMax;

        unsigned counter = MAX_PARTICLES_IN_FRAME;

        while (emissionTimer_ > 0.0f && counter)
        {
            emissionTimer_ -= Lerp(intervalMin, intervalMax, Random(1.0f));
            if (EmitNewModel())
            {
                --counter;
                needCommit = true;
            }
            else
                break;
        }
    }

    // Update existing models
    Vector3 relativeConstantForce = node_->GetWorldRotation().Inverse() * effect_->GetConstantForce();
    // If billboards are not relative, apply scaling to the position update
    Vector3 scaleVector = Vector3::ONE;
    if (scaled_ && !relative_)
        scaleVector = node_->GetWorldScale();

    for (unsigned i = 0; i < models_.Size(); ++i)
    {
        EmitModel& model = models_[i];
        Billboard& billboard = billboards_[i];

        if (billboard.enabled_)
        {
            needCommit = true;

            // Time to live
            if (model.timer_ >= model.timeToLive_)
            {
                billboard.enabled_ = false;
                continue;
            }
            model.timer_ += lastTimeStep_;

            // Velocity & position
            const Vector3& constantForce = effect_->GetConstantForce();
            if (constantForce != Vector3::ZERO)
            {
                if (relative_)
                    model.velocity_ += lastTimeStep_ * relativeConstantForce;
                else
                    model.velocity_ += lastTimeStep_ * constantForce;
            }

            float dampingForce = effect_->GetDampingForce();
            if (dampingForce != 0.0f)
            {
                Vector3 force = -dampingForce * model.velocity_;
                model.velocity_ += lastTimeStep_ * force;
            }
            billboard.position_ += lastTimeStep_ * model.velocity_ * scaleVector;
            billboard.direction_ = model.velocity_.Normalized();

            // Rotation
            billboard.rotation_ += lastTimeStep_ * model.rotationSpeed_;

            // Scaling
            float sizeAdd = effect_->GetSizeAdd();
            float sizeMul = effect_->GetSizeMul();
            if (sizeAdd != 0.0f || sizeMul != 1.0f)
            {
                model.scale_ += lastTimeStep_ * sizeAdd;
                if (model.scale_ < 0.0f)
                    model.scale_ = 0.0f;
                if (sizeMul != 1.0f)
                    model.scale_ *= (lastTimeStep_ * (sizeMul - 1.0f)) + 1.0f;
                billboard.size_ = model.size_ * model.scale_;
            }

            // Color interpolation
            unsigned& index = model.colorIndex_;
            const Vector<ColorFrame>& colorFrames_ = effect_->GetColorFrames();
            if (index < colorFrames_.Size())
            {
                if (index < colorFrames_.Size() - 1)
                {
                    if (model.timer_ >= colorFrames_[index + 1].time_)
                        ++index;
                }
                if (index < colorFrames_.Size() - 1)
                    billboard.color_ = colorFrames_[index].Interpolate(colorFrames_[index + 1], model.timer_);
                else
                    billboard.color_ = colorFrames_[index].color_;
            }

            // Texture animation
            unsigned& texIndex = model.texIndex_;
            const Vector<TextureFrame>& textureFrames_ = effect_->GetTextureFrames();
            if (textureFrames_.Size() && texIndex < textureFrames_.Size() - 1)
            {
                if (model.timer_ >= textureFrames_[texIndex + 1].time_)
                {
                    billboard.uv_ = textureFrames_[texIndex + 1].uv_;
                    ++texIndex;
                }
            }
        }
    }

    if (needCommit)
        Commit();

    needUpdate_ = false;
}

void ModelEmitter::SetEffect(ModelEffect* effect)
{
    if (effect == effect_)
        return;

    Reset();

    // Unsubscribe from the reload event of previous effect (if any), then subscribe to the new
    if (effect_)
        UnsubscribeFromEvent(effect_, E_RELOADFINISHED);

    effect_ = effect;

    if (effect_)
        SubscribeToEvent(effect_, E_RELOADFINISHED, URHO3D_HANDLER(ModelEmitter, HandleEffectReloadFinished));

    ApplyEffect();
    MarkNetworkUpdate();
}

void ModelEmitter::SetNumModels(unsigned num)
{
    // Prevent negative value being assigned from the editor
    if (num > M_MAX_INT)
        num = 0;
    if (num > MAX_BILLBOARDS)
        num = MAX_BILLBOARDS;

    models_.Resize(num);
    SetNumBillboards(num);
}

void ModelEmitter::SetEmitting(bool enable)
{
    if (enable != emitting_)
    {
        emitting_ = enable;
        sendFinishedEvent_ = enable;
        periodTimer_ = 0.0f;
        // Note: network update does not need to be marked as this is a file only attribute
    }
}

void ModelEmitter::SetSerializeModels(bool enable)
{
    serializeModels_ = enable;
    // Note: network update does not need to be marked as this is a file only attribute
}

void ModelEmitter::SetAutoRemoveMode(AutoRemoveMode mode)
{
    autoRemove_ = mode;
    MarkNetworkUpdate();
}

void ModelEmitter::ResetEmissionTimer()
{
    emissionTimer_ = 0.0f;
}

void ModelEmitter::RemoveAllModels()
{
    for (PODVector<Billboard>::Iterator i = billboards_.Begin(); i != billboards_.End(); ++i)
        i->enabled_ = false;

    Commit();
}

void ModelEmitter::Reset()
{
    RemoveAllModels();
    ResetEmissionTimer();
    SetEmitting(true);
}

void ModelEmitter::ApplyEffect()
{
    if (!effect_)
        return;

    SetMaterial(effect_->GetMaterial());
    SetNumModels(effect_->GetNumModels());
    SetRelative(effect_->IsRelative());
    SetScaled(effect_->IsScaled());
    SetSorted(effect_->IsSorted());
    SetFixedScreenSize(effect_->IsFixedScreenSize());
    SetAnimationLodBias(effect_->GetAnimationLodBias());
    SetFaceCameraMode(effect_->GetFaceCameraMode());
}

ModelEffect* ModelEmitter::GetEffect() const
{
    return effect_;
}

void ModelEmitter::SetEffectAttr(const ResourceRef& value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SetEffect(cache->GetResource<ModelEffect>(value.name_));
}

ResourceRef ModelEmitter::GetEffectAttr() const
{
    return GetResourceRef(effect_, ModelEffect::GetTypeStatic());
}

void ModelEmitter::SetModelsAttr(const VariantVector& value)
{
    unsigned index = 0;
    SetNumModels(index < value.Size() ? value[index++].GetUInt() : 0);

    for (PODVector<EmitModel>::Iterator i = models_.Begin(); i != models_.End() && index < value.Size(); ++i)
    {
        i->velocity_ = value[index++].GetVector3();
        i->size_ = value[index++].GetVector2();
        i->timer_ = value[index++].GetFloat();
        i->timeToLive_ = value[index++].GetFloat();
        i->scale_ = value[index++].GetFloat();
        i->rotationSpeed_ = value[index++].GetFloat();
        i->colorIndex_ = (unsigned)value[index++].GetInt();
        i->texIndex_ = (unsigned)value[index++].GetInt();
    }
}

VariantVector ModelEmitter::GetModelsAttr() const
{
    VariantVector ret;
    if (!serializeModels_)
    {
        ret.Push(models_.Size());
        return ret;
    }

    ret.Reserve(models_.Size() * 8 + 1);
    ret.Push(models_.Size());
    for (PODVector<EmitModel>::ConstIterator i = models_.Begin(); i != models_.End(); ++i)
    {
        ret.Push(i->velocity_);
        ret.Push(i->size_);
        ret.Push(i->timer_);
        ret.Push(i->timeToLive_);
        ret.Push(i->scale_);
        ret.Push(i->rotationSpeed_);
        ret.Push(i->colorIndex_);
        ret.Push(i->texIndex_);
    }
    return ret;
}

VariantVector ModelEmitter::GetModelBillboardsAttr() const
{
    VariantVector ret;
    if (!serializeModels_)
    {
        ret.Push(billboards_.Size());
        return ret;
    }

    ret.Reserve(billboards_.Size() * 7 + 1);
    ret.Push(billboards_.Size());

    for (PODVector<Billboard>::ConstIterator i = billboards_.Begin(); i != billboards_.End(); ++i)
    {
        ret.Push(i->position_);
        ret.Push(i->size_);
        ret.Push(Vector4(i->uv_.min_.x_, i->uv_.min_.y_, i->uv_.max_.x_, i->uv_.max_.y_));
        ret.Push(i->color_);
        ret.Push(i->rotation_);
        ret.Push(i->direction_);
        ret.Push(i->enabled_);
    }

    return ret;
}

void ModelEmitter::OnSceneSet(Scene* scene)
{
    BillboardSet::OnSceneSet(scene);

    if (scene && IsEnabledEffective())
        SubscribeToEvent(scene, E_SCENEPOSTUPDATE, URHO3D_HANDLER(ModelEmitter, HandleScenePostUpdate));
    else if (!scene)
         UnsubscribeFromEvent(E_SCENEPOSTUPDATE);
}

bool ModelEmitter::EmitNewModel()
{
    unsigned index = GetFreeModel();
    if (index == M_MAX_UNSIGNED)
        return false;
    assert(index < models_.Size());
    EmitModel& model = models_[index];
    Billboard& billboard = billboards_[index];

    Vector3 startDir;
    Vector3 startPos;

    startDir = effect_->GetRandomDirection();
    startDir.Normalize();

    switch (effect_->GetEmitterType())
    {
    case EMITTER_SPHERE:
        {
            Vector3 dir(
                Random(2.0f) - 1.0f,
                Random(2.0f) - 1.0f,
                Random(2.0f) - 1.0f
            );
            dir.Normalize();
            startPos = effect_->GetEmitterSize() * dir * 0.5f;
        }
        break;

    case EMITTER_BOX:
        {
            const Vector3& emitterSize = effect_->GetEmitterSize();
            startPos = Vector3(
                Random(emitterSize.x_) - emitterSize.x_ * 0.5f,
                Random(emitterSize.y_) - emitterSize.y_ * 0.5f,
                Random(emitterSize.z_) - emitterSize.z_ * 0.5f
            );
        }
        break;
    }

    model.size_ = effect_->GetRandomSize();
    model.timer_ = 0.0f;
    model.timeToLive_ = effect_->GetRandomTimeToLive();
    model.scale_ = 1.0f;
    model.rotationSpeed_ = effect_->GetRandomRotationSpeed();
    model.colorIndex_ = 0;
    model.texIndex_ = 0;

    if (faceCameraMode_ == FC_DIRECTION)
    {
        startPos += startDir * model.size_.y_;
    }

    if (!relative_)
    {
        startPos = node_->GetWorldTransform() * startPos;
        startDir = node_->GetWorldRotation() * startDir;
    };

    model.velocity_ = effect_->GetRandomVelocity() * startDir;

    billboard.position_ = startPos;
    billboard.size_ = models_[index].size_;
    const Vector<TextureFrame>& textureFrames_ = effect_->GetTextureFrames();
    billboard.uv_ = textureFrames_.Size() ? textureFrames_[0].uv_ : Rect::POSITIVE;
    billboard.rotation_ = effect_->GetRandomRotation();
    const Vector<ColorFrame>& colorFrames_ = effect_->GetColorFrames();
    billboard.color_ = colorFrames_.Size() ? colorFrames_[0].color_ : Color();
    billboard.enabled_ = true;
    billboard.direction_ = startDir;

    return true;
}

unsigned ModelEmitter::GetFreeModel() const
{
    for (unsigned i = 0; i < billboards_.Size(); ++i)
    {
        if (!billboards_[i].enabled_)
            return i;
    }

    return M_MAX_UNSIGNED;
}

void ModelEmitter::HandleScenePostUpdate(StringHash eventType, VariantMap& eventData)
{
    // Store scene's timestep and use it instead of global timestep, as time scale may be other than 1
    using namespace ScenePostUpdate;

    lastTimeStep_ = eventData[P_TIMESTEP].GetFloat();

    // If no invisible update, check that the billboardset is in view (framenumber has changed)
    if ((effect_ && effect_->GetUpdateInvisible()) || viewFrameNumber_ != lastUpdateFrameNumber_)
    {
        lastUpdateFrameNumber_ = viewFrameNumber_;
        needUpdate_ = true;
        MarkForUpdate();
    }

    if (node_ && !emitting_ && sendFinishedEvent_)
    {
        // Send finished event only once all billboards are gone
        bool hasEnabledBillboards = false;

        for (unsigned i = 0; i < billboards_.Size(); ++i)
        {
            if (billboards_[i].enabled_)
            {
                hasEnabledBillboards = true;
                break;
            }
        }

        if (!hasEnabledBillboards)
        {
            sendFinishedEvent_ = false;

            // Make a weak pointer to self to check for destruction during event handling
            WeakPtr<ModelEmitter> self(this);

            using namespace ModelEffectFinished;

            VariantMap& eventData = GetEventDataMap();
            eventData[P_NODE] = node_;
            eventData[P_EFFECT] = effect_;

            node_->SendEvent(E_PARTICLEEFFECTFINISHED, eventData);

            if (self.Expired())
                return;

            DoAutoRemove(autoRemove_);
        }
    }
}

void ModelEmitter::HandleEffectReloadFinished(StringHash eventType, VariantMap& eventData)
{
    // When model effect file is live-edited, remove existing models and reapply the effect parameters
    Reset();
    ApplyEffect();
}

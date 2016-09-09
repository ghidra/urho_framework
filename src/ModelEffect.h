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

#pragma once

#include <Urho3D/Graphics/GraphicsDefs.h>
#include <Urho3D/Math/Color.h>
#include <Urho3D/Resource/Resource.h>
#include <Urho3D/Resource/XMLElement.h>

using namespace Urho3D;

/// Model emitter shapes.
enum EmitterType
{
    EMITTER_SPHERE = 0,
    EMITTER_BOX
};

/// %Color animation frame definition.
struct ColorFrame
{
    /// Construct with default values.
    ColorFrame() :
        time_(0.0f)
    {
    }

    /// Construct with a color and zero time.
    ColorFrame(const Color& color) :
        color_(color),
        time_(0.0f)
    {
    }

    /// Construct from a color and time.
    ColorFrame(const Color& color, float time) :
        color_(color),
        time_(time)
    {
    }

    /// Return interpolated value with another color-time pair at the time specified.
    Color Interpolate(const ColorFrame& next, float time) const
    {
        float timeInterval = next.time_ - time_;
        if (timeInterval > 0.0f)
        {
            float t = (time - time_) / timeInterval;
            return color_.Lerp(next.color_, t);
        }
        else
            return next.color_;
    }

    /// Color.
    Color color_;
    /// Time.
    float time_;
};

/// %Texture animation frame definition.
struct TextureFrame
{
    /// Construct with default values.
    TextureFrame() :
        uv_(0.0f, 0.0f, 1.0f, 1.0f),
        time_(0.0f)
    {
    }

    /// UV coordinates.
    Rect uv_;
    /// Time.
    float time_;
};

static const unsigned DEFAULT_NUM_PARTICLES = 10;

namespace Urho3D {
class Material;
class XMLFile;
class XMLElement;
}

/// %Model effect definition.
class URHO3D_API ModelEffect : public Resource
{
    URHO3D_OBJECT(ModelEffect, Resource);

public:
    /// Construct.
    ModelEffect(Context* context);
    /// Destruct.
    virtual ~ModelEffect();
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Load resource from stream. May be called from a worker thread. Return true if successful.
    virtual bool BeginLoad(Deserializer& source);
    /// Finish resource loading. Always called from the main thread. Return true if successful.
    virtual bool EndLoad();
    /// Save resource. Return true if successful.
    virtual bool Save(Serializer& dest) const;

    /// Save resource to XMLElement. Return true if successful.
    bool Save(XMLElement& dest) const;
    /// Load resource from XMLElement synchronously. Return true if successful.
    bool Load(const XMLElement& source);
    /// Set material.
    void SetMaterial(Material* material);
    /// Set maximum number of models.
    void SetNumModels(unsigned num);
    /// Set whether to update when models are not visible.
    void SetUpdateInvisible(bool enable);
    /// Set whether billboards are relative to the scene node.
    void SetRelative(bool enable);
    /// Set whether scene node scale affects billboards' size.
    void SetScaled(bool enable);
    /// Set whether billboards are sorted by distance.
    void SetSorted(bool enable);
    /// Set whether billboards have fixed size on screen (measured in pixels) regardless of distance to camera.
    void SetFixedScreenSize(bool enable);
    /// Set animation LOD bias.
    void SetAnimationLodBias(float lodBias);
    /// Set emitter type.
    void SetEmitterType(EmitterType type);
    /// Set emitter size.
    void SetEmitterSize(const Vector3& size);
    /// Set negative direction limit.
    void SetMinDirection(const Vector3& direction);
    /// Set positive direction limit.
    void SetMaxDirection(const Vector3& direction);
    /// Set constant force acting on models.
    void SetConstantForce(const Vector3& force);
    /// Set model velocity damping force.
    void SetDampingForce(float force);
    /// Set emission active period length (0 = infinite.)
    void SetActiveTime(float time);
    /// Set emission inactive period length (0 = infinite.)
    void SetInactiveTime(float time);
    /// Set minimum emission rate.
    void SetMinEmissionRate(float rate);
    /// Set maximum emission rate.
    void SetMaxEmissionRate(float rate);
    /// Set model minimum size.
    void SetMinModelSize(const Vector2& size);
    /// Set model maximum size.
    void SetMaxModelSize(const Vector2& size);
    /// Set model minimum time to live.
    void SetMinTimeToLive(float time);
    /// Set model maximum time to live.
    void SetMaxTimeToLive(float time);
    /// Set model minimum velocity.
    void SetMinVelocity(float velocity);
    /// Set model maximum velocity.
    void SetMaxVelocity(float velocity);
    /// Set model minimum rotation.
    void SetMinRotation(float rotation);
    /// Set model maximum rotation.
    void SetMaxRotation(float rotation);
    /// Set model minimum rotation speed.
    void SetMinRotationSpeed(float speed);
    /// Set model maximum rotation speed.
    void SetMaxRotationSpeed(float speed);
    /// Set model size additive modifier.
    void SetSizeAdd(float sizeAdd);
    /// Set model size multiplicative modifier.
    void SetSizeMul(float sizeMul);
    /// Set how the models should rotate in relation to the camera. Default is to follow camera rotation on all axes (FC_ROTATE_XYZ.)
    void SetFaceCameraMode(FaceCameraMode mode);

    /// Add a color frame sorted in the correct position based on time.
    void AddColorTime(const Color& color, const float time);
    /// Add a color frame sorted in the correct position based on time.
    void AddColorFrame(const ColorFrame& colorFrame);
    /// Remove color frame at index
    void RemoveColorFrame(unsigned index);
    /// Set color animation of models.
    void SetColorFrames(const Vector<ColorFrame>& colorFrames);
    /// Set color animation frame at index. If index is greater than number of color frames, new color frames are added.
    void SetColorFrame(unsigned index, const ColorFrame& colorFrame);
    /// Set number of color frames.
    void SetNumColorFrames(unsigned number);
    /// Sort the list of color frames based on time.
    void SortColorFrames();

    /// Add a texture frame sorted in the correct position based on time.
    void AddTextureTime(const Rect& uv, const float time);
    /// Add a texture frame sorted in the correct position based on time.
    void AddTextureFrame(const TextureFrame& textureFrame);
    /// Remove texture frame at index
    void RemoveTextureFrame(unsigned index);
    /// Set model texture animation.
    void SetTextureFrames(const Vector<TextureFrame>& animation);
    /// Set number of texture animation frames.
    void SetTextureFrame(unsigned index, const TextureFrame& textureFrame);
    /// Set number of texture frames.
    void SetNumTextureFrames(unsigned number);
    /// Sort the list of texture frames based on time.
    void SortTextureFrames();
    /// Clone the model effect.
    SharedPtr<ModelEffect> Clone(const String& cloneName = String::EMPTY) const;

    /// Return material.
    Material* GetMaterial() const { return material_; }

    /// Return maximum number of models.
    unsigned GetNumModels() const { return numModels_; }

    /// Return whether to update when models are not visible.
    bool GetUpdateInvisible() const { return updateInvisible_; }

    /// Return whether billboards are relative to the scene node.
    bool IsRelative() const { return relative_; }

    /// Return whether scene node scale affects billboards' size.
    bool IsScaled() const { return scaled_; }

    /// Return whether billboards are sorted.
    bool IsSorted() const { return sorted_; }

    /// Return whether billboards are fixed screen size.
    bool IsFixedScreenSize() const { return fixedScreenSize_; }

    /// Return animation Lod bias.
    float GetAnimationLodBias() const { return animationLodBias_; }

    /// Return emitter type.
    EmitterType GetEmitterType() const { return emitterType_; }

    /// Return emitter size.
    const Vector3& GetEmitterSize() const { return emitterSize_; }

    /// Return negative direction limit.
    const Vector3& GetMinDirection() const { return directionMin_; }

    /// Return positive direction limit.
    const Vector3& GetMaxDirection() const { return directionMax_; }

    /// Return constant force acting on models.
    const Vector3& GetConstantForce() const { return constantForce_; }

    /// Return model velocity damping force.
    float GetDampingForce() const { return dampingForce_; }

    /// Return emission active period length (0 = infinite.)
    float GetActiveTime() const { return activeTime_; }

    /// Return emission inactive period length (0 = infinite.)
    float GetInactiveTime() const { return inactiveTime_; }

    /// Return minimum emission rate.
    float GetMinEmissionRate() const { return emissionRateMin_; }

    /// Return maximum emission rate.
    float GetMaxEmissionRate() const { return emissionRateMax_; }

    /// Return model minimum size.
    const Vector2& GetMinModelSize() const { return sizeMin_; }

    /// Return model maximum size.
    const Vector2& GetMaxModelSize() const { return sizeMax_; }

    /// Return model minimum time to live.
    float GetMinTimeToLive() const { return timeToLiveMin_; }

    /// Return model maximum time to live.
    float GetMaxTimeToLive() const { return timeToLiveMax_; }

    /// Return model minimum velocity.
    float GetMinVelocity() const { return velocityMin_; }

    /// Return model maximum velocity.
    float GetMaxVelocity() const { return velocityMax_; }

    /// Return model minimum rotation.
    float GetMinRotation() const { return rotationMin_; }

    /// Return model maximum rotation.
    float GetMaxRotation() const { return rotationMax_; }

    /// Return model minimum rotation speed.
    float GetMinRotationSpeed() const { return rotationSpeedMin_; }

    /// Return model maximum rotation speed.
    float GetMaxRotationSpeed() const { return rotationSpeedMax_; }

    /// Return model size additive modifier.
    float GetSizeAdd() const { return sizeAdd_; }

    /// Return model size multiplicative modifier.
    float GetSizeMul() const { return sizeMul_; }

    /// Return all color animation frames.
    const Vector<ColorFrame>& GetColorFrames() const { return colorFrames_; }

    /// Return number of color animation frames.
    unsigned GetNumColorFrames() const { return colorFrames_.Size(); }

    /// Return a color animation frame, or null if outside range.
    const ColorFrame* GetColorFrame(unsigned index) const;

    /// Return all texture animation frames.
    const Vector<TextureFrame>& GetTextureFrames() const { return textureFrames_; }

    /// Return number of texture animation frames.
    unsigned GetNumTextureFrames() const { return textureFrames_.Size(); }

    /// Return a texture animation frame, or null if outside range.
    const TextureFrame* GetTextureFrame(unsigned index) const;

    /// Return how the models rotate in relation to the camera.
    FaceCameraMode GetFaceCameraMode() const { return faceCameraMode_; }

    /// Return random direction.
    Vector3 GetRandomDirection() const;
    /// Return random size.
    Vector2 GetRandomSize() const;
    /// Return random velocity.
    float GetRandomVelocity() const;
    /// Return random timetolive.
    float GetRandomTimeToLive() const;
    /// Return random rotationspeed.
    float GetRandomRotationSpeed() const;
    /// Return random rotation.
    float GetRandomRotation() const;

private:
    /// Read a float range from an XML element.
    void GetFloatMinMax(const XMLElement& element, float& minValue, float& maxValue);
    /// Read a Vector2 range from an XML element.
    void GetVector2MinMax(const XMLElement& element, Vector2& minValue, Vector2& maxValue);
    /// Read a Vector3 from an XML element.
    void GetVector3MinMax(const XMLElement& element, Vector3& minValue, Vector3& maxValue);

    /// Material.
    SharedPtr<Material> material_;
    /// Number of models.
    unsigned numModels_;
    /// Update when invisible flag.
    bool updateInvisible_;
    /// Billboards relative flag.
    bool relative_;
    /// Scale affects billboard scale flag.
    bool scaled_;
    /// Billboards sorted flag.
    bool sorted_;
    /// Billboards fixed screen size flag.
    bool fixedScreenSize_;
    /// Animation LOD bias.
    float animationLodBias_;
    /// Emitter shape.
    EmitterType emitterType_;
    /// Emitter size.
    Vector3 emitterSize_;
    /// Model direction minimum.
    Vector3 directionMin_;
    /// Model direction maximum.
    Vector3 directionMax_;
    /// Model constant force.
    Vector3 constantForce_;
    /// Model velocity damping force.
    float dampingForce_;
    /// Active period.
    float activeTime_;
    /// Inactive period.
    float inactiveTime_;
    /// Models per second minimum.
    float emissionRateMin_;
    /// Models per second maximum.
    float emissionRateMax_;
    /// Model size minimum.
    Vector2 sizeMin_;
    /// Model size maximum.
    Vector2 sizeMax_;
    /// Model time to live minimum.
    float timeToLiveMin_;
    /// Model time to live maximum.
    float timeToLiveMax_;
    /// Model velocity minimum.
    float velocityMin_;
    /// Model velocity maximum.
    float velocityMax_;
    /// Model rotation angle minimum.
    float rotationMin_;
    /// Model rotation angle maximum.
    float rotationMax_;
    /// Model rotation speed minimum.
    float rotationSpeedMin_;
    /// Model rotation speed maximum.
    float rotationSpeedMax_;
    /// Model size additive parameter.
    float sizeAdd_;
    /// Model size multiplicative parameter.
    float sizeMul_;
    /// Model color animation frames.
    Vector<ColorFrame> colorFrames_;
    /// Texture animation frames.
    Vector<TextureFrame> textureFrames_;
    /// Material name acquired during BeginLoad().
    String loadMaterialName_;
    /// Model rotation mode in relation to the camera.
    FaceCameraMode faceCameraMode_;
};

/// Model effect finished.
URHO3D_EVENT(E_MODELEFFECTFINISHED, ModelEffectFinished)
{
    URHO3D_PARAM(P_NODE, Node);                    // Node pointer
    URHO3D_PARAM(P_EFFECT, Effect);                // ModelEffect pointer
}

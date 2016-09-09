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

#include <Urho3D/Graphics/BillboardSet.h>

class ModelEffect;

/// One model in the model system.
struct EmitModel
{
    /// Velocity.
    Vector3 velocity_;
    /// Original billboard size.
    Vector2 size_;
    /// Time elapsed from creation.
    float timer_;
    /// Lifetime.
    float timeToLive_;
    /// Size scaling value.
    float scale_;
    /// Rotation speed.
    float rotationSpeed_;
    /// Current color animation index.
    unsigned colorIndex_;
    /// Current texture animation index.
    unsigned texIndex_;
};

/// %Model emitter component.
class URHO3D_API ModelEmitter : public BillboardSet
{
    URHO3D_OBJECT(ModelEmitter, BillboardSet);

public:
    /// Construct.
    ModelEmitter(Context* context);
    /// Destruct.
    virtual ~ModelEmitter();
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Handle enabled/disabled state change.
    virtual void OnSetEnabled();
    /// Update before octree reinsertion. Is called from a worker thread.
    virtual void Update(const FrameInfo& frame);

    /// Set model effect.
    void SetEffect(ModelEffect* effect);
    /// Set maximum number of models.
    void SetNumModels(unsigned num);
    /// Set whether should be emitting. If the state was changed, also resets the emission period timer.
    void SetEmitting(bool enable);
    /// Set whether models should be serialized. Default true, set false to reduce scene file size.
    void SetSerializeModels(bool enable);
    //// Set to remove either the emitter component or its owner node from the scene automatically on model effect completion. Disabled by default.
    void SetAutoRemoveMode(AutoRemoveMode mode);
    /// Reset the emission period timer.
    void ResetEmissionTimer();
    /// Remove all current models.
    void RemoveAllModels();
    /// Reset the model emitter completely. Removes current models, sets emitting state on, and resets the emission timer.
    void Reset();
    /// Apply not continuously updated values such as the material, the number of models and sorting mode from the model effect. Call this if you change the effect programmatically.
    void ApplyEffect();

    /// Return model effect.
    ModelEffect* GetEffect() const;

    /// Return maximum number of models.
    unsigned GetNumModels() const { return models_.Size(); }

    /// Return whether is currently emitting.
    bool IsEmitting() const { return emitting_; }

    /// Return whether models are to be serialized.
    bool GetSerializeModels() const { return serializeModels_; }

    /// Return automatic removal mode on model effect completion.
    AutoRemoveMode GetAutoRemoveMode() const { return autoRemove_; }

    /// Set models effect attribute.
    void SetEffectAttr(const ResourceRef& value);
    /// Set models effect attribute.
    ResourceRef GetEffectAttr() const;
    /// Set models attribute.
    void SetModelsAttr(const VariantVector& value);
    /// Return models attribute. Returns model amount only if models are not to be serialized.
    VariantVector GetModelsAttr() const;
    /// Return billboards attribute. Returns billboard amount only if models are not to be serialized.
    VariantVector GetModelBillboardsAttr() const;

protected:
    /// Handle scene being assigned.
    virtual void OnSceneSet(Scene* scene);

    /// Create a new model. Return true if there was room.
    bool EmitNewModel();
    /// Return a free model index.
    unsigned GetFreeModel() const;

private:
    /// Handle scene post-update event.
    void HandleScenePostUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle live reload of the model effect.
    void HandleEffectReloadFinished(StringHash eventType, VariantMap& eventData);

    /// Model effect.
    SharedPtr<ModelEffect> effect_;
    /// Models.
    PODVector<EmitModel> models_;
    /// Active/inactive period timer.
    float periodTimer_;
    /// New model emission timer.
    float emissionTimer_;
    /// Last scene timestep.
    float lastTimeStep_;
    /// Rendering framenumber on which was last updated.
    unsigned lastUpdateFrameNumber_;
    /// Currently emitting flag.
    bool emitting_;
    /// Need update flag.
    bool needUpdate_;
    /// Serialize models flag.
    bool serializeModels_;
    /// Ready to send effect finish event flag.
    bool sendFinishedEvent_;
    /// Automatic removal mode.
    AutoRemoveMode autoRemove_;
};

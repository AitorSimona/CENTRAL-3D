#ifndef __BROKENENGINE_H__
#define __BROKENENGINE_H__

// ---Modules----------------------
#include "Module.h"
#include "ModuleEventManager.h"
#include "ModuleInput.h"
#include "ModuleTimeManager.h"
#include "ModuleHardware.h"
#include "ModuleFileSystem.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"
#include "ModuleScripting.h"
#include "ModuleThreading.h"
#include "ModuleGui.h"
#include "ModuleAudio.h"
#include "ModuleParticles.h"
#include "ModulePhysics.h"
#include "ModuleUI.h"
#include "ModuleSelection.h"
// --------------------------------

// ---Panel------------------------
#include "Panel.h"
// --------------------------------

// ---GameObject && Components-----
#include "Particle.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ComponentMeshRenderer.h"
#include "ComponentScript.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ComponentAudioListener.h"
#include "ComponentAudioSource.h"
#include "ComponentBone.h"
#include "ComponentCollider.h"
#include "ComponentDynamicRigidBody.h"
#include "ComponentParticleEmitter.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "ComponentText.h"
#include "ComponentProgressBar.h"
#include "ComponentLight.h"
// --------------------------------

// ---Importers and Resources------
#include "JSONLoader.h"

#include "Importer.h"
#include "ImporterFolder.h"
#include "ImporterScene.h"
#include "ImporterModel.h"
#include "ImporterPrefab.h"
#include "ImporterMaterial.h"
#include "ImporterShader.h"
#include "ImporterMesh.h"
#include "ImporterTexture.h"
#include "ImporterScript.h"
#include "ImporterMeta.h"
#include "ImporterAnimation.h"
#include "ImporterBone.h"

#include "Resource.h"
#include "ResourceFolder.h"
#include "ResourceModel.h"
#include "ResourcePrefab.h"
#include "ResourceScene.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "ResourceScript.h"
#include "ResourceMeta.h"
#include "ResourceAnimation.h"
#include "ResourceBone.h"
// --------------------------------

// ---Others-----------------------
#include "Light.h"
#include "RandomGenerator.h"
// --------------------------------

// ---Application------------------
#include "Application.h"
// --------------------------------

// ---Logging----------------------
#include "EngineLog.h"
// --------------------------------
//
//// ---Entry Point------------------
//#include "EntryPoint.h"
//// --------------------------------

#endif
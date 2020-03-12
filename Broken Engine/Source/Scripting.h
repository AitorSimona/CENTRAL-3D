//bool IsMouseInGame() const;
//int GetMouseRaycastHit(lua_State *L);

//// GameObjects
//GameObject* FindGameObject(const char* objName) const;
//uint32_t FindGameObjectUID(const char* objName) const;

//GameObject* Instantiate(GameObject* reference, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale);
//uint32_t InstantiateByUID(uint32_t objUID, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale);
//GameObject* InstantiateByName(const char* objName, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale);

//GameObject* Destroy(GameObject* target);
//void DestroyByUID(uint32_t objUID);

//// SCRIPT TRANSLATOR
//void Activate(bool state);
//bool IsActivated() const;

//// OBJECT TRANSLATOR
//// General
//const GameObject* GetGameObject() const;
//const char* GetObjectName() const;

//void ActivateObject(bool state);
//bool IsObjectActivated() const;

//void SetStatic(bool state);
//bool IsStatic() const;
//void DestroySelf() const;

//// Rotation
//float GetEulerX(bool local) const;	// Roll
//float GetEulerY(bool local) const;	// Pitch
//float GetEulerZ(bool local) const;	// Yaw
//int GetEulerRotation(bool local, lua_State *L) const;
//
//void RotateByEuler(float x, float y, float z, bool local);
//void SetEulerRotation(float x, float y, float z, bool local);

////DON'T USE, quat methods need revision
//float GetQuatX(bool local) const;
//float GetQuatY(bool local) const;
//float GetQuatZ(bool local) const;
//float GetQuatW(bool local) const;
//int GetQuatRotation(bool local, lua_State *L) const;

//void RotateByQuat(float x, float y, float z, float w, bool local);
//void SetQuatRotation(float x, float y, float z, float w, bool local);

//// Others
//void LookAt(float posX, float posY, float posZ, bool local);
//void LookTo(float dirX, float dirY, float dirZ, bool local);
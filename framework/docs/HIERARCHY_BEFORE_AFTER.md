# Step 1: hierarchy before / after

## include/Model.h

```diff
--- before
+++ after
@@ -107,6 +107,7 @@
 using Model = Polyhedron;
 
 struct Component_Poly {
+    std::optional<EntityId> parent; // Local SRT is relative to this entity.
     Polyhedron polyhedron;
     Transform3D transform;
     glm::vec3 sideLengths{1.0f};
```

## include/Graphics.h

```diff
--- before
+++ after
@@ -43,6 +43,9 @@
     bool setPosition(EntityId id, const glm::vec3& worldPosition);
     bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
     bool setTransform(EntityId id, const Transform3D& transform);
+    bool setParent(EntityId id, std::optional<EntityId> parent);
+    std::optional<glm::mat4> worldMatrix(EntityId id) const;
+    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
     bool setFaceTransform(EntityId id, std::size_t faceIndex,
                           const Transform3D& transform, bool independence);
 
```

## detail/Scene.inl

```diff
--- before
+++ after
@@ -151,6 +151,7 @@
 }
 
 inline bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
+    // For a parented entity this position is in parent coordinates.
     if (id >= components_.size() || !finiteVector(worldPosition)) {
         return false;
     }
@@ -174,6 +175,43 @@
         recalculateNormals(face);
     }
     return setModel(id,replacement);
+}
+
+inline bool Scene::setParent(EntityId id, std::optional<EntityId> parent) {
+    if (id >= components_.size()) return false;
+    auto ancestor = parent;
+    std::size_t remaining = components_.size();
+    while (ancestor) {
+        if (*ancestor >= components_.size() || *ancestor == id || remaining-- == 0) return false;
+        ancestor = components_[*ancestor].parent;
+    }
+    components_[id].parent = parent;
+    return true;
+}
+inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
+    if (id >= components_.size()) return {};
+    glm::mat4 result(1);
+    std::optional<EntityId> current = id;
+    std::size_t remaining = components_.size();
+    while (current) {
+        if (*current >= components_.size() || remaining-- == 0) return {};
+        const auto& component = components_[*current];
+        result = detail::transformMatrix(component.transform) * result;
+        if (!detail::finiteMatrix(result)) return {};
+        current = component.parent;
+    }
+    return result;
+}
+inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
+    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
+    const auto& component = components_[id];
+    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
+    if (component.parent) {
+        const auto parent = worldMatrix(*component.parent);
+        if (!parent) return {};
+        result = *parent * result;
+    }
+    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
 }
 
 inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

## detail/Renderer.inl

```diff
--- before
+++ after
@@ -151,10 +151,10 @@
         glm::mat4 matrix(1);
         if(range.lineId) {
             const auto& line=scene.getLines()[*range.lineId];
-            if(line.owner) matrix=DiagramSystem::modelMatrix(scene.getComponent(*line.owner)->transform);
+            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
         } else {
             const auto* c=scene.getComponent(range.entityId);
-            matrix=DiagramSystem::faceModelMatrix(*c,c->polyhedron.faces[range.faceIndex]);
+            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
         }
         if(!detail::finiteMatrix(matrix)||!detail::finiteMatrix(projection*view*matrix))return false;
     }
@@ -167,11 +167,11 @@
         glm::mat4 model(1);const Mesh* face=nullptr;
         if(range.lineId) {
             const auto& line=scene.getLines()[*range.lineId];
-            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;model=DiagramSystem::modelMatrix(owner->transform);}
+            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
         } else {
             const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
             face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;
-            model=DiagramSystem::faceModelMatrix(*c,*face);
+            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;model=*world;
         }
         const auto mvp=projection*view*model;
         const float determinant=glm::determinant(glm::mat3(model));
```

## include/Picking.h

```diff
--- before
+++ after
@@ -44,7 +44,8 @@
         const auto* component=scene.getComponent(id);if(!component||!component->visible)continue;
         for(std::size_t f=0;f<component->polyhedron.faces.size();++f) {
             const auto& face=component->polyhedron.faces[f];if(!face.visible)continue;
-            const auto matrix=DiagramSystem::faceModelMatrix(*component,face);
+            const auto world=scene.faceWorldMatrix(id,f);if(!world)continue;
+            const auto matrix=*world;
             const auto indices=detail::triangles(face);
             for(std::size_t t=0;t<indices.size();t+=3) {
                 const auto ia=indices[t],ib=indices[t+1],ic=indices[t+2];
```


# Step 2: UV before / after

## include/Geometry.h

```diff
--- before
+++ after
@@ -6,6 +6,116 @@
 #include <stdexcept>
 
 namespace graphics {
+enum class UVMapping { Planar, Box, Cylindrical, Spherical };
+struct UVOptions {
+    UVMapping mapping = UVMapping::Planar;
+    int axis = 2; // Planar projection drops X=0, Y=1, or Z=2.
+    glm::vec2 scale{1};
+    glm::vec2 offset{0};
+    float rotationDegrees = 0;
+};
+
+inline bool transformTexcoords(Mesh& mesh, const glm::vec2& scale,
+                              const glm::vec2& offset, float rotationDegrees = 0) {
+    if (mesh.texcoords.size() != mesh.localVertices.size() || mesh.texcoords.empty()
+        || !std::isfinite(scale.x) || !std::isfinite(scale.y)
+        || !std::isfinite(offset.x) || !std::isfinite(offset.y)
+        || !std::isfinite(rotationDegrees)) return false;
+    auto uvs = mesh.texcoords;
+    const double angle = glm::radians(double(rotationDegrees));
+    const double c = std::cos(angle), s = std::sin(angle);
+    for (auto& uv : uvs) {
+        const auto p = glm::dvec2(uv) * glm::dvec2(scale);
+        uv = glm::vec2(c*p.x-s*p.y, s*p.x+c*p.y) + offset;
+        if (!std::isfinite(uv.x) || !std::isfinite(uv.y)) return false;
+    }
+    mesh.texcoords = std::move(uvs);
+    return true;
+}
+
+inline bool generateTexcoords(Mesh& mesh, const UVOptions& options = UVOptions{}) {
+    if (!detail::validMesh(mesh) || options.axis < 0 || options.axis > 2) return false;
+    switch (options.mapping) {
+    case UVMapping::Planar: case UVMapping::Box:
+    case UVMapping::Cylindrical: case UVMapping::Spherical: break;
+    default: return false;
+    }
+    Mesh result = mesh;
+    glm::dvec3 low(mesh.localVertices[0]), high(low);
+    for (const auto& p : mesh.localVertices) {
+        low = glm::min(low, glm::dvec3(p)); high = glm::max(high, glm::dvec3(p));
+    }
+    const auto size = high-low, center = (low+high)*0.5;
+    const double pi = std::acos(-1.0);
+    auto project = [&](std::uint32_t index, int axis) {
+        const auto p = glm::dvec3(mesh.localVertices[index]);
+        if (options.mapping == UVMapping::Planar || options.mapping == UVMapping::Box) {
+            const int u = axis == 0 ? 2 : 0, v = axis == 1 ? 2 : 1;
+            return glm::vec2(size[u] > 0 ? (p[u]-low[u])/size[u] : 0.5,
+                             size[v] > 0 ? (p[v]-low[v])/size[v] : 0.5);
+        }
+        const auto d = p-center;
+        const double radius = glm::length(d);
+        return glm::vec2(std::atan2(d.z,d.x)/(2*pi)+0.5,
+            options.mapping == UVMapping::Spherical
+                ? (radius > 0 ? std::asin(std::clamp(d.y/radius,-1.0,1.0))/pi+0.5 : 0.5)
+                : (size.y > 0 ? (p.y-low.y)/size.y : 0.5));
+    };
+    if (options.mapping == UVMapping::Planar) {
+        result.texcoords.clear();
+        for (std::uint32_t i=0;i<mesh.localVertices.size();++i) result.texcoords.push_back(project(i,options.axis));
+    } else {
+        const auto indices = detail::triangles(mesh);
+        if (indices.size() > std::numeric_limits<std::uint32_t>::max()) return false;
+        result.localVertices.clear(); result.vertexNormals.clear(); result.vertexColors.clear();
+        result.texcoords.clear(); result.triangleIndices.clear(); result.outlineIndices.clear();
+        result.shape = GLFace::Mesh;
+        std::vector<std::uint32_t> first(mesh.localVertices.size(),std::numeric_limits<std::uint32_t>::max());
+        for (std::size_t t=0;t<indices.size();t+=3) {
+            const auto a=glm::dvec3(mesh.localVertices[indices[t]]);
+            const auto b=glm::dvec3(mesh.localVertices[indices[t+1]]);
+            const auto d=glm::dvec3(mesh.localVertices[indices[t+2]]);
+            const auto normal=glm::abs(glm::cross(b-a,d-a));
+            const int axis=normal.x>normal.y ? (normal.x>normal.z?0:2) : (normal.y>normal.z?1:2);
+            std::array<glm::vec2,3> uv{project(indices[t],axis),project(indices[t+1],axis),project(indices[t+2],axis)};
+            if (options.mapping != UVMapping::Box) {
+                // Pole U is taken from neighboring corners, where longitude is defined.
+                for (int k=0;k<3;++k) {
+                    const auto p=glm::dvec3(mesh.localVertices[indices[t+k]])-center;
+                    if (p.x==0 && p.z==0) {
+                        const auto u=uv[(k+1)%3].x, v=uv[(k+2)%3].x;
+                        uv[k].x=(u+v+(std::abs(u-v)>0.5f?1.0f:0.0f))*0.5f;
+                    }
+                }
+                const float lo=std::min({uv[0].x,uv[1].x,uv[2].x});
+                const float hi=std::max({uv[0].x,uv[1].x,uv[2].x});
+                if (hi-lo>0.5f) for (auto& p:uv) if (p.x<0.5f) p.x+=1;
+            }
+            for (int k=0;k<3;++k) {
+                const auto source=indices[t+k], target=std::uint32_t(result.localVertices.size());
+                if(first[source]==std::numeric_limits<std::uint32_t>::max()) first[source]=target;
+                result.localVertices.push_back(mesh.localVertices[source]);
+                if(!mesh.vertexNormals.empty()) result.vertexNormals.push_back(mesh.vertexNormals[source]);
+                if(!mesh.vertexColors.empty()) result.vertexColors.push_back(mesh.vertexColors[source]);
+                result.texcoords.push_back(uv[k]); result.triangleIndices.push_back(target);
+            }
+        }
+        for(auto index:mesh.outlineIndices) {
+            if(first[index]==std::numeric_limits<std::uint32_t>::max()) {
+                first[index]=std::uint32_t(result.localVertices.size());
+                result.localVertices.push_back(mesh.localVertices[index]);
+                if(!mesh.vertexNormals.empty()) result.vertexNormals.push_back(mesh.vertexNormals[index]);
+                if(!mesh.vertexColors.empty()) result.vertexColors.push_back(mesh.vertexColors[index]);
+                result.texcoords.push_back(project(index,options.axis));
+            }
+            result.outlineIndices.push_back(first[index]);
+        }
+    }
+    if (!transformTexcoords(result,options.scale,options.offset,options.rotationDegrees)
+        || !detail::validMesh(result)) return false;
+    mesh=std::move(result); return true;
+}
+
 inline void recalculateNormals(Mesh& mesh) {
     mesh.vertexNormals.assign(mesh.localVertices.size(), glm::vec3(0));
     const auto indices = detail::triangles(mesh);
```

## include/Graphics.h

```diff
--- before
+++ after
@@ -62,6 +62,9 @@
                            std::size_t vertexIndex, const glm::vec3& position);
     bool setTriangleIndices(EntityId id, std::size_t meshIndex,
                             const std::vector<std::uint32_t>& indices);
+    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});
+    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale,
+                            const glm::vec2& offset, float rotationDegrees = 0);
     bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);
     bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);
     bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);
```

## detail/Scene.inl

```diff
--- before
+++ after
@@ -514,6 +514,17 @@
     if(!detail::validMesh(replacement))return false;
     recalculateNormals(replacement);return setMesh(id,meshIndex,replacement);
 }
+inline bool Scene::generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options) {
+    const auto* mesh=getMesh(id,meshIndex); if(!mesh)return false;
+    auto replacement=*mesh;
+    return graphics::generateTexcoords(replacement,options) && setMesh(id,meshIndex,replacement);
+}
+inline bool Scene::transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale,
+                                     const glm::vec2& offset, float rotationDegrees) {
+    const auto* mesh=getMesh(id,meshIndex); if(!mesh)return false;
+    auto replacement=*mesh;
+    return graphics::transformTexcoords(replacement,scale,offset,rotationDegrees) && setMesh(id,meshIndex,replacement);
+}
 inline bool Scene::setTexcoords(EntityId id,std::size_t meshIndex,const std::vector<glm::vec2>& uvs) {
     const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
     auto replacement=*mesh;replacement.texcoords=uvs;return setMesh(id,meshIndex,replacement);
```


#pragma once
#include <algorithm>
#include <cmath>

namespace graphics {
inline bool Bounds3::valid() const {
    return detail::finiteVector(minimum) && detail::finiteVector(maximum)
        && maximum.x>minimum.x && maximum.y>minimum.y && maximum.z>minimum.z;
}
inline bool Bounds3::contains(const glm::vec3& p,const glm::vec3& margin) const {
    return valid() && detail::finiteVector(p) && detail::finiteVector(margin)
        && margin.x>=0 && margin.y>=0 && margin.z>=0
        && p.x>=minimum.x+margin.x && p.x<=maximum.x-margin.x
        && p.y>=minimum.y+margin.y && p.y<=maximum.y-margin.y
        && p.z>=minimum.z+margin.z && p.z<=maximum.z-margin.z;
}
inline bool Bounds3::overlaps(const Bounds3& b) const {
    return valid() && b.valid() && minimum.x<b.maximum.x && maximum.x>b.minimum.x
        && minimum.y<b.maximum.y && maximum.y>b.minimum.y && minimum.z<b.maximum.z && maximum.z>b.minimum.z;
}
inline bool DistancePath::setPoints(const std::vector<glm::vec3>& points,bool closed) {
    if(points.size()<2) return Diagnostics::fail(PipelineStage::Motion,"DistancePath::setPoints","Need at least two points.");
    auto candidate=points;
    for(const auto& p:candidate) if(!detail::finiteVector(p))
        return Diagnostics::fail(PipelineStage::Motion,"DistancePath::setPoints","Path contains NaN/Inf.");
    if(closed && glm::length(glm::dvec3(candidate.front())-glm::dvec3(candidate.back()))>1e-10) candidate.push_back(candidate.front());
    std::vector<double> distances{0};
    for(std::size_t i=1;i<candidate.size();++i) {
        const double segment=glm::length(glm::dvec3(candidate[i])-glm::dvec3(candidate[i-1]));
        if(!std::isfinite(segment) || segment<=1e-10 || !std::isfinite(distances.back()+segment))
            return Diagnostics::fail(PipelineStage::Motion,"DistancePath::setPoints","Zero-length, repeated or overflowing path segment.");
        distances.push_back(distances.back()+segment);
    }
    points_=std::move(candidate);distances_=std::move(distances);return true;
}
inline std::optional<PathPose> DistancePath::sample(double distance,Playback playback) const {
    const double total=length();
    if(total<=0 || !std::isfinite(distance)) {
        Diagnostics::fail(PipelineStage::Motion,"DistancePath::sample","Configure a nonempty path and finite distance.");return {};
    }
    double sign=1;
    if(playback==Playback::Once) distance=std::clamp(distance,0.0,total);
    else if(playback==Playback::Loop) { distance=std::fmod(distance,total);if(distance<0) distance+=total; }
    else if(playback==Playback::PingPong) {
        distance=std::fmod(distance,2*total);if(distance<0) distance+=2*total;
        if(distance>total) {distance=2*total-distance;sign=-1;}
    } else { Diagnostics::fail(PipelineStage::Motion,"DistancePath::sample","Invalid Playback value.");return {}; }
    const auto end=std::upper_bound(distances_.begin(),distances_.end(),distance);
    const std::size_t i=end==distances_.end()?distances_.size()-1:std::size_t(end-distances_.begin());
    const float weight=float((distance-distances_[i-1])/(distances_[i]-distances_[i-1]));
    PathPose pose;pose.position=glm::mix(points_[i-1],points_[i],weight);
    pose.tangent=glm::vec3(glm::normalize(glm::dvec3(points_[i])-glm::dvec3(points_[i-1]))*sign);
    pose.yawDegrees=glm::degrees(std::atan2(pose.tangent.x,pose.tangent.z));return pose;
}
inline bool AnimationClock::advance(double dt) {
    if(!std::isfinite(dt) || dt<0 || !std::isfinite(seconds) || seconds<0 || !std::isfinite(seconds+dt))
        return Diagnostics::fail(PipelineStage::Motion,"AnimationClock::advance","Time must be finite, nonnegative and within range.");
    if(playing) seconds+=dt;return true;
}
namespace detail {
inline bool rigidRoom(const glm::mat4& matrix) {
    const auto r=glm::mat3(matrix), identity=glm::transpose(r)*r;
    return TransformSystem::validAffine(matrix) && std::abs(glm::determinant(r)-1)<.001f
        && glm::length(identity[0]-glm::vec3(1,0,0))<.001f
        && glm::length(identity[1]-glm::vec3(0,1,0))<.001f
        && glm::length(identity[2]-glm::vec3(0,0,1))<.001f;
}
inline bool horizontalOverlap(const CharacterBody& b,const glm::vec3& feet,const Bounds3& box) {
    return feet.x+b.halfWidth.x>box.minimum.x && feet.x-b.halfWidth.x<box.maximum.x
        && feet.z+b.halfWidth.y>box.minimum.z && feet.z-b.halfWidth.y<box.maximum.z;
}
}
inline bool CollisionSystem::stepRoomBody(RoomBody& body,const Bounds3& room,const glm::mat4& roomWorld,
        float dt,bool floorOpen,bool elastic,const glm::vec3& gravity,const glm::vec3& angularVelocity) {
    if(!room.valid() || !detail::rigidRoom(roomWorld) || !detail::finiteVector(gravity)
        || !detail::finiteVector(angularVelocity) || !detail::finiteVector(body.localPosition)
        || !detail::finiteVector(body.localVelocity) || !detail::positiveLengths(body.halfExtent)
        || !detail::finiteVector(body.worldPosition) || !detail::finiteVector(body.worldVelocity)
        || !std::isfinite(dt) || dt<0 || dt>.25f) return Diagnostics::fail(PipelineStage::Motion,"CollisionSystem::stepRoomBody",
            "Need a rigid room transform, finite body state, positive half extents and dt in [0,.25].");
    const auto low=room.minimum+body.halfExtent, high=room.maximum-body.halfExtent;
    if(low.x>=high.x || low.y>=high.y || low.z>=high.z)
        return Diagnostics::fail(PipelineStage::Motion,"CollisionSystem::stepRoomBody","Body does not fit inside the room.");
    auto next=body;
    const int steps=std::max(1,int(std::ceil(dt*120)));const float h=dt/steps;
    const auto rotation=glm::mat3(roomWorld);
    const auto localGravity=glm::transpose(rotation)*gravity;
    for(int s=0;s<steps;++s) {
        if(next.released) {next.worldVelocity+=gravity*h;next.worldPosition+=next.worldVelocity*h;continue;}
        if(!elastic) next.localVelocity+=localGravity*h;
        next.localPosition+=next.localVelocity*h;
        if(floorOpen && next.localPosition.y<low.y) {
            next.released=true;next.worldPosition=glm::vec3(roomWorld*glm::vec4(next.localPosition,1));
            next.worldVelocity=rotation*next.localVelocity+glm::cross(angularVelocity,next.worldPosition-glm::vec3(roomWorld[3]));continue;
        }
        for(int axis=0;axis<3;++axis) {
            if(elastic && !(floorOpen && axis==1)) {
                const double width=double(high[axis])-low[axis];
                const double p=double(next.localPosition[axis])-low[axis];
                double folded=std::fmod(p,2*width);if(folded<0) folded+=2*width;
                const bool reflected=folded>width;
                next.localPosition[axis]=float(low[axis]+(reflected?2*width-folded:folded));
                if(reflected) next.localVelocity[axis]=-next.localVelocity[axis];
                if(next.localPosition[axis]<=low[axis]) next.localVelocity[axis]=std::abs(next.localVelocity[axis]);
                if(next.localPosition[axis]>=high[axis]) next.localVelocity[axis]=-std::abs(next.localVelocity[axis]);
            } else {
                if(next.localPosition[axis]<low[axis] && !(floorOpen && axis==1)) {
                    next.localPosition[axis]=low[axis];next.localVelocity[axis]=elastic?std::abs(next.localVelocity[axis]):std::max(0.0f,next.localVelocity[axis]);
                }
                if(next.localPosition[axis]>high[axis]) {
                    next.localPosition[axis]=high[axis];next.localVelocity[axis]=elastic?-std::abs(next.localVelocity[axis]):std::min(0.0f,next.localVelocity[axis]);
                }
            }
        }
    }
    if(!detail::finiteVector(next.localPosition) || !detail::finiteVector(next.localVelocity)
        || !detail::finiteVector(next.worldPosition) || !detail::finiteVector(next.worldVelocity))
        return Diagnostics::fail(PipelineStage::Motion,"CollisionSystem::stepRoomBody","Simulation overflow; original state was preserved.");
    body=next;return true;
}
inline bool CollisionSystem::jump(CharacterBody& body,float speed) {
    if(!std::isfinite(speed) || speed<=0) return Diagnostics::fail(PipelineStage::Motion,"CollisionSystem::jump","Jump speed must be positive and finite.");
    if(!body.grounded) return false; // Normal input rejection, not an API error.
    body.verticalSpeed=speed;body.grounded=false;return true;
}
inline std::optional<CharacterStep> CollisionSystem::stepCharacter(CharacterBody& body,const Bounds3& stage,
        const std::vector<Bounds3>& obstacles,float dt,float gravity) {
    if(!stage.valid() || !detail::finiteVector(body.feet) || !detail::finiteVector(body.direction)
        || !std::isfinite(body.speed) || body.speed<0 || !std::isfinite(body.height) || body.height<=0
        || !std::isfinite(body.halfWidth.x) || !std::isfinite(body.halfWidth.y) || body.halfWidth.x<=0 || body.halfWidth.y<=0
        || !std::isfinite(body.verticalSpeed) || !std::isfinite(body.walkDistance)
        || !std::isfinite(dt) || dt<0 || dt>.25f || !std::isfinite(gravity) || gravity<=0) {
        Diagnostics::fail(PipelineStage::Motion,"CollisionSystem::stepCharacter","Invalid bounds, body dimensions, speed, gravity or dt.");return {};
    }
    if(stage.maximum.x-stage.minimum.x<=2*body.halfWidth.x || stage.maximum.z-stage.minimum.z<=2*body.halfWidth.y) {
        Diagnostics::fail(PipelineStage::Motion,"CollisionSystem::stepCharacter","Character does not fit on the stage.");return {};
    }
    float spacing=std::min(body.halfWidth.x,body.halfWidth.y)*.5f;
    for(const auto& box:obstacles) {
        if(!box.valid()) {Diagnostics::fail(PipelineStage::Motion,"CollisionSystem::stepCharacter","Invalid obstacle bounds.");return {};}
        spacing=std::min(spacing,std::min(box.maximum.x-box.minimum.x,box.maximum.z-box.minimum.z)*.5f);
    }
    const double count=std::ceil(std::max(double(dt)*120,double(body.speed)*dt/std::max(double(spacing),1e-9)));
    if(count>4096) {Diagnostics::fail(PipelineStage::Motion,"CollisionSystem::stepCharacter","Motion exceeds the substep budget; reduce speed or dt.");return {};}
    auto next=body;CharacterStep event;next.direction.y=0;
    const double directionLength=glm::length(glm::dvec3(next.direction));
    next.direction=directionLength>1e-10?glm::vec3(glm::dvec3(next.direction)/directionLength):glm::vec3(0);
    const int steps=std::max(1,int(count));const float h=dt/steps;
    for(int i=0;i<steps;++i) {
        const auto previous=next.feet;auto candidate=previous+next.direction*(next.speed*h);
        bool blocked=candidate.x-next.halfWidth.x<stage.minimum.x || candidate.x+next.halfWidth.x>stage.maximum.x
            || candidate.z-next.halfWidth.y<stage.minimum.z || candidate.z+next.halfWidth.y>stage.maximum.z;
        for(const auto& box:obstacles) if(previous.y<box.maximum.y-1e-4f && previous.y+next.height>box.minimum.y+1e-4f
            && detail::horizontalOverlap(next,candidate,box)) blocked=true;
        if(blocked) {next.direction=-next.direction;candidate=previous;event.blocked=event.turned=true;}
        next.feet=candidate;next.walkDistance+=glm::length(glm::dvec2(candidate.x-previous.x,candidate.z-previous.z));
        bool supported=std::abs(next.feet.y-stage.minimum.y)<1e-4f;
        for(const auto& box:obstacles) if(detail::horizontalOverlap(next,next.feet,box) && std::abs(next.feet.y-box.maximum.y)<1e-4f) supported=true;
        if(next.grounded && !supported) next.grounded=false;
        if(!next.grounded) {
            const float oldY=next.feet.y;next.verticalSpeed-=gravity*h;float newY=oldY+next.verticalSpeed*h;
            float landing=stage.minimum.y;
            for(const auto& box:obstacles) if(detail::horizontalOverlap(next,next.feet,box)) {
                if(next.verticalSpeed<=0 && oldY>=box.maximum.y-1e-4f && newY<=box.maximum.y) landing=std::max(landing,box.maximum.y);
                if(next.verticalSpeed>0 && oldY+next.height<=box.minimum.y && newY+next.height>=box.minimum.y) {
                    newY=box.minimum.y-next.height;next.verticalSpeed=0;
                }
            }
            if(next.verticalSpeed<=0 && newY<=landing && oldY>=landing-1e-4f) {
                newY=landing;next.verticalSpeed=0;next.grounded=true;event.landed=true;
            }
            next.feet.y=newY;
        }
    }
    if(!detail::finiteVector(next.feet) || !std::isfinite(next.walkDistance) || !std::isfinite(next.verticalSpeed)) {
        Diagnostics::fail(PipelineStage::Motion,"CollisionSystem::stepCharacter","Simulation overflow; original state was preserved.");return {};
    }
    body=next;return event;
}
inline std::optional<Bounds3> CollisionSystem::randomFreeBox(const Bounds3& area,const glm::vec3& size,
        const std::vector<Bounds3>& occupied,std::mt19937& random,unsigned attempts) {
    if(!area.valid() || !detail::positiveLengths(size) || attempts==0 || attempts>100000
        || size.x>area.maximum.x-area.minimum.x || size.y>area.maximum.y-area.minimum.y || size.z>area.maximum.z-area.minimum.z) {
        Diagnostics::fail(PipelineStage::Scene,"CollisionSystem::randomFreeBox","Box must fit in the area; attempts must be in [1,100000].");return {};
    }
    for(const auto& box:occupied) if(!box.valid()) {Diagnostics::fail(PipelineStage::Scene,"CollisionSystem::randomFreeBox","Invalid occupied bounds.");return {};}
    for(unsigned i=0;i<attempts;++i) {
        Bounds3 box;
        for(int axis=0;axis<3;++axis) box.minimum[axis]=std::uniform_real_distribution<float>(area.minimum[axis],area.maximum[axis]-size[axis])(random);
        box.maximum=box.minimum+size;
        if(std::none_of(occupied.begin(),occupied.end(),[&](const Bounds3& other){return box.overlaps(other);})) return box;
    }
    Diagnostics::fail(PipelineStage::Scene,"CollisionSystem::randomFreeBox","No free placement within the attempt budget.","Use smaller objects, fewer occupied regions or a larger placement area.");return {};
}
inline std::optional<EntityId> PatternSystem::addBox(Scene& scene,const glm::vec3& size,const glm::vec3& position,
        GlColor color,std::optional<EntityId> parent) {
    if(!detail::positiveLengths(size) || !detail::finiteVector(position) || !detail::validColor(color) || (parent && !scene.getComponent(*parent))) {
        Diagnostics::fail(PipelineStage::Scene,"PatternSystem::addBox","Need positive dimensions, finite position/color and an existing parent.");return {};
    }
    auto model=DiagramSystem::makePolyhedron(GLShape::Cube,size);for(auto& face:model.faces) face.color=color;
    Transform3D transform;transform.basis=position;
    const auto entity=scene.addModel(model,transform,PivotMode::Preserve);
    if(!entity || !scene.setParent(*entity,parent)) return {};return entity;
}
inline std::optional<EntityId> PatternSystem::addPolygon(Scene& scene,const std::vector<glm::vec3>& points,
        GlColor color,std::optional<EntityId> parent) {
    if(points.size()<3 || !detail::validColor(color) || (parent && !scene.getComponent(*parent))) {
        Diagnostics::fail(PipelineStage::Scene,"PatternSystem::addPolygon","Need a convex polygon (>=3 points), valid color and parent.");return {};
    }
    Mesh face;face.localVertices=points;face.color=color;
    for(std::size_t i=1;i+1<points.size();++i) face.triangleIndices.insert(face.triangleIndices.end(),{0,std::uint32_t(i),std::uint32_t(i+1)});
    recalculateNormals(face);Model model;model.faces.push_back(face);
    const auto entity=scene.addModel(model,Transform3D{},PivotMode::Preserve);
    if(!entity || !scene.setParent(*entity,parent)) return {};return entity;
}
inline std::optional<EntityId> PatternSystem::addRoom(Scene& scene,const Bounds3& bounds,bool frontOpen) {
    if(!bounds.valid()) {Diagnostics::fail(PipelineStage::Scene,"PatternSystem::addRoom","Room bounds must have positive finite extents.");return {};}
    auto model=DiagramSystem::makePolyhedron(GLShape::Cube,bounds.maximum-bounds.minimum);
    for(auto& face:model.faces) {face.cull=CullMode::Front;recalculateNormals(face);for(auto& normal:face.vertexNormals) normal=-normal;}
    model.faces[0].visible=!frontOpen;
    Transform3D transform;transform.basis=(bounds.minimum+bounds.maximum)*.5f;
    return scene.addModel(model,transform,PivotMode::Preserve);
}
inline std::optional<EntityId> PatternSystem::addJoint(Scene& scene,EntityId parent,const glm::vec3& anchor) {
    if(!scene.getComponent(parent) || !detail::finiteVector(anchor)) {Diagnostics::fail(PipelineStage::Scene,"PatternSystem::addJoint","Missing parent or non-finite anchor.");return {};}
    Transform3D transform;transform.basis=anchor;const auto joint=scene.addGroup(transform);
    if(!joint || !scene.setParent(*joint,parent)) return {};return joint;
}
inline bool PatternSystem::rotateJoint(Scene& scene,EntityId joint,const glm::vec3& axis,float degrees) {
    const auto* component=scene.getComponent(joint);const auto rotation=TransformSystem::rotation({{0,0,0},axis},degrees);
    if(!component || !rotation) return Diagnostics::fail(PipelineStage::Transform,"PatternSystem::rotateJoint","Missing joint or invalid rotation.");
    auto transform=component->transform;transform.rotation=glm::mat3(*rotation);return scene.setTransform(joint,transform);
}
inline bool PatternSystem::follow(Scene& scene,EntityId entity,const DistancePath& path,double distance,const glm::vec3& offset,bool orient) {
    const auto pose=path.sample(distance);const auto* component=scene.getComponent(entity);
    if(!pose || !component || !detail::finiteVector(offset)) return Diagnostics::fail(PipelineStage::Motion,"PatternSystem::follow","Invalid path sample, entity or offset.");
    auto transform=component->transform;transform.basis=pose->position+offset;
    if(orient) transform.rotation=glm::mat3(glm::rotate(glm::mat4(1),glm::radians(pose->yawDegrees),glm::vec3(0,1,0)));
    return scene.setTransform(entity,transform);
}
inline bool PatternSystem::addNumber(Scene& scene,unsigned digit,const glm::vec3& position,float height,GlColor color,std::optional<EntityId> parent) {
    if(digit>9 || !detail::finiteVector(position) || !std::isfinite(height) || height<=0 || !detail::validColor(color) || (parent && !scene.getComponent(*parent)))
        return Diagnostics::fail(PipelineStage::Scene,"PatternSystem::addNumber","Need digit 0-9, positive height, finite values and a valid parent.");
    constexpr unsigned masks[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
    const glm::vec2 endpoints[7][2]={{{0,1},{.5f,1}},{{.5f,1},{.5f,.5f}},{{.5f,.5f},{.5f,0}},{{0,0},{.5f,0}},{{0,.5f},{0,0}},{{0,1},{0,.5f}},{{0,.5f},{.5f,.5f}}};
    for(unsigned i=0;i<7;++i) if(masks[digit]&(1u<<i)) {
        const auto a=position+glm::vec3(endpoints[i][0]*height,0),b=position+glm::vec3(endpoints[i][1]*height,0);
        if(!scene.addLine(a,b,color,parent)) return false;
    }
    return true;
}
inline bool PatternSystem::foldFace(Scene& scene,EntityId entity,std::size_t face,const AxisLine& hinge,float degrees) {
    const auto matrix=TransformSystem::rotation(hinge,degrees);return matrix && scene.setFaceLocalMatrix(entity,face,*matrix);
}
inline bool PatternSystem::openFace(Scene& scene,EntityId entity,std::size_t face,float progress,
        const glm::vec3& displacement,const glm::vec3& topPivot,bool scaleOpen) {
    if(!std::isfinite(progress) || progress<0 || progress>1 || !detail::finiteVector(displacement) || !detail::finiteVector(topPivot))
        return Diagnostics::fail(PipelineStage::Transform,"PatternSystem::openFace","Need progress in [0,1] and finite vectors.");
    if(scaleOpen) {
        // Fully open faces are hidden; retain a nonsingular matrix for normals/picking.
        const auto scale=TransformSystem::scaling({1,std::max(1e-4f,1-progress),1},topPivot);
        return scale && scene.setFaceLocalMatrix(entity,face,*scale) && scene.setFaceVisible(entity,face,progress<1);
    }
    return scene.setFaceLocalMatrix(entity,face,glm::translate(glm::mat4(1),displacement*progress)) && scene.setFaceVisible(entity,face,true);
}
inline bool PatternSystem::swapModels(Scene& scene,EntityId first,EntityId second) {
    const auto* a=scene.getComponent(first);const auto* b=scene.getComponent(second);
    if(!a || !b || a->transformOnly || b->transformOnly) return Diagnostics::fail(PipelineStage::Scene,"PatternSystem::swapModels","Both entities must contain a model.");
    const auto firstModel=a->polyhedron,secondModel=b->polyhedron;
    if(!scene.setModel(first,secondModel)) return false;
    if(!scene.setModel(second,firstModel)) {scene.setModel(first,firstModel);return false;}
    return true;
}
inline std::vector<glm::ivec3> PatternSystem::snakeCells(const glm::ivec3& cells) {
    std::vector<glm::ivec3> result;
    if(cells.x<=0 || cells.y!=1 || cells.z<=0 || std::uint64_t(cells.x)*cells.z>1000000) {
        Diagnostics::fail(PipelineStage::Motion,"PatternSystem::snakeCells","Need an XZ grid with y=1 and at most 1,000,000 cells.");return result;
    }
    for(int z=0;z<cells.z;++z) for(int x=0;x<cells.x;++x) result.push_back({z%2?cells.x-1-x:x,0,z});return result;
}
inline bool PatternSystem::aligned(float first,float second,float tolerance) {
    if(!std::isfinite(first) || !std::isfinite(second) || !std::isfinite(tolerance) || tolerance<0)
        return Diagnostics::fail(PipelineStage::Motion,"PatternSystem::aligned","Need finite heights and nonnegative tolerance.");
    return std::abs(double(first)-second)<=tolerance;
}
inline glm::vec3 PatternSystem::snap(const glm::vec3& point,float spacing) {
    if(!detail::finiteVector(point) || !std::isfinite(spacing) || spacing<=0) {
        Diagnostics::fail(PipelineStage::Transform,"PatternSystem::snap","Need finite point and positive spacing.");return point;
    }
    const auto result=glm::vec3(glm::round(glm::dvec3(point)/double(spacing))*double(spacing));
    if(!detail::finiteVector(result)) {Diagnostics::fail(PipelineStage::Transform,"PatternSystem::snap","Snap overflow.");return point;}return result;
}
inline std::optional<glm::vec3> PatternSystem::screenOnPlane(const Camera& camera,double x,double y,const PickViewport& viewport,
        const glm::vec3& planePoint,const glm::vec3& normal) {
    const auto ray=screenRay(camera,x,y,viewport);const auto n=detail::motionUnitVector(normal);
    if(!ray || !n || !detail::finiteVector(planePoint)) {Diagnostics::fail(PipelineStage::Input,"PatternSystem::screenOnPlane","Invalid camera, viewport, pointer or plane.");return {};}
    const double denominator=glm::dot(glm::dvec3(ray->direction),glm::dvec3(*n));
    if(std::abs(denominator)<1e-10) return {};
    const double t=glm::dot(glm::dvec3(planePoint)-glm::dvec3(ray->origin),glm::dvec3(*n))/denominator;
    if(t<0 || t>ray->maxDistance) return {};return glm::vec3(glm::dvec3(ray->origin)+glm::dvec3(ray->direction)*t);
}
inline bool PatternSystem::moveCamera(Camera& camera,const glm::vec3& offset) {
    auto next=camera;next.position+=offset;next.target+=offset;
    if(!CameraSystem::validCamera(next)) return Diagnostics::fail(PipelineStage::Transform,"PatternSystem::moveCamera","Camera translation creates an invalid camera.");
    camera=next;return true;
}
inline bool PatternSystem::yawCamera(Camera& camera,float degrees) {
    if(!CameraSystem::validCamera(camera)) return Diagnostics::fail(PipelineStage::Transform,"PatternSystem::yawCamera","Invalid camera.");
    const auto forward=glm::normalize(camera.target-camera.position),right=glm::normalize(glm::cross(forward,camera.up));
    const auto up=glm::normalize(glm::cross(right,forward));const auto rotation=TransformSystem::rotation({camera.position,up},degrees);
    if(!rotation) return false;
    auto next=camera;next.target=glm::vec3(*rotation*glm::vec4(camera.target,1));next.up=glm::mat3(*rotation)*up;
    if(!CameraSystem::validCamera(next)) return Diagnostics::fail(PipelineStage::Transform,"PatternSystem::yawCamera","Rotation creates an invalid camera.");
    camera=next;return true;
}
inline bool PatternSystem::orbitCamera(Camera& camera,const AxisLine& axis,float degrees) {
    const auto rotation=TransformSystem::rotation(axis,degrees);if(!rotation) return false;
    auto next=camera;next.position=glm::vec3(*rotation*glm::vec4(camera.position,1));
    next.target=glm::vec3(*rotation*glm::vec4(camera.target,1));next.up=glm::mat3(*rotation)*camera.up;
    if(!CameraSystem::validCamera(next)) return Diagnostics::fail(PipelineStage::Transform,"PatternSystem::orbitCamera","Orbit creates an invalid camera.");
    camera=next;return true;
}
inline bool PatternSystem::zoomCamera(Camera& camera,float factor) {
    if(!std::isfinite(factor) || factor<=0) return Diagnostics::fail(PipelineStage::Transform,"PatternSystem::zoomCamera","Zoom factor must be positive and finite.");
    auto next=camera;
    if(next.orthographic) next.orthographicHalfExtent*=factor;
    else next.position=next.target+(next.position-next.target)*factor;
    if(!CameraSystem::validCamera(next)) return Diagnostics::fail(PipelineStage::Transform,"PatternSystem::zoomCamera","Zoom creates an invalid camera.");
    camera=next;return true;
}
inline std::optional<JointRig> PatternSystem::addWalkingRig(Scene& scene,const GridLayout& grid) {
    if(!grid.valid() || !detail::rigidRoom(grid.frame)) {Diagnostics::fail(PipelineStage::Scene,"PatternSystem::addWalkingRig","Need a valid grid with a rigid frame.");return {};}
    JointRig rig;const auto root=scene.addGroup();if(!root) return {};rig.root=*root;
    auto point=[&](glm::vec3 ratio){return grid.origin+grid.extent*ratio;};
    const auto torso=addBox(scene,grid.extent*glm::vec3(.45f,.35f,.7f),point({.5f,.58f,.5f}),{.65f,.65f,.7f},rig.root);
    const auto head=addJoint(scene,rig.root,point({.5f,.8f,.5f}));if(!torso || !head) return {};rig.head=*head;
    if(!addBox(scene,grid.extent*glm::vec3(.3f,.2f,.6f),{0,grid.extent.y*.1f,0},{1,.8f,.5f},rig.head)
        || !addBox(scene,grid.extent*glm::vec3(.1f,.05f,.2f),{0,grid.extent.y*.1f,grid.extent.z*.4f},{1,.2f,.2f},rig.head)) return {};
    for(std::size_t i=0;i<2;++i) {
        const float x=i==0?.2f:.8f;const GlColor color=i==0?GlColor{.2f,.6f,1}:GlColor{1,.4f,.2f};
        const auto shoulder=addJoint(scene,rig.root,point({x,.72f,.5f})),hip=addJoint(scene,rig.root,point({i==0?.35f:.65f,.4f,.5f}));
        if(!shoulder || !hip) return {};rig.shoulders[i]=*shoulder;rig.hips[i]=*hip;
        if(!addBox(scene,grid.extent*glm::vec3(.15f,.32f,.3f),{0,-grid.extent.y*.16f,0},color,*shoulder)
            || !addBox(scene,grid.extent*glm::vec3(.18f,.4f,.4f),{0,-grid.extent.y*.2f,0},color,*hip)) return {};
    }
    Transform3D transform;transform.rotation=glm::mat3(grid.frame);transform.basis=glm::vec3(grid.frame[3]);
    if(!scene.setTransform(rig.root,transform)) return {};return rig;
}
inline bool PatternSystem::poseWalkingRig(Scene& scene,const JointRig& rig,const glm::vec3& feet,float yawDegrees,double distance,float speed) {
    if(!detail::finiteVector(feet) || !std::isfinite(yawDegrees) || !std::isfinite(distance) || !std::isfinite(speed) || speed<0)
        return Diagnostics::fail(PipelineStage::Motion,"PatternSystem::poseWalkingRig","Need finite pose values and nonnegative speed.");
    const auto* root=scene.getComponent(rig.root);
    if(!root || !scene.getComponent(rig.head)) return Diagnostics::fail(PipelineStage::Scene,"PatternSystem::poseWalkingRig","Rig IDs are missing or stale.");
    for(auto id:rig.shoulders) if(!scene.getComponent(id)) return Diagnostics::fail(PipelineStage::Scene,"PatternSystem::poseWalkingRig","Shoulder joint is missing.");
    for(auto id:rig.hips) if(!scene.getComponent(id)) return Diagnostics::fail(PipelineStage::Scene,"PatternSystem::poseWalkingRig","Hip joint is missing.");
    auto transform=root->transform;transform.basis=feet;
    transform.rotation=glm::mat3(glm::rotate(glm::mat4(1),glm::radians(yawDegrees),glm::vec3(0,1,0)));
    if(!scene.setTransform(rig.root,transform)) return false;
    const float angle=std::min(50.0f,15+speed*12)*float(std::sin(std::remainder(distance,1.0)*6.283185307179586));
    for(std::size_t i=0;i<2;++i) {
        const float sign=i==0?1.0f:-1.0f;
        if(!rotateJoint(scene,rig.hips[i],{1,0,0},sign*angle) || !rotateJoint(scene,rig.shoulders[i],{1,0,0},-sign*angle)) return false;
    }
    return true;
}
} // namespace graphics

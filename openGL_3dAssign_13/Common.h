#pragma once

#include <glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstddef>
#include <filesystem>
#include <optional>
#include <array>
#include <string>
#include <vector>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_DIAGRAMS = 50;

struct Point {
    float x = 0.0f;
    float y = 0.0f;
};

struct GlColor {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;

    void set(float r, float g, float b);
    void setRandom();
};

enum class GLShape { None, Cube, Tetra }; // Cube has six Rect Face,Tetra has four Triangle Face
enum class GLFace{ None, Rect, Triangle };
enum GLAtrribute{ Vertices, Color, Normal }; //object attribute



class Entity {//E of ECS
    std::vector<size_t> polyId{}; // entity id (index) per poly
    std::vector<std::unique_ptr<Polyhedron*>> polyptr{};
    std::vector<size_t> tetraFaceId{};
    std::vector<size_t> cubeFaceId{};

public:
    Entity(){} // reserve vectors to (n, 4*n, 6*n) 
   std::optional<size_t> push_entity(const Polyhedron& pp); // push to polyptr -> push the index to polyid -> get Faces and the type, and push each to FaceId
   

};

class DiagramSystem { //S of ECS

};




class PlaneFigure { //2-dimension diagram component, 
public:
    Diagram(GLShape shape, const std::vector<Point>& localVertices,
            GlColor color, Point pivot, Point localPivot = {});

    GLShape getShape() const { return shape_; }
    const GlColor& getColor() const { return color_; }
    std::size_t getVertexCount() const { return localVertices_.size(); }
    Point getPivot() const { return pivot_; }
    const std::vector<Point>& getOldPivots() const { return oldPivots; };
    Point getScale() const { return scale_; }



    //we now transfer responsibility for System of ECS to diagramSystem
    bool canTranslate(Point direction) const;
    bool translate(Point direction);///t transform
    void setDirection(Point direction, float speed); // r
    bool setScale(Point scale);//s transform
    bool setRotation(float degrees);//r transform
    bool setPose(Point pivot, float rotationDegrees);
    bool advance(float deltaTime);
    std::vector<Point> worldVertices() const;
    std::vector<Point> worldVerticesAt(Point pivot, float rotationDegrees) const;  // m of mvp
    bool fitsNdc() const;
    bool contains(Point point) const;

private:
    GLFace shape_;
    std::array<glm::vec3, 2> localVertices_{};
    GlColor color_;
    std::optional<glm::vec3> basis_;       // World position. Translation and direction move this point.
    std::optional<glm::vec3> localBasis_;  // basis for rotation and scale in local coordinates. generally equal to poly, but if independence == true -> get own basis and attribute
    Point scale_{ 1.0f, 1.0f };
    float rotationDegrees_ = 0.0f;
    Point direction_{};
    float speed_ = 0.0f;
    bool independence = false; // separated for poly
    
};



class Polyhedron {// 3-dimension poly, that has own PlaneFigures

};



class Componet_Poly {// C of ECS, that has Components

};


float randomFloat(float minimum, float maximum);
std::string readTextFile(const std::filesystem::path& path);


class ShaderHandler {
public:
    bool initShader(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    bool initBuffer();
    bool upload(const std::vector<Diagram>& diagrams,
                std::optional<std::size_t> selectedIndex);
    void render() const;
    void destroy();

private:
    struct DrawRange {
        GLenum mode = GL_TRIANGLES;
        GLint first = 0;
        GLsizei count = 0;
    };

    static constexpr std::size_t MAX_VERTICES = MAX_DIAGRAMS * 4;
    GLuint shaderProgramID_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_[2] = { 0, 0 }; // Position and color streams.
    GLuint pathVao_ = 0;
    GLuint pathVbo_ = 0;
    GLsizeiptr pathBufferBytes_ = 0;
    GLuint ebo_ = 0;          // Shared outline and fill indices for all shapes.
    GLint usePathPositionLocation_ = -1;
    GLint useOverrideLocation_ = -1;
    GLint overrideColorLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    std::vector<DrawRange> pathRanges_;
    std::optional<std::size_t> selectedIndex_;
    GLsizei selectedOutlineCount_ = 0;
    GLenum selectedOutlineMode_ = GL_LINE_LOOP;
};

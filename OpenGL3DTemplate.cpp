#include <tiny_obj_loader.h>
#include <glut.h>
#include <vector>
#include <iostream>

// Rotation angle for animation
float rotAng = 0.0f;

// Vectors to store vertices and normals
std::vector<float> vertices;
std::vector<float> normals;
std::vector<tinyobj::material_t> materials; // Make materials global

// Function to load the OBJ model
bool LoadModel(const char* obj_filepath, const char* mtl_filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Load the OBJ file and the associated materials file (mtl)
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_filepath, mtl_filepath)) {
        std::cerr << "Failed to load OBJ file: " << warn << err << std::endl;
        return false;
    }

    // Extract vertex and normal data
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            if (index.normal_index >= 0) {
                normals.push_back(attrib.normals[3 * index.normal_index + 0]);
                normals.push_back(attrib.normals[3 * index.normal_index + 1]);
                normals.push_back(attrib.normals[3 * index.normal_index + 2]);
            }
        }
    }

    // Handle materials (if they exist)
    if (!materials.empty()) {
        const tinyobj::material_t& mat = materials[0];
        GLfloat mat_diffuse[] = { mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1.0f };
        GLfloat mat_specular[] = { mat.specular[0], mat.specular[1], mat.specular[2], 1.0f };
        GLfloat mat_ambient[] = { mat.ambient[0], mat.ambient[1], mat.ambient[2], 1.0f };
        GLfloat mat_shininess[] = { mat.shininess };

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    }

    return true;
}



// Function to render the loaded model
void RenderModel() {
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < vertices.size(); i += 3) {
        if (!normals.empty()) {
            glNormal3f(normals[i], normals[i + 1], normals[i + 2]);
        }
        glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();
}

void SetupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Enable the first light source

    // Set light position (directional light in this case)
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Set light color
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // White light
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);  // Allow material to interact with light
}


void SetMaterial(const tinyobj::material_t& material) {
    GLfloat mat_ambient[] = { material.ambient[0], material.ambient[1], material.ambient[2], 1.0f };
    GLfloat mat_diffuse[] = { material.diffuse[0], material.diffuse[1], material.diffuse[2], 1.0f };
    GLfloat mat_specular[] = { material.specular[0], material.specular[1], material.specular[2], 1.0f };
    GLfloat mat_shininess[] = { material.shininess };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}



// Display function to render the scene
void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    // Adjust model position, scale, and rotation
    glTranslatef(0.0f, -1.0f, 0.0f); // Move model down for better viewing
    glScalef(0.02f, 0.02f, 0.02f);    // Scale down the model even further
    glRotatef(rotAng, 0, 1, 0);       // Rotate the model
    glColor3f(0.8f, 0.2f, 0.2f);     // Set model color
    RenderModel();
    glPopMatrix();

    glFlush();
}

// Animation function to update the rotation
void Anim() {
    rotAng += 0.1f;
    if (rotAng > 360.0f) rotAng -= 360.0f;
    glutPostRedisplay();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("OpenGL - 3D Template");

    glutDisplayFunc(Display);
    glutIdleFunc(Anim);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Set up perspective and camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, 1.0f, 0.1f, 300.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 10.0f,   // Move camera further back
        0.0f, 0.0f, 0.0f,    // Look at the origin
        0.0f, 1.0f, 0.0f);   // Up direction

    // Set up lighting
    SetupLighting();

    // Set up material properties for the model
    // Update this line to load the OBJ with the path to the MTL file
    if (!LoadModel("C:/Users/nadee/Desktop/Hunting_3D_Game/zombie/PhychoZombie/untitled.obj",
        "C:/Users/nadee/Desktop/Hunting_3D_Game/zombie/PhychoZombie/untitled.mtl")) {
        std::cerr << "Error: Could not load the model!" << std::endl;
        return -1; // Exit if model loading fails
    }

    // Start the GLUT main loop
    glutMainLoop();
    return 0;
}



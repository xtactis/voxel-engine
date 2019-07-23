//TODO make the World struct
//TODO make voxels selectable
//TODO look into the scaling, I don't think 0.05 resolution is actually 5cm
//TODO support voxel deletion
//TODO add saving functionality
//TODO fix lighting
//TODO add support for octomap files (load/save)
//TODO look into using octotrees instead of big ass arrays for bits
//TODO convert structs to classes
//TODO reduce memory usage
//TODO parallelize where possible
//TODO refactor to make things safer
//TODO split into multiple files, maybe? anything below 5k lines is fine with me tbh

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#define AAA printf("AAA\n");

const int WIDTH = 800;
const int HEIGHT = 600;
const char *NAME = "neki kurac";

const float specLight[]={0.5,0.5,0.5,1};
const float diffLight[]={0.5,0.5,0.5,1};
const float ambLight[]={0.5,0.5,0.5,1};

const int chunkSize = 16;
double resolution = 0.1;
char *filename;

int mouseEndX, mouseEndY, mouseStartX, mouseStartY;
int thetaX = 0, thetaY = 0;
bool leftMouseDown = false;
bool rightMouseDown = false;

long long int voxelCount=0;

float minx=0, maxx=0, miny=0, maxy=0, minz=0, maxz=0;
int W, H, D;

glm::vec3 white = glm::vec3(1, 1, 1);

glm::vec3 pos=glm::vec3(0,0,0);
std::vector<glm::vec3> pts;

struct BitArray {
    uint8_t *arr;
    int _size;
    BitArray(int size=chunkSize) {
        arr = (uint8_t*)calloc((size+7)/8, 1);
        _size = size;
    }
    bool operator[](int i) {
        //printf("%d\n", arr[i/8]);
        return arr[i/8]&(1<<(i%8));
    }
    void set(int i, bool value) {
        if (value) {
            arr[i/8] |= 1<<(i%8);
        } else {
            arr[i/8] &= ~(1<<(i%8));
        }
    }
};

struct Shader {
    GLuint program;
    GLuint VERTEX_ATTR_COORDS = 1;
    GLuint VERTEX_ATTR_COLOR = 2;
    const char* vertex_shader =
        "attribute vec3 aCoords;"
        "attribute vec3 aColor;"
        "uniform mat4 umvMat;"
        "uniform mat4 upMat;"
        "varying vec3 vColor;"
        "void main () {"
            "gl_Position = upMat * umvMat * vec4(aCoords, 1.0);"
//            "aColor = aCoords;"
            "vColor = aColor;"
        "}";
    
    const char* fragment_shader =
        "varying vec3 vColor;"
        "void main () {"
            "gl_FragColor = vec4 (vColor, 1.0);"
        "}";
    
    Shader() {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertex_shader, NULL);
        glCompileShader(vs);
    
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragment_shader, NULL);
        glCompileShader(fs);
    
        program = glCreateProgram();
        glAttachShader(program, fs);
        glAttachShader(program, vs);
    
        glBindAttribLocation(program, VERTEX_ATTR_COORDS, "aCoords");
        glBindAttribLocation(program, VERTEX_ATTR_COLOR, "aColor");
    
        glLinkProgram(program);
    }
    
    void run() {
        glUseProgram(program);
    }
};

Shader *shader;
    
struct Camera {
    float x, y, z;
    float lookX, lookY, lookZ;
    float upX, upY, upZ;
} cam;

struct Mesh {
    std::vector<float> points;
    std::vector<float> colors;
    GLuint vaoIds[1], vboIds[2];
    int _pSize = 0, _cSize = 0;
    
    void addToMesh(glm::vec3 point) {
        //printf("%f %f %f\n", point[0], point[1], point[2]);
        points.push_back(point[0]);
        points.push_back(point[1]);
        points.push_back(point[2]);
        colors.push_back((point[2]-minz)/(maxz-minz));
        colors.push_back((point[2]-minz)/(maxz-minz));
        colors.push_back((point[2]-minz)/(maxz-minz));
        _pSize = points.size();
        _cSize = colors.size();
    }
    
    void addCubeToMesh(float x, float y, float z, float size, std::vector<bool> neighbors) {
        ++voxelCount;
        size /= 2;
        glm::vec3 p1(x-size, y-size, z+size);
        glm::vec3 p2(x+size, y-size, z+size);
        glm::vec3 p3(x+size, y+size, z+size);
        glm::vec3 p4(x-size, y+size, z+size);
        glm::vec3 p5(x+size, y-size, z-size);
        glm::vec3 p6(x-size, y-size, z-size);
        glm::vec3 p7(x-size, y+size, z-size);
        glm::vec3 p8(x+size, y+size, z-size);
        
        //front
        if (!neighbors[1]) {
            addToMesh(p1);
            addToMesh(p2);
            addToMesh(p3);
            
            addToMesh(p1);
            addToMesh(p3);
            addToMesh(p4);
        }
        
        //back
        if (!neighbors[0]) {
            addToMesh(p5);
            addToMesh(p6);
            addToMesh(p7);
            
            addToMesh(p5);
            addToMesh(p7);
            addToMesh(p8);
        }
        
        //right
        if (!neighbors[5]) {
            addToMesh(p2);
            addToMesh(p5);
            addToMesh(p8);
            
            addToMesh(p2);
            addToMesh(p8);
            addToMesh(p3);
        }
        
        //left
        if (!neighbors[4]) {
            addToMesh(p6);
            addToMesh(p1);
            addToMesh(p4);
            
            addToMesh(p6);
            addToMesh(p4);
            addToMesh(p7);
        }
        
        //top
        if (!neighbors[3]) {
            addToMesh(p4);
            addToMesh(p3);
            addToMesh(p8);
            
            addToMesh(p4);
            addToMesh(p8);
            addToMesh(p7);
        }
        
        //bottom
        if (!neighbors[2]) {
            addToMesh(p6);
            addToMesh(p5);
            addToMesh(p2);
            
            addToMesh(p6);
            addToMesh(p2);
            addToMesh(p1);
        }
    }
    
    void clearMesh() {
        points.clear();
        colors.clear();
    }
    
    void drawMesh() {
        // Get the variables from the shader to which data will be passed
        GLint mvloc = glGetUniformLocation(shader->program, "umvMat");
        GLint ploc = glGetUniformLocation(shader->program, "upMat");
    
        // Pass the model-view matrix to the shader
        GLfloat mvMat[16]; 
        glGetFloatv(GL_MODELVIEW_MATRIX, mvMat); 
        glUniformMatrix4fv(mvloc, 1, false, mvMat);
    
        // Pass the projection matrix to the shader
        GLfloat pMat[16]; 
        glGetFloatv(GL_PROJECTION_MATRIX, pMat); 
        glUniformMatrix4fv(ploc, 1, false, pMat);
    
        // Enable VAO to set pyramid data
        glBindVertexArray(vaoIds[0]);
    
        // Draw pyramid
        glDrawArrays(GL_TRIANGLES, 0, _pSize);
    
        // Disable VAO
        glBindVertexArray(0);
    }
    
    void createMesh() {
        glGenVertexArrays(1, vaoIds);
        glGenBuffers(2, vboIds);
    
        GLint vertexAttribCoords = glGetAttribLocation(shader->program, "aCoords");
        GLint vertexAttribColor = glGetAttribLocation(shader->program, "aColor");
    
        // Bind VAO (set current) to define pyramid data
        glBindVertexArray(vaoIds[0]);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);  // coordinates
        glBufferData(GL_ARRAY_BUFFER, _pSize*sizeof(float), points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(vertexAttribCoords, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vertexAttribCoords);
    
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);  // color
        glBufferData(GL_ARRAY_BUFFER, _cSize*sizeof(float), colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(vertexAttribColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vertexAttribColor);
    
        // Disable VAO
        glBindVertexArray(0);
    }
};

bool checkNeighbor(int gx, int gy, int gz, int x, int y, int z); //TODO to avoid this make the World struct

struct Chunk {
    BitArray block[chunkSize][chunkSize];
    Mesh mesh;
    int globalX, globalY, globalZ;
    
    Chunk(){}
    void init(int x, int y, int z) {
        globalX = x;
        globalY = y;
        globalZ = z;
    }
    
    Chunk(int x, int y, int z) {
        init(x, y, z);
    }
    
    void setBlock(float x, float y, float z) {
        //printf("%d\n", block[8][9][1]);
        //wwdprintf("%.10f %f %f %d %d %d\n", x, y, z, (int)floor(x/resolution), (int)floor(y/resolution), (int)floor(z/resolution));
        block[(int)floor(x/resolution)][(int)floor(y/resolution)].set((int)floor(z/resolution), true);
    }
  
/*    bool checkNeighbor(int x, int y, int z) {
        int gx = globalX, gy = globalY, gz = globalZ;
        std::vector<std::pair<int*, int*>> pcoord(3);
        pcoord.push_back(std::make_pair(&x, &gx));
        pcoord.push_back(std::make_pair(&y, &gy));
        pcoord.push_back(std::make_pair(&z, &gz));
        for (auto e: pcoord) {
            if (*e.first < 0) {
                *e.first = 15;
                --*e.second;
            } else if (*e.first > 15) {
                *e.first = 0;
                ++*e.second;
            }
        }
        return (*world)[gz+gy*D+gz*D*H].block[x, y, z];
    }
*/  
    void createChunk() {
        mesh.clearMesh();
        for (int i = 0; i < chunkSize; ++i) {
            for (int j = 0; j < chunkSize; ++j) {
                for (int k = 0; k < chunkSize; ++k) {
                    if (!block[i][j][k]) continue;
                    std::vector<bool> neighbors;
                    neighbors.clear();
                    neighbors.push_back(checkNeighbor(globalX, globalY, globalZ, i, j, k-1));
                    neighbors.push_back(checkNeighbor(globalX, globalY, globalZ, i, j, k+1));
                    neighbors.push_back(checkNeighbor(globalX, globalY, globalZ, i, j-1, k));
                    neighbors.push_back(checkNeighbor(globalX, globalY, globalZ, i, j+1, k));
                    neighbors.push_back(checkNeighbor(globalX, globalY, globalZ, i-1, j, k));
                    neighbors.push_back(checkNeighbor(globalX, globalY, globalZ, i+1, j, k));
                    mesh.addCubeToMesh(i*resolution, j*resolution, k*resolution, resolution, neighbors);
                }
            }
        }
        mesh.createMesh();
        mesh.clearMesh();
    }
    
    void drawChunk() {
        mesh.drawMesh();
    }
};

struct World {
    std::vector<Chunk> *chunks;
    float minx, maxx, miny, maxy, minz, maxz;
    float W, H, D;
    
    //TODO this struct
    
    World() {
        const float width = maxx-minx;
        const float height = maxy-miny;
        const float depth = maxz-minz;
        const float realChunkSize = chunkSize*resolution;
        printf("Dimensions: %.2fx%.2fx%.2f\n", width, height, depth);
        printf("Resolution: %.2f\n", resolution);
        //world = new Chunk[(int)ceil(width/realChunkSize)][(int)ceil(height/realChunkSize)][(int)ceil(depth/realChunkSize)];
        W = floor(width/realChunkSize)+1;
        H = floor(height/realChunkSize)+1;
        D = floor(depth/realChunkSize)+1;
        chunks = new std::vector<Chunk>(W*H*D);
        printf("[%.2f, %.2f] [%.2f, %.2f] [%.2f %.2f]\n", minx, maxx, miny, maxy, minz, maxz);
        for (const auto &p: pts) {
            float x = p[0]+fabs(minx);
            float y = p[1]+fabs(miny);
            float z = p[2]+fabs(minz);
            int indx = floor(x/realChunkSize);
            int indy = floor(y/realChunkSize);
            int indz = floor(z/realChunkSize);
            //printf("%f %f %f\n", p[0], p[1], p[2]);
            //printf("%f %f %f %d %d %d %d; %d %d %d\n", x, y, z, indx, indy, indz, indx+indy*W+indz*W*H, W, H, D);
            (*chunks)[indz+indy*D+indx*D*H].setBlock(x-indx*realChunkSize, y-indy*realChunkSize, z-indz*realChunkSize);
        }
    }
};
std::vector<Chunk> *world;

Mesh mesh;

#include <bitset>
int md = 0;
struct Octree {
	bool root;
	float halfsize; //actually halfsize lol
	std::bitset<8> children;
	Octree *childrenNodes[8];
	
	Octree(float s) {
		halfsize = s/2;
		root = false;
	}
	
	bool addPoint(float x, float y, float z, int depth=0) {
		md = std::max(depth, md);
		char ind = (x>halfsize)|(y>halfsize)<<1|(z>halfsize)<<2;
		//printf("%d <- %d %d %d\n", ind, ((x>halfsize)&1), ((y>halfsize)&1)<<1, ((z>halfsize)&1)<<2);
		children[ind] = 1;
		if (root) {
			if (halfsize > resolution)
				childrenNodes[ind]->addPoint(x-halfsize*(x>halfsize), y-halfsize*(y>halfsize), z-halfsize*(z>halfsize), ++depth);
		} else {
			//printf("%d\n", depth);
			root = true;
			for (int i = 0; i < 8; ++i) {
				childrenNodes[i] = new Octree(halfsize);
			}
			//printf("%d\n", depth);
		}
	}
	
	bool checkLeaf(float x, float y, float z) {
		if (root) {
			return checkLeaf(x-halfsize*(x>halfsize), y-halfsize*(y>halfsize), z-halfsize*(z>halfsize));
		}
		char ind = (x>halfsize)|(y>halfsize)<<1|(z>halfsize)<<2;
		return children[ind];
	}
	
	void createMeshHelper(double x=0, double y=0, double z=0) {
		if (!root) {
			mesh.addCubeToMesh(x, y, z, halfsize*2, std::vector<bool>(6, 0));
			return;
		}
		for (int i = 0; i < 8; ++i) {
			if (children[i])
				childrenNodes[i]->createMeshHelper(x+halfsize*(i&1), y+halfsize*!!(i&2), z+halfsize*!!(i&4));
		}
	}
	
	void createMesh() {
		mesh.clearMesh();
		createMeshHelper();
		mesh.createMesh();
	}
	
	void draw() {
		mesh.drawMesh();
	}
	
	int size() {
		int ret = 1+4+1+8*8;
		if (!root) return ret;
		for (int i = 0; i < 8; ++i) {
			if (children[i])
				ret += childrenNodes[i]->size();
		}
		return ret;
	}
};

Octree *oct;


bool checkNeighbor(int gx, int gy, int gz, int x, int y, int z) {
    //int gx = globalX, gy = globalY, gz = globalZ;
    std::vector<std::pair<int*, int*>> pcoord;
    pcoord.push_back(std::make_pair(&x, &gx));
    pcoord.push_back(std::make_pair(&y, &gy));
    pcoord.push_back(std::make_pair(&z, &gz));
    
    for (auto e: pcoord) {
        if (*(e.first) < 0) {
            if (*(e.second) == 0) return false;
            *(e.first) = chunkSize-1;
            --*(e.second);
        } else if (*e.first > chunkSize-1) {
            *(e.first) = 0;
            ++*(e.second);
        }
    }
    if (gx > W || gy > H || gz > D) return false;
    //printf("%d %d %d %d\n", gz+gy*D+gx*D*H, x, y, z);
    return (*world)[gz+gy*D+gx*D*H].block[x][y][z];
}

void camera() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    /*glLightfv(GL_LIGHT0,GL_AMBIENT,ambLight);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffLight);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specLight);
    glLightfv(GL_LIGHT0,GL_POSITION, glm::value_ptr(pos));*/

    glLoadIdentity();
    glm::vec3 eye(0, 0, 1);
    glm::vec3 look(0, 0, 0);
    glm::vec3 up(0, 1, 0);
    glm::mat4 camera_matrix = glm::lookAt(eye, look, up);
    glMultMatrixf(glm::value_ptr(camera_matrix));
    
    
    glTranslatef(cam.x, cam.y, cam.z);
    glRotatef(thetaX, 0, 1, 0);
    glRotatef(thetaY, 1, 0, 0);
}

void drawCubes() {
    //printf("aaa %d\n", H*W*D*16*16*16/1024);
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            for (int k = 0; k < D; ++k) {
                glPushMatrix();
                    //printf("%d\n", i+j*W+k*W*H);
                    glTranslatef(i*resolution*chunkSize, j*resolution*chunkSize, k*resolution*chunkSize);
                    (*world)[k+j*D+i*D*H].drawChunk();
                glPopMatrix();
            }
        }
    }
    //printf("bbb\n");
}

void drawGLScene() {
    camera();
    shader->run();
    
	oct->draw();
    //drawCubes();
    
    glutSwapBuffers();
}

void resizeGLScene(int width, int height) {
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const float fovy = 45;
    const float zNear = 0.1;
    const float zFar = 100;

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(fovy), // vertikalni "field of view", u radijanima - obicno
        // izmedju 90 i 30 stupnjeva
        (float)width/height, // aspect ratio
        zNear, // near clipping plane. Cim veci tim bolji
        zFar // far clipping plane. Cim manji tim bolji - zFar > zNear
    );

    glMultMatrixf(glm::value_ptr(projectionMatrix));
}

void keyboard(unsigned char key, int, int) {
    if (key==27) {
        exit(0);
    }
    if (key == 'a' || key == 'A')
        cam.x += 0.2;
    if (key == 'd' || key == 'D')
        cam.x -= 0.2;
    if (key == 's' || key == 'S')
        cam.y += 0.2;
    if (key == 'w' || key == 'W')
        cam.y -= 0.2;
    if (key == 'r' || key == 'r')
        cam.z += 0.2;
    if (key == 'f' || key == 'F')
        cam.z -= 0.2;
    if (key == 'v' || key == 'V')
        printf("Drawing %lld voxels.\n", voxelCount);
}

void mouse(int button, int state, int x, int y) {
    printf("%d %d %d %d\n", button, state, x, y);
    mouseStartX = x;
    mouseStartY = y;
    if (button == 0 && state == 0) {
        leftMouseDown = true;
    } else if (button == 0 && state == 1) {
        leftMouseDown = false;
    }
    if (button == 2 && state == 0) {
        rightMouseDown = true;
    } else if (button == 2 && state == 1) {
        rightMouseDown = false;
    }
    if (button == 3 && state == 1) {
        cam.z += 0.2;
    }
    if (button == 4 && state == 1) {
        cam.z -= 0.2;
    }
}

void mouseMove(int x, int y) {
    printf("%d %d\n", x, y);
    if (leftMouseDown && rightMouseDown) {
        cam.x += (mouseEndX-mouseStartX)/fabs(cam.z*2);
        cam.y += (mouseEndY-mouseStartY)/-fabs(cam.z*2);
    } else if (leftMouseDown) {
        thetaX += mouseEndX-mouseStartX;
        thetaY += mouseEndY-mouseStartY;
        thetaX %= 360;
        thetaY %= 360;
    }
    mouseStartX = mouseEndX;
    mouseStartY = mouseEndY;
    mouseEndX = x;
    mouseEndY = y;
}

void update(int a) {
    glutPostRedisplay();
    glutTimerFunc(0, update, 0);
}

void parseCmdArgs(int argc, char **argv) {
    for (int i = 1; i < argc-1; ++i) {
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--resolution") == 0) {
            resolution = std::atof(argv[++i]);
        } else {
            printf("Unrecognized parameter: %s\n", argv[i]);
            exit(-1);
        }
    }
    filename = argv[argc-1];
}

void init() {
    std::ifstream infile(filename);
    std::string line;
    std::vector<glm::vec3> pts;
    float x, y, z; 
    bool firstline = true;
    while (std::getline(infile, line)) {
        if (firstline) {
            sscanf(line.c_str(), "NODE %f %f %f 0 0 0", &x, &y, &z);
            firstline = false;
            minx = maxx = x;
            miny = maxy = y;
            minz = maxz = z;
        } else {
            sscanf(line.c_str(), "%f %f %f", &x, &y, &z);
        }
        pts.push_back(glm::vec3(x, y, z));
        minx = std::min(x, minx);
        miny = std::min(y, miny);
        minz = std::min(z, minz);
        maxx = std::max(x, maxx);
        maxy = std::max(y, maxy);
        maxz = std::max(z, maxz);
    }
    infile.close();
    const float width = maxx-minx;
    const float height = maxy-miny;
    const float depth = maxz-minz;
    const double realChunkSize = chunkSize*resolution;
    printf("Dimensions: %.2fx%.2fx%.2f\n", width, height, depth);
    printf("Resolution: %.2f\n", resolution);
    //world = new Chunk[(int)ceil(width/realChunkSize)][(int)ceil(height/realChunkSize)][(int)ceil(depth/realChunkSize)];
    W = floor(width/realChunkSize)+1;
    H = floor(height/realChunkSize)+1;
    D = floor(depth/realChunkSize)+1;
    /*world = new std::vector<Chunk>(W*H*D);
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            for (int k = 0; k < D; ++k) {
                (*world)[k+j*D+i*D*H].init(i, j, k);
            }
        }
    }*/
    printf("[%.2f, %.2f] [%.2f, %.2f] [%.2f %.2f]\n", minx, maxx, miny, maxy, minz, maxz);
    float size = std::max(std::max(width, height), depth);
    oct = new Octree(size);
	int i = 0;
	printf("\n");
    for (const auto &p: pts) {
		if (++i%10000)	printf("\r%d", (int)(100.0*i/pts.size()));
        float x = p[0]+fabs(minx);
        float y = p[1]+fabs(miny);
        float z = p[2]+fabs(minz);
        int indx = floor(x/realChunkSize);
        int indy = floor(y/realChunkSize);
        int indz = floor(z/realChunkSize);
        //printf("%f %f %f\n", p[0], p[1], p[2]);
        //printf("%f %f %f %d %d %d %d; %d %d %d\n", x, y, z, indx, indy, indz, indx+indy*W+indz*W*H, W, H, D);
        //(*world)[indz+indy*D+indx*D*H].setBlock(x-indx*realChunkSize, y-indy*realChunkSize, z-indz*realChunkSize);
        
        auto f = [](float x) {return floor(x/resolution)*resolution;};
        oct->addPoint(f(x), f(y), f(z));
    }
    
    printf("\n%d %d\n", oct->size(), md);
    
    shader = new Shader();
	
	oct->createMesh();
    /*for (auto &chunk: *world) {
        chunk.createChunk();
    }*/
    
    cam.x = 0; cam.y = 0; cam.z = 1;
    cam.lookX = 0; cam.lookY = 0; cam.lookZ = 0;
    cam.upX = 0; cam.upY = 1; cam.upZ = 0;
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    parseCmdArgs(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow(NAME);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error %s\n", glewGetErrorString(err)); 
        exit(1);
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    if (GLEW_ARB_vertex_program)
        fprintf(stdout, "Status: ARB vertex programs available.\n");
    if (glewGetExtension("GL_ARB_fragment_program"))
        fprintf(stdout, "Status: ARB fragment programs available.\n");
    if (glewIsSupported("GL_VERSION_1_4  GL_ARB_point_sprite"))
        fprintf(stdout, "Status: ARB point sprites available.\n");
    
    glClearColor(0,0,0,.5);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST); // z-buffer test
    //glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    
    glutDisplayFunc(drawGLScene);
    glutReshapeFunc(resizeGLScene);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMove);
    glutTimerFunc(0,update,0);
    
    init();
    
    glutMainLoop();
    
    //delete world;
    delete shader;
    return 0;
}

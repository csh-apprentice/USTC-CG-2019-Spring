//  88-Line 2D Moving Least Squares Material Point Method (MLS-MPM)
// [Explained Version by David Medina]

// Uncomment this line for image exporting functionality
#define TC_IMAGE_IO

// Note: You DO NOT have to install taichi or taichi_mpm.
// You only need [taichi.h] - see below for instructions.
#include "taichi.h"

using namespace taichi;
using Vec = Vector2;
using Mat = Matrix2;

// Window
const int window_size = 500;

// Grid resolution (cells)
const int n = 80;

const real dt = 1e-4_f;
const real frame_dt = 1e-3_f;
const real dx = 1.0_f / n;
const real inv_dx = 1.0_f / dx;

// Snow material properties
const auto particle_mass = 1.0_f;
const auto vol = 1.0_f;        // Particle Volume
const auto hardening = 10.0_f; // Snow hardening factor
const auto E = 1e4_f;          // Young's Modulus
const auto nu = 0.2_f;         // Poisson ratio
const bool plastic = true;
int start_index, end_index;
const Vec Vec_zero(0.0, 0.0);

// Initial Lamé parameters
const real mu_0 = E / (2 * (1 + nu));
const real lambda_0 = E * nu / ((1+nu) * (1 - 2 * nu));

struct Particle {
  // Position and velocity
  Vec x, v;
  // Deformation gradient
  Mat F;
  // Affine momentum from APIC
  Mat C;
  // Determinant of the deformation gradient (i.e. volume)
  real Jp;
  // Color
  int c;

  int ptype/*0: fluid 1: jelly 2: snow*/;

  Particle(Vec x, int c, Vec v=Vec(0), int ptype = 2) :
    x(x),
    v(v),
    F(1),
    C(0),
    Jp(1),
    c(c) ,
    ptype(ptype) {}
};



std::vector<Particle> particles;

// Vector3: [velocity_x, velocity_y, mass]
Vector3 grid[n + 1][n + 1];

void advance(real dt,Vec space_craft_pos,int space_num,int planet_num,real weight) {
  // Reset grid
  std::memset(grid, 0, sizeof(grid));

  // P2G
  for (auto &p : particles) {
    // element-wise floor
    Vector2i base_coord = (p.x * inv_dx - Vec(0.5f)).cast<int>();

    Vec fx = p.x * inv_dx - base_coord.cast<real>();

    // Quadratic kernels [http://mpm.graphics Eqn. 123, with x=fx, fx-1,fx-2]
    Vec w[3] = {
      Vec(0.5) * sqr(Vec(1.5) - fx),
      Vec(0.75) - sqr(fx - Vec(1.0)),
      Vec(0.5) * sqr(fx - Vec(0.5))
    };

    auto e = std::exp(hardening * (1.0_f - p.Jp));
    if (p.ptype == 1) e = 0.3;
    auto mu = mu_0 * e, lambda = lambda_0 * e;
    if (p.ptype == 0) mu = 0;

    

    // Current volume
    real J = determinant(p.F);

    // Polar decomposition for fixed corotated model
    Mat r, s;
    polar_decomp(p.F, r, s);

    // [http://mpm.graphics Paragraph after Eqn. 176]
    real Dinv = 4 * inv_dx * inv_dx;
    // [http://mpm.graphics Eqn. 52]
    auto PF = (2 * mu * (p.F-r) * transposed(p.F) + lambda * (J-1) * J);

    // Cauchy stress times dt and inv_dx
    auto stress = - (dt * vol) * (Dinv * PF);

    // Fused APIC momentum + MLS-MPM stress contribution
    // See http://taichi.graphics/wp-content/uploads/2019/03/mls-mpm-cpic.pdf
    // Eqn 29
    auto affine = stress + particle_mass * p.C;

    // P2G
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        auto dpos = (Vec(i, j) - fx) * dx;
        // Translational momentum
        Vector3 mass_x_velocity(p.v * particle_mass, particle_mass);
        grid[base_coord.x + i][base_coord.y + j] += (
          w[i].x*w[j].y * (mass_x_velocity + Vector3(affine * dpos, 0))
        );
      }
    }
  }

  //设置行星于宇宙飞船之间的引力
  real x_cor, y_cor;
  x_cor = 0.5 - space_craft_pos.x;
  y_cor = 0.3 - space_craft_pos.y;
  real dis = sqrt(pow(x_cor, 2) + pow(y_cor, 2));

  real force = (space_num * planet_num) * weight / (pow(dis, 2));
  //Vec normailize_toward(x_cor / dis, y_cor / dis);
  // real foce_y = (space_num * planet_num) * weight / (pow(dis, 2));
  


 
  

  //g += dt * Vector3(force * x_cor / dis, force * y_cor / dis, 0);




  // For all grid nodes
  for(int i = 0; i <= n; i++) {
    for(int j = 0; j <= n; j++) {
      auto &g = grid[i][j];
      // No need for epsilon here
      if (g[2] > 0) {
        // Normalize by mass
        g /= g[2];
        // Gravity
        //g += dt * Vector3(0, -200, 0);
        g += dt * Vector3(force * x_cor / dis, force * y_cor / dis, 0);

        // boundary thickness
        real boundary = 0.05;
        // Node coordinates
        real x = (real) i / n;
        real y = real(j) / n;

        // Sticky boundary
        if (x < boundary || x > 1-boundary || y > 1-boundary) {
          g = Vector3(0);
        }
        // Separate boundary
        if (y < boundary) {
          g[1] = std::max(0.0f, g[1]);
        }
      }
    }
  }


  int index = 0;
  // G2P
  for (auto &p : particles) {
    // element-wise floor
    
    Vector2i base_coord = (p.x * inv_dx - Vec(0.5f)).cast<int>();
    Vec fx = p.x * inv_dx - base_coord.cast<real>();
    Vec w[3] = {
                Vec(0.5) * sqr(Vec(1.5) - fx),
                Vec(0.75) - sqr(fx - Vec(1.0)),
                Vec(0.5) * sqr(fx - Vec(0.5))
    };

    p.C = Mat(0);
    p.v = Vec(0);

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        auto dpos = (Vec(i, j) - fx);
        auto grid_v = Vec(grid[base_coord.x + i][base_coord.y + j]);
        auto weight = w[i].x * w[j].y;
        // Velocity
        if (index >= start_index && index <= end_index)
             p.v += weight * grid_v;
      //  else
       //p.v = weight * Vec_zero;
        // APIC C
        p.C += 4 * inv_dx * Mat::outer_product(weight * grid_v, dpos);
      }
    }

    // Advection

    
    p.x += dt * p.v;

    // MLS-MPM F-update
    auto F = (Mat(1) + dt * p.C) * p.F;

    Mat svd_u, sig, svd_v;
    svd(F, svd_u, sig, svd_v);

    // Snow Plasticity
   /***********************************(3)*****************************************/
    if (p.ptype == 0) { p.F = Mat(1) * sqrt(determinant(F)); }
    else if (p.ptype == 1) { p.F = F; }
    else if (p.ptype == 2) {
        Mat svd_u, sig, svd_v; svd(F, svd_u, sig, svd_v);
        for (int i = 0; i < 2 * int(plastic); i++)                // Snow Plasticity
            sig[i][i] = clamp(sig[i][i], 1.0_f - 2.5e-2_f, 1.0_f + 7.5e-3_f);
        real oldJ = determinant(F); F = svd_u * sig * transposed(svd_v);
        real Jp_new = clamp(p.Jp * oldJ / determinant(F), 0.6_f, 20.0_f);
        p.Jp = Jp_new; p.F = F;
    }
    index++;
  }
}

// Seed particles with position and color
/***********************************(4)*****************************************/
void add_object(Vec center, int c,int num=1000, int ptype = 2,real length=0.08_f) {   // Seed particles with position and color
    for (int i = 0; i < num; i++)  // Randomly sample 1000 particles in the square
        particles.push_back(Particle((Vec::rand() * 2.0_f - Vec(1)) * length + center, c, Vec(0.0), ptype));
}
////////////////////////////////////////////////////////////////////////////////


void add_spacecraft(Vec center, real radius, int c, int ptype = 1, int num = 500, Vec velocity = Vec(0.0_f))
//input: circle center & radius, color, number of particles, initial velocity
{
    start_index = particles.size() ;
    int i = 0;
    while (i < num) {
        auto pos = (Vec::rand() * 2.0_f - Vec(1)) * radius;
        if (pos.x * pos.x + pos.y * pos.y < radius * radius) {
            
            particles.push_back(Particle(pos + center, c, velocity, ptype));
            i++;
        }
    }
    end_index = particles.size() - 1;
   
}


Vec get_space_pos()
{
    real sum_x = 0, sum_y = 0;
    Vec sum = Vec(0, 0);
    for (int i = start_index;i < end_index;i++)
    {
        sum_x += particles[start_index].x.x;
        sum_y += particles[start_index].x.y;
    }
    return Vec(sum_x / (end_index - start_index + 1), sum_y / (end_index - start_index + 1));   //确定质心
}




void add_object_circle(Vec center, real radius, int c, int ptype = 1, int num = 500, Vec velocity = Vec(0.0_f))
//input: circle center & radius, color, number of particles, initial velocity
{
    int i = 0;
    while (i < num) {
        auto pos = (Vec::rand() * 2.0_f - Vec(1)) * radius;
        if (pos.x * pos.x + pos.y * pos.y < radius * radius) {
            particles.push_back(Particle(pos + center, c, velocity,ptype));
            i++;
        }
    }
}


void add_horse(Vec center, int c, int num = 1000, int ptype = 1, real length = 0.08_f) {   // Seed particles with position and color
   
    int i = 0;
    while (i < num) {
        auto pos = Vec::rand();
        if (pos.y<center.y + length && pos.y>center.y )
        if (pos.x >center.x-length&&pos.x<center.x+length) {
              particles.push_back(Particle(pos, c, Vec(-2.0), ptype));          
            i++;
        }
        if(pos.y<center.y  && pos.y>center.y-length)
            if ((pos.x > center.x - length && pos.x < center.x - length / 2) || (pos.x > center.x + length / 2 && pos.x < center.x + length)) {
                particles.push_back(Particle(pos, c, Vec(0.0), ptype));
                i++;
            }
    }
}


void set_sea_level(real level, int c, int num = 5000, Vec velocity = Vec(0.0_f))
{
    
        int i = 0;
    while (i < num) {
        auto pos = Vec::rand();
        if (pos.x>0.05&&pos.x<0.95&&pos.y>0.05&&pos.y < level) {
            particles.push_back(Particle(pos, c, velocity, 0));
            i++;
        }
    }

}

void add_object_rectangle(Vec v1, Vec v2, int c, int num = 500, Vec velocity = Vec(0.0_f))
{
	Vec box_min(min(v1.x, v2.x), min(v1.y, v2.y)), box_max(max(v1.x, v2.x), max(v1.y, v2.y));
	int i = 0;
	while (i < num) {
		auto pos = Vec::rand();
		if (pos.x > box_min.x&&pos.y > box_min.y&&pos.x < box_max.x&&pos.y < box_max.y) {
			particles.push_back(Particle(pos, c, velocity,0));
			i++;
		}
	}
}

void add_object_wave(real bound1, real bound2, real level,real size,int c, int num = 500, Vec velocity = Vec(0.0_f))
{
    real bound_min(min(bound1,bound2)), bound_max(max(bound1, bound2));
    int i = 0;
    while (i < num) {
        auto pos = Vec::rand();
        if (pos.x > bound_min&& pos.x < bound_max && pos.y>level&&pos.y<level+size*(cos(pos.x/10)+1)) {
            particles.push_back(Particle(pos, c, velocity,0));
            i++;
        }
    }
}

void add_jet_rec() {
    add_object_rectangle(Vec(0.05, 0.05), Vec(0.06, 0.06), 0x87CEFA, 2, Vec(17.0, 0.0));
    //add_object_rectangle(Vec(0.5, 0.5), Vec(0.51, 0.51), 0x87CEFA, 10, Vec(0.0, -10.0));
}

void add_jet_wave() {
    add_object_wave(0.05,0.05+3.1415926/10,0.05_f,0.02_f ,0x87CEFA, 20, Vec(15.0, 0.0));
    //add_object_rectangle(Vec(0.5, 0.5), Vec(0.51, 0.51), 0x87CEFA, 10, Vec(0.0, -10.0));
}

int main() {
  GUI gui("Real-time 2D MLS-MPM", window_size, window_size);
  auto &canvas = gui.get_canvas();

  //add_object(Vec(0.55,0.45), 0xED553B,0);
  //add_object(Vec(0.85,0.35), 0xF2B134,20,1,0.02_f);
  //add_object(Vec(0.55,0.85), 0x068587,1);
  //set_sea_level(0.3_f, 0x87CEFA);
  //add_horse(Vec(0.55, 0.15), 0xED553B,1000,1);
  add_object_circle(Vec(0.5, 0.3), 0.05, 0xED553B,1, 1000, Vec(0.0, 0.0));
  //add_spacecraft(Vec(0.15, 0.85), 0.01, 0x068587, 1, 50, Vec(0.0, -2.835));
  add_spacecraft(Vec(0.15, 0.85), 0.01, 0x068587, 1, 50, Vec(0.0, -2));
  //add_spacecraft(Vec(0.15, 0.85), 0.01, 0x068587, 1, 50, Vec(0.0, -3));
  cout << "start index is" << start_index << "  ; end_index is " << end_index << endl;

  int frame = 0;

  // Main Loop
  for (int step = 0;; step++) {
    // Advance simulation
    
    advance(dt,get_space_pos(),500,1000,0.00001*1.05);

    // Visualize frame
    if (step % int(frame_dt / dt) == 0) {
      // Clear background
      canvas.clear(0x112F41);
      // Box
      canvas.rect(Vec(0.04), Vec(0.96)).radius(2).color(0x4FB99F).close();
      // Particles
      for (auto p : particles) {
        canvas.circle(p.x).radius(2).color(p.c);
      }
      // Update image
      gui.update();

      //if (step < 5e3&&(step%100)>=60&&step>0) add_jet_rec();
      // Write to disk (optional)
      // canvas.img.write_as_image(fmt::format("tmp/{:05d}.png", frame++));
    }
  }
}

/* -----------------------------------------------------------------------------
** Reference: A Moving Least Squares Material Point Method with Displacement
              Discontinuity and Two-Way Rigid Body Coupling (SIGGRAPH 2018)

  By Yuanming Hu (who also wrote this 88-line version), Yu Fang, Ziheng Ge,
           Ziyin Qu, Yixin Zhu, Andre Pradhana, Chenfanfu Jiang


** Build Instructions:

Step 1: Download and unzip mls-mpm88.zip (Link: http://bit.ly/mls-mpm88)
        Now you should have "mls-mpm88.cpp" and "taichi.h".

Step 2: Compile and run

* Linux:
    g++ mls-mpm88-explained.cpp -std=c++14 -g -lX11 -lpthread -O3 -o mls-mpm
    ./mls-mpm


* Windows (MinGW):
    g++ mls-mpm88-explained.cpp -std=c++14 -lgdi32 -lpthread -O3 -o mls-mpm
    .\mls-mpm.exe


* Windows (Visual Studio 2017+):
  - Create an "Empty Project"
  - Use taichi.h as the only header, and mls-mpm88-explained.cpp as the only source
  - Change configuration to "Release" and "x64"
  - Press F5 to compile and run


* OS X:
    g++ mls-mpm88-explained.cpp -std=c++14 -framework Cocoa -lpthread -O3 -o mls-mpm
    ./mls-mpm


** FAQ:
Q1: What does "1e-4_f" mean?
A1: The same as 1e-4f, a float precision real number.

Q2: What is "real"?
A2: real = float in this file.

Q3: What are the hex numbers like 0xED553B?
A3: They are RGB color values.
    The color scheme is borrowed from
    https://color.adobe.com/Copy-of-Copy-of-Core-color-theme-11449181/

Q4: How can I get higher-quality?
A4: Change n to 320; Change dt to 1e-5; Change E to 2e4;
    Change particle per cube from 500 to 8000 (Ln 72).
    After the change the whole animation takes ~3 minutes on my computer.

Q5: How to record the animation?
A5: Uncomment Ln 2 and 85 and create a folder named "tmp".
    The frames will be saved to "tmp/XXXXX.png".

    To get a video, you can use ffmpeg. If you already have taichi installed,
    you can simply go to the "tmp" folder and execute

      ti video 60

    where 60 stands for 60 FPS. A file named "video.mp4" is what you want.

Q6: How is taichi.h generated?
A6: Please check out my #include <taichi> talk:
    http://taichi.graphics/wp-content/uploads/2018/11/include_taichi.pdf
    and the generation script:
    https://github.com/yuanming-hu/taichi/blob/master/misc/amalgamate.py
    You can regenerate it using `ti amal`, if you have taichi installed.

Questions go to yuanming _at_ mit.edu
                            or https://github.com/yuanming-hu/taichi_mpm/issues.

                                                      Last Update: March 6, 2019
                                                      Version 1.5

----------------------------------------------------------------------------- */

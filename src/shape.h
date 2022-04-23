#define CUBE_SIZE 128

#define FLOAT_FACTOR 64 // instead of using floats, we ok to have precission of 1/64 & we know numbers*64 are within uint16_t

#define FAST_MATH 1

#if FAST_MATH == 1
#include "math_fast_int.h"

#define sin_fact sin_fast_int
#define cos_fact cos_fast_int
#define rad_fact radians_fast_int
#else
#define sin_fact(x) (sin(x) * FLOAT_FACTOR)
#define cos_fact(x) (cos(x) * FLOAT_FACTOR)
#define rad_fact(x) (radians(x))
#endif

extern uint16_t points[][2]; // will be declared later

void connectPoints(uint8_t i, uint8_t j, uint16_t points[][2]);

struct coord_3d { int16_t x; int16_t y; int16_t z; };
struct coord_3d_8 { int8_t x; int8_t y; int8_t z; };

#define SHAPE 2

#if SHAPE == 1

// TODO try to not store points, store shape property (angles) and calculate points when using each

#define NPOINTS 8

static coord_3d_8 orig_points[8] = {  // eight 3D points - set values for 3D cube
    {-1, -1, 1},
    {1, -1, 1},
    {1, 1, 1},
    {-1, 1, 1},
    {-1, -1, -1},
    {1, -1, -1},
    {1, 1, -1},
    {-1, 1, -1}};

void shape_init() {
  for (uint8_t i = 0; i < NPOINTS; ++i) {
    orig_points[i].x *= FLOAT_FACTOR;
    orig_points[i].y *= FLOAT_FACTOR;
    orig_points[i].z *= FLOAT_FACTOR;
  }
}

void shape_render() {
  // connect the lines between the individual points
  for (uint8_t i = 0; i < 4; i++) {
    connectPoints(i, (i + 1) % 4, points);
    connectPoints(4 + i, 4 + (i + 1) % 4, points);
    connectPoints(i, 4 + i, points);
  }
}

#elif SHAPE == 2

#define N_CIRCLES 8
#define N_C_POINTS 4

#define NPOINTS (N_C_POINTS * N_CIRCLES)
static coord_3d_8 orig_points[NPOINTS];

void rotate(int16_t angle_deg, uint8_t axis0, coord_3d &point_coords);

void shape_init() {
  for (uint8_t o = 0; o < N_CIRCLES; o++) {
    int16_t xc = (int16_t) (cos_fact(rad_fact(o * 360 / N_CIRCLES)));
    int16_t yc = (int16_t) (sin_fact(rad_fact(o * 360 / N_CIRCLES)));
    for (uint8_t i = 0; i < N_C_POINTS; i++) {
      coord_3d p {
         (int16_t) (sin_fact(rad_fact(i * 360 / N_C_POINTS)) / 3),
         (int16_t) (cos_fact(rad_fact(i * 360 / N_C_POINTS)) / 3),
         0
      };
      rotate(90, 0, p);
      rotate(o * 360 / N_CIRCLES, 2, p);
      orig_points[o * N_C_POINTS + i].x = xc + p.x;
      orig_points[o * N_C_POINTS + i].y = yc + p.y;
      orig_points[o * N_C_POINTS + i].z = p.z;
    }
  }
}

void shape_render() {
  for (uint8_t o = 0; o < N_CIRCLES; o++) {
    for (uint8_t i = 0; i < N_C_POINTS; i++) {
      connectPoints(o * N_C_POINTS + i, o * N_C_POINTS + (i + 1) % N_C_POINTS, points);
    }
  }
  for (uint8_t i = 0; i < N_C_POINTS; i++) {
    for (uint8_t o = 0; o < N_CIRCLES; o++) {
      connectPoints(o * N_C_POINTS + i, ((o + 1) % N_CIRCLES) * N_C_POINTS + i, points);
    }
  }
}

#endif

uint16_t points[NPOINTS][2];  // eight 2D points for the cube, values will be calculated in the code

int16_t angle_deg_0 = 60;  // rotation around the Y axis
int16_t angle_deg_1 = 60;  // rotation around the X axis
int16_t angle_deg_2 = 60;  // rotation around the Z axis
#define z_offset -4.0       // offset on Z axis
uint16_t time_frame;        // ever increasing time value

void rotate_pair(int16_t angle_deg, int16_t &coordA, int16_t &coordB) {
  // rotate 3d points in given 2-axis projection
  int16_t cos_val = (int16_t) (cos_fact(rad_fact(angle_deg)));
  int16_t sin_val = (int16_t) (sin_fact(rad_fact(angle_deg)));

  int16_t old_x = coordA;
  coordA = (coordA * cos_val - coordB * sin_val) / FLOAT_FACTOR;
  coordB = (coordB * cos_val + old_x * sin_val) / FLOAT_FACTOR;
}

/**
 * rotate point around given axis by given degree
 */
void rotate(int16_t angle_deg, uint8_t axis0, coord_3d &point_coords) {
  if (axis0 == 0) {
    rotate_pair(angle_deg, point_coords.y, point_coords.z);
  } else if (axis0 == 1) {
    rotate_pair(angle_deg, point_coords.x, point_coords.z);
  } else {
    rotate_pair(angle_deg, point_coords.x, point_coords.y);
  }
}

void shape_update() {
  time_frame = (time_frame + 1) % (50 * 360);  // to keep it cyclic

  angle_deg_0 = (angle_deg_0 + 3) % 360;
  angle_deg_1 = (angle_deg_1 + 5) % 360;
  angle_deg_2 = (angle_deg_2 + 7) % 360;

  // angle_deg_0 = angle_deg_1 = angle_deg_2 = 0;
}

// TODO replace with 3 integers - probably will be placed inside registers
coord_3d rotated_3d_point;  // eight 3D points - rotated around Y axis

void shape_calculate() {
  shape_update();
  int16_t sin_val = (int16_t) (sin_fact(rad_fact(time_frame) * 4));
  int16_t cube_size = (CUBE_SIZE * 4 / 5) + sin_val * (CUBE_SIZE / 8) / FLOAT_FACTOR;
  // cube_size = CUBE_SIZE * 3 / 2;

  // init points
  for (uint8_t i = 0; i < NPOINTS; ++i) {
    rotated_3d_point.x = orig_points[i].x;
    rotated_3d_point.y = orig_points[i].y;
    rotated_3d_point.z = orig_points[i].z;

    // rotate to current position
    rotate(angle_deg_0, 0, rotated_3d_point);
    rotate(angle_deg_1, 1, rotated_3d_point);
    // rotate(angle_deg_2, 2, rotated_3d_point);

    // calculate the points

    // project 3d points into 2d space with perspective divide -- 2D x = x/z,   2D y = y/z
    int16_t zRatio = rotated_3d_point.z + z_offset * FLOAT_FACTOR;
    points[i][0] = (CUBE_SIZE / 2) + (rotated_3d_point.x * cube_size / zRatio);
    points[i][1] = (CUBE_SIZE / 2) + (rotated_3d_point.y * cube_size / zRatio);
  }
}


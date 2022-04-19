#define CUBE_SIZE 128

#define FLOAT_FACTOR 64 // instead of using floats, we ok to have precission of 1/64 & we know numbers*64 are within uint16_t

void connectPoints(uint8_t i, uint8_t j, uint16_t points[][2]);

#define SHAPE 1

#if SHAPE == 1

#define NPOINTS 8

static int8_t orig_points[8][3] = {  // eight 3D points - set values for 3D cube
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
    orig_points[i][0] *= FLOAT_FACTOR;
    orig_points[i][1] *= FLOAT_FACTOR;
    orig_points[i][2] *= FLOAT_FACTOR;
  }
}

void shape_render(uint16_t points[][2]) {
  // connect the lines between the individual points
  for (uint8_t i = 0; i < 4; i++) {
    connectPoints(i, (i + 1) % 4, points);
    connectPoints(4 + i, 4 + (i + 1) % 4, points);
    connectPoints(i, 4 + i, points);
  }
}

#elif SHAPE == 2
#define NPOINTS 0
static const int8_t orig_points[NPOINTS][3];

void shape_init() {}

void shape_render(uint16_t points[][2]) {}

#endif

uint16_t points[NPOINTS][2];  // eight 2D points for the cube, values will be calculated in the code

uint16_t angle_deg_0 = 60;  // rotation around the Y axis
uint16_t angle_deg_1 = 60;  // rotation around the X axis
uint16_t angle_deg_2 = 60;  // rotation around the Z axis
#define z_offset -4.0       // offset on Z axis
uint16_t time_frame;        // ever increasing time value

/**
 * rotate point around given axis by given degree
 */
void rotate(uint16_t angle_deg, uint8_t axis0, int16_t point_coords[3]) {
  // rotate 3d points in given 2-axis projection
  uint8_t axis1 = axis0 == 1 ? 0 : 1;
  uint8_t axis2 = axis0 == 2 ? 0 : 2;
  int16_t cos_val = (int16_t) (cos(radians(angle_deg)) * FLOAT_FACTOR);
  int16_t sin_val = (int16_t) (sin(radians(angle_deg)) * FLOAT_FACTOR);

  int16_t axis0_coord = (point_coords[axis0] * cos_val - point_coords[axis2] * sin_val) / FLOAT_FACTOR;
  int16_t axis1_coord = point_coords[axis1];
  int16_t axis2_coord = (point_coords[axis0] * sin_val + point_coords[axis2] * cos_val) / FLOAT_FACTOR;
  point_coords[axis0] = axis0_coord;
  point_coords[axis1] = axis1_coord;
  point_coords[axis2] = axis2_coord;
}

void shape_update() {
  time_frame = (time_frame + 1) % (50 * 360);  // to keep it cyclic

  angle_deg_0 = (angle_deg_0 + 3) % 360;
  angle_deg_1 = (angle_deg_1 + 5) % 360;
  angle_deg_2 = (angle_deg_2 + 7) % 360;

#if DEBUG_ENABLED == 1
  angle_deg_0 = angle_deg_1 = angle_deg_2 = 0;
#endif
}

void shape_calculate() {
  shape_update();
  int16_t cube_size = (CUBE_SIZE * 2 / 3) + sin(time_frame * 0.05) * (CUBE_SIZE / 4);

  int16_t rotated_3d_points[3];  // eight 3D points - rotated around Y axis
  // init points
  for (uint8_t i = 0; i < NPOINTS; ++i) {
    rotated_3d_points[0] = orig_points[i][0];
    rotated_3d_points[1] = orig_points[i][1];
    rotated_3d_points[2] = orig_points[i][2];

    // rotate to current position
    rotate(angle_deg_0, 0, rotated_3d_points);
    rotate(angle_deg_1, 1, rotated_3d_points);
    rotate(angle_deg_2, 2, rotated_3d_points);

    // calculate the points

    // project 3d points into 2d space with perspective divide -- 2D x = x/z,   2D y = y/z
    int16_t zRatio = rotated_3d_points[2] + z_offset * FLOAT_FACTOR;
    points[i][0] = (CUBE_SIZE / 2) + (rotated_3d_points[0] * cube_size / zRatio);
    points[i][1] = (CUBE_SIZE / 2) + (rotated_3d_points[1] * cube_size / zRatio);
  }
}


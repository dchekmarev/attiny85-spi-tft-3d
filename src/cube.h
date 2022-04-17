#define CUBE_SIZE 64

#define NPOINTS 8

uint16_t points[NPOINTS][2];  // eight 2D points for the cube, values will be calculated in the code

static const int8_t orig_points[NPOINTS][3] = {  // eight 3D points - set values for 3D cube
    {-1, -1, 1},
    {1, -1, 1},
    {1, 1, 1},
    {-1, 1, 1},
    {-1, -1, -1},
    {1, -1, -1},
    {1, 1, -1},
    {-1, 1, -1}};

uint16_t angle_deg_0 = 60;  // rotation around the Y axis
uint16_t angle_deg_1 = 60;  // rotation around the X axis
uint16_t angle_deg_2 = 60;  // rotation around the Z axis
#define z_offset -4.0       // offset on Z axis
uint16_t time_frame;        // ever increasing time value

/**
 * rotate point around given axis by given degree
 */
void rotate(uint16_t angle_deg, uint8_t axis0, float rotated_3d_points[3]) {
  // rotate 3d points in given 2-axis projection
  uint8_t axis1 = axis0 == 1 ? 0 : 1;
  uint8_t axis2 = axis0 == 2 ? 0 : 2;
  float cos_val = cos(radians(angle_deg));
  float sin_val = sin(radians(angle_deg));

  float axis0_coord = rotated_3d_points[axis0] * cos_val - rotated_3d_points[axis2] * sin_val;
  float axis1_coord = rotated_3d_points[axis1];
  float axis2_coord = rotated_3d_points[axis0] * sin_val + rotated_3d_points[axis2] * cos_val;
  rotated_3d_points[axis0] = axis0_coord;
  rotated_3d_points[axis1] = axis1_coord;
  rotated_3d_points[axis2] = axis2_coord;
}

void cube_update() {
  time_frame = (time_frame + 1) % (50 * 360);  // to keep it cyclic

  angle_deg_0 = (angle_deg_0 + 3) % 360;
  angle_deg_1 = (angle_deg_1 + 5) % 360;
  angle_deg_2 = (angle_deg_2 + 7) % 360;

#if DEBUG_ENABLED == 1
  angle_deg_0 = angle_deg_1 = angle_deg_2 = 0;
#endif
}

void cube_calculate() {
  cube_update();
  uint16_t cube_size = (CUBE_SIZE - 15 - 1) + sin(time_frame * 0.2) * 15;

  float rotated_3d_points[3];  // eight 3D points - rotated around Y axis

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
    float zRatio = rotated_3d_points[2] + z_offset;
    points[i][0] = (CUBE_SIZE / 2) + (int16_t)(rotated_3d_points[0] * cube_size / zRatio);
    points[i][1] = (CUBE_SIZE / 2) + (int16_t)(rotated_3d_points[1] * cube_size / zRatio);
  }
}

void connectPoints(uint8_t i, uint8_t j, uint16_t points[][2]);

void cube_render(uint16_t points[][2]) {
  // connect the lines between the individual points
  for (uint8_t i = 0; i < 4; i++) {
    connectPoints(i, (i + 1) % 4, points);
    connectPoints(4 + i, 4 + (i + 1) % 4, points);
    connectPoints(i, 4 + i, points);
  }
}

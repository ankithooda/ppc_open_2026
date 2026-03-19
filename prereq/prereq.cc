
struct Result {
    float avg[3];
};

/*
This is the function you need to implement. Quick reference:
- x coordinates: 0 <= x < nx
- y coordinates: 0 <= y < ny
- horizontal position: 0 <= x0 < x1 <= nx
- vertical position: 0 <= y0 < y1 <= ny
- color components: 0 <= c < 3
- input: data[c + 3 * x + 3 * nx * y]
- output: avg[c]
*/
Result calculate(int ny, int nx, const float *data, int y0, int x0, int y1, int x1) {


  double sum[3] = {0.0};

    for (int x = x0; x < x1; x++) {
      for (int y = y0; y < y1; y++) {
        for (int c = 0; c < 3; c++) {
          sum[c] = sum[c] + (double)data[c + 3 * x + 3 * nx * y];
        }
      }
    }

    // Total pixels in the rectangle
    int l = (y1 - y0) * (x1 - x0);

    for (int i = 0; i < 3; i++) {
      sum[i] = sum[i] / l;
    }

    Result result{{(float)sum[0], (float)sum[1], (float)sum[2]}};

    return result;
}

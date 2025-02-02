//
// Created by fss on 23-1-5.
//

#include "image_util.hpp"

float Letterbox(const cv::Mat &image,
                cv::Mat &out_image,
                const cv::Size &new_shape,
                int stride,
                const cv::Scalar &color,
                bool fixed_shape,
                bool scale_up) {
  cv::Size shape = image.size();
  float r = std::min(
      (float) new_shape.height / (float) shape.height, (float) new_shape.width / (float) shape.width);
  if (!scale_up) {
    r = std::min(r, 1.0f);
  }

  int new_unpad[2]{
      (int) std::round((float) shape.width * r), (int) std::round((float) shape.height * r)};

  cv::Mat tmp;
  if (shape.width != new_unpad[0] || shape.height != new_unpad[1]) {
    cv::resize(image, tmp, cv::Size(new_unpad[0], new_unpad[1]));
  } else {
    tmp = image.clone();
  }

  float dw = new_shape.width - new_unpad[0];
  float dh = new_shape.height - new_unpad[1];

  if (!fixed_shape) {
    dw = (float) ((int) dw % stride);
    dh = (float) ((int) dh % stride);
  }

  dw /= 2.0f;
  dh /= 2.0f;

  int top = int(std::round(dh - 0.1f));
  int bottom = int(std::round(dh + 0.1f));
  int left = int(std::round(dw - 0.1f));
  int right = int(std::round(dw + 0.1f));
  cv::copyMakeBorder(tmp, out_image, top, bottom, left, right, cv::BORDER_CONSTANT, color);
  return 1.0f / r;
}

template<typename T>
T clip(const T &n, const T &lower, const T &upper) {
  return std::max(lower, std::min(n, upper));
}

void ScaleCoords(const cv::Size &img_shape, cv::Rect &coords, const cv::Size &img_origin_shape) {
  float gain = std::min((float) img_shape.height / (float) img_origin_shape.height,
                        (float) img_shape.width / (float) img_origin_shape.width);

  int pad[2] = {(int) (((float) img_shape.width - (float) img_origin_shape.width * gain) / 2.0f),
                (int) (((float) img_shape.height - (float) img_origin_shape.height * gain) / 2.0f)};

  coords.x = (int) std::round(((float) (coords.x - pad[0]) / gain));
  coords.y = (int) std::round(((float) (coords.y - pad[1]) / gain));

  coords.width = (int) std::round(((float) coords.width / gain));
  coords.height = (int) std::round(((float) coords.height / gain));

  coords.x = clip(coords.x, 0, img_origin_shape.width);
  coords.y = clip(coords.y, 0, img_origin_shape.height);
  coords.width = clip(coords.width, 0, img_origin_shape.width);
  coords.height = clip(coords.height, 0, img_origin_shape.height);
}


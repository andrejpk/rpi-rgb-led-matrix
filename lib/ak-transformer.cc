#include <assert.h>

#include "ak-transformer.h"


namespace rgb_matrix {
    
/***********************************/
/* Large Square Transformer Canvas */
/***********************************/
class Rect64x32Transformer::TransformCanvas : public Canvas {
public:
  TransformCanvas() : delegatee_(NULL) {}

  void SetDelegatee(Canvas* delegatee);

  virtual void Clear();
  virtual void Fill(uint8_t red, uint8_t green, uint8_t blue);
  virtual int width() const;
  virtual int height() const;
  virtual void SetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);

private:
  Canvas *delegatee_;
};

void Rect64x32Transformer::TransformCanvas::SetDelegatee(Canvas* delegatee) {
  // Our assumptions of the underlying geometry:
  // assert(delegatee->height() == 16);
  assert(delegatee->width() == 128);

  delegatee_ = delegatee;
}

void Rect64x32Transformer::TransformCanvas::Clear() { 
  delegatee_->Clear(); 
}

void Rect64x32Transformer::TransformCanvas::Fill(uint8_t red, uint8_t green, uint8_t blue) {
  delegatee_->Fill(red, green, blue);
}

int Rect64x32Transformer::TransformCanvas::width() const { 
  return 64; 
}

int Rect64x32Transformer::TransformCanvas::height() const { 
  return 32; 
}

void Rect64x32Transformer::TransformCanvas::SetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) {
  if (x < 0 || x >= width() || y < 0 || y >= height()) return;
  x = width() - x - 1;  
  // y = height() - y;
  // We have up to column 64 one direction, then folding around. Lets map
  if (y >= 16) {
    x = 127 - x;
    y = y - 16;
  }
  else
  {
      // x = 128 - x;
      y = 15 - y;
  }
  delegatee_->SetPixel(x, y, red, green, blue);
}

/****************************/
/* Large Square Transformer */
/****************************/
Rect64x32Transformer::Rect64x32Transformer()
  : canvas_(new TransformCanvas()) {
}

Rect64x32Transformer::~Rect64x32Transformer() {
  delete canvas_;
}

Canvas *Rect64x32Transformer::Transform(Canvas *output) {
  assert(output != NULL);
  
  canvas_->SetDelegatee(output);
  return canvas_;
}

} // namespace rgb_matrix


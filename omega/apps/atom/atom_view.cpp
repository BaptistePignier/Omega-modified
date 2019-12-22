#include "atom_view.h"
#include <assert.h>
#include <string.h>
#include <poincare/integer.h>
#include <poincare/number.h>
#include <escher/palette.h>


#include <apps/i18n.h>
#include "atom_defs.h"

#define ATOM_VIEW_VARS_NUM 4

namespace Atom {

AtomView::AtomView() :
  View()
{
}

void AtomView::drawAtom(KDContext * ctx, uint8_t id) const {
  KDColor fill = Palette::AtomColor[(uint8_t)atomsdefs[id].type];

  if (atomsdefs[id].y >= 7) {
    ctx->fillRect(KDRect(6 + atomsdefs[id].x * 17, 15 + atomsdefs[id].y * 17, 18, 18), fill);
    ctx->strokeRect(KDRect(6 + atomsdefs[id].x * 17, 15 + atomsdefs[id].y * 17, 18, 18), Palette::AtomTableLines);
    
    ctx->drawString(atomsdefs[id].symbol, KDPoint(8 + atomsdefs[id].x * 17, 17 + atomsdefs[id].y * 17), KDFont::SmallFont, Palette::AtomText, fill);
  } else {
    ctx->fillRect(KDRect(6 + atomsdefs[id].x * 17, 6 + atomsdefs[id].y * 17, 18, 18), fill);
    ctx->strokeRect(KDRect(6 + atomsdefs[id].x * 17, 6 + atomsdefs[id].y * 17, 18, 18), Palette::AtomTableLines);
    
    ctx->drawString(atomsdefs[id].symbol, KDPoint(8 + atomsdefs[id].x * 17, 8 + atomsdefs[id].y * 17), KDFont::SmallFont, Palette::AtomText, fill);
  }
}

uint8_t cursor_pos = 2;
bool partial_draw = false;
bool copy_mode = false;
uint8_t copy_cursor_pos = 0;

void AtomView::handleLeft() {
  if (cursor_pos > 0) {
    cursor_pos--;
    partial_draw = true;
    markRectAsDirty(bounds());
  }
}

void AtomView::handleRight() {
  if (cursor_pos < ATOM_NUMS - 1) {
    cursor_pos++;
    partial_draw = true;
    markRectAsDirty(bounds());
  }
}

void AtomView::handleUp() {
  
  if (copy_mode) {
    
    if (copy_cursor_pos > 0)
      copy_cursor_pos--;
    
    partial_draw = true;
    markRectAsDirty(bounds());
    return;
  }
  
  uint8_t curr_x = atomsdefs[cursor_pos].x;
  uint8_t curr_y = atomsdefs[cursor_pos].y;
  bool updated = false;
  
  if (curr_y > 0 && curr_y <= 9) {
    for(uint8_t i = 0; i < ATOM_NUMS; i++) {
      if (atomsdefs[i].x == curr_x && atomsdefs[i].y == curr_y - 1) {
        cursor_pos = i;
        updated = true;
        break;
      }
    }
  }
  
  if (updated) {
    partial_draw = true;
    markRectAsDirty(bounds());
  }
}

void AtomView::handleDown() {
  
  if (copy_mode) {
    
    if (copy_cursor_pos < ATOM_VIEW_VARS_NUM-1)
      copy_cursor_pos++;
    
    partial_draw = true;
    markRectAsDirty(bounds());
    return;
  }
  
  uint8_t curr_x = atomsdefs[cursor_pos].x;
  uint8_t curr_y = atomsdefs[cursor_pos].y;
  bool updated = false;
  
  if (curr_y >= 0 && curr_y < 9) {
    for(uint8_t i = 0; i < ATOM_NUMS; i++) {
      if (atomsdefs[i].x == curr_x && atomsdefs[i].y == curr_y + 1) {
        cursor_pos = i;
        updated = true;
        break;
      }
    }
  }
  
  if (updated) {
    partial_draw = true;
    markRectAsDirty(bounds());
  }
}

bool AtomView::handleOK() {
  if (copy_mode) {
    copy_mode = false;
    char buffer[12];
    
    switch(copy_cursor_pos) {
    case 0:
      Poincare::Integer(atomsdefs[cursor_pos].num).serialize(buffer, 4);
      break;
    case 1:
      Poincare::Integer(atomsdefs[cursor_pos].neutrons + atomsdefs[cursor_pos].num).serialize(buffer, 4);
      break;
    case 2:
      Poincare::Number::FloatNumber(atomsdefs[cursor_pos].mass).serialize(buffer, 11);
      break;
    case 3:
      Poincare::Number::FloatNumber(atomsdefs[cursor_pos].electroneg).serialize(buffer, 11);
      break;
    default:
      break;
    }
    
    Clipboard::sharedClipboard()->store(buffer);
    partial_draw = true;
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

void AtomView::handleCopy() {
  copy_mode = true;
  copy_cursor_pos = 0;
  partial_draw = true;
  markRectAsDirty(bounds());
}

bool AtomView::handleBack() {
  if (copy_mode) {
    copy_mode = false;
    partial_draw = true;
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

void AtomView::drawRect(KDContext * ctx, KDRect rect) const {
  if (partial_draw) {
    partial_draw = false;
    ctx->fillRect(KDRect(50,   0, 169, 57), Palette::BackgroundHard);
    ctx->fillRect(KDRect( 8, 170, 305, 35), Palette::BackgroundHard);
  } else {
    ctx->fillRect(bounds(), Palette::BackgroundHard);
  }
  
  for(uint8_t i = 0; i < ATOM_NUMS; i++) {
    AtomView::drawAtom(ctx, i);
  }
  
  if (!copy_mode) {
    if (atomsdefs[cursor_pos].y >= 7) {
      ctx->strokeRect(KDRect(6 + atomsdefs[cursor_pos].x * 17, 15 + atomsdefs[cursor_pos].y * 17, 18, 18), Palette::PrimaryText);
      ctx->strokeRect(KDRect(7 + atomsdefs[cursor_pos].x * 17, 16 + atomsdefs[cursor_pos].y * 17, 16, 16), Palette::PrimaryText);
    } else {
      ctx->strokeRect(KDRect(6 + atomsdefs[cursor_pos].x * 17, 6 + atomsdefs[cursor_pos].y * 17, 18, 18), Palette::PrimaryText);
      ctx->strokeRect(KDRect(7 + atomsdefs[cursor_pos].x * 17, 7 + atomsdefs[cursor_pos].y * 17, 16, 16), Palette::PrimaryText);
    }
  }
  
  ctx->fillRect(KDRect(48,  99, 2, 61), Palette::AtomTableLines);
  ctx->fillRect(KDRect(48, 141, 9,  2), Palette::AtomTableLines);
  ctx->fillRect(KDRect(48, 158, 9,  2), Palette::AtomTableLines);

  char protons[4];
  char nucleons[4];
  
  Poincare::Integer(atomsdefs[cursor_pos].num).serialize(protons, 4);
  Poincare::Integer(atomsdefs[cursor_pos].neutrons + atomsdefs[cursor_pos].num).serialize(nucleons, 4);
  
  ctx->drawString(atomsdefs[cursor_pos].symbol, KDPoint(73, 23), KDFont::LargeFont, Palette::PrimaryText, Palette::BackgroundHard);
  ctx->drawString(I18n::translate(atomsdefs[cursor_pos].name), KDPoint(110, 27), KDFont::SmallFont, Palette::PrimaryText, Palette::BackgroundHard);
  
  // , KDColorBlack, Palette::SelectDark
  
  if (copy_mode && copy_cursor_pos == 0)
    ctx->drawString(nucleons, KDPoint(50, 17), KDFont::SmallFont, Palette::PrimaryText, Palette::SelectDark);
  else
    ctx->drawString(nucleons, KDPoint(50, 17), KDFont::SmallFont, Palette::PrimaryText, Palette::BackgroundHard);
    
  if (copy_mode && copy_cursor_pos == 1)
    ctx->drawString(protons, KDPoint(50, 31), KDFont::SmallFont, Palette::PrimaryText, Palette::SelectDark);
  else
    ctx->drawString(protons, KDPoint(50, 31), KDFont::SmallFont, Palette::PrimaryText, Palette::BackgroundHard);
  
  char buffer[20];
  
  memset(buffer, ' ', 19);
  buffer[19] = 0;
  
  buffer[0] = 'M';
  buffer[1] = ':';
  
  int num = Poincare::Number::FloatNumber(atomsdefs[cursor_pos].mass).serialize(buffer + 5, 13);
  buffer[5  + num] = 0;
  
  if (copy_mode && copy_cursor_pos == 2)
    ctx->drawString(buffer, KDPoint(8, 174), KDFont::SmallFont, Palette::PrimaryText, Palette::SelectDark);
  else
    ctx->drawString(buffer, KDPoint(8, 174), KDFont::SmallFont, Palette::PrimaryText, Palette::BackgroundHard);
  
  
  memset(buffer, ' ', 19);
  buffer[19] = 0;
  
  
  buffer[0] = 'K';
  buffer[1] = 'h';
  buffer[2] = 'i';
  buffer[3] = ':';
  
  if (atomsdefs[cursor_pos].electroneg == -1) {
    buffer[5] = 'N';
    buffer[6] = '/';
    buffer[7] = 'A';
    buffer[8] = 0;
  } else {
    num = Poincare::Number::FloatNumber(atomsdefs[cursor_pos].electroneg).serialize(buffer + 5, 13);
    buffer[5  + num] = 0;
  }
  
  
  if (copy_mode && copy_cursor_pos == 3)
    ctx->drawString(buffer, KDPoint(8, 188), KDFont::SmallFont, Palette::PrimaryText, Palette::SelectDark);
  else
    ctx->drawString(buffer, KDPoint(8, 188), KDFont::SmallFont, Palette::PrimaryText, Palette::BackgroundHard);
}

}

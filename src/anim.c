/*
  anim.c -- animation module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mininim.h"

/* set to true to quit animation */
enum quit_anim quit_anim;
bool pause_anim;
bool cutscene;
bool next_frame_inv;
uint64_t anim_cycle;

struct anim *anima;
size_t anima_nmemb;

void
play_anim (void (*draw_callback) (void),
           void (*compute_callback) (void),
           int freq)
{
  quit_anim = NO_QUIT;

  acknowledge_resize ();
  ALLEGRO_EVENT event;
  ALLEGRO_TIMER *timer = create_timer (1.0 / freq);
  ALLEGRO_EVENT_QUEUE *event_queue = create_event_queue ();
  al_register_event_source (event_queue, get_display_event_source (display));
  al_register_event_source (event_queue, get_keyboard_event_source ());
  al_register_event_source (event_queue, get_timer_event_source (video_timer));
  al_register_event_source (event_queue, get_timer_event_source (timer));
  al_start_timer (timer);

  while (! quit_anim) {
    al_wait_for_event (event_queue, &event);
    switch (event.type) {
    case ALLEGRO_EVENT_TIMER:
      if (event.timer.source == timer) {
        if (was_key_pressed (ALLEGRO_KEY_ESCAPE, 0, ALLEGRO_KEYMOD_ALT, false))
          pause_anim = true;
        if (was_key_pressed (ALLEGRO_KEY_ESCAPE, 0, ALLEGRO_KEYMOD_CTRL, true))
          pause_anim = false;

        kid_debug ();

        if (! is_video_effect_started ()) show ();
        if (! pause_anim
            || was_key_pressed (ALLEGRO_KEY_ESCAPE, 0,
                                ALLEGRO_KEYMOD_ALT, true)) {
          if (compute_callback) compute_callback ();
          clear_bitmap (uscreen, TRANSPARENT_COLOR);
          draw_callback ();
          draw_bottom_text (uscreen, NULL);
          play_samples ();
          anim_cycle++;
        }
        drop_all_events_from_source
          (event_queue, get_timer_event_source (timer));
        al_set_timer_count (timer, 0);
      } else if (event.timer.source == video_timer) {
        show ();
        /* drop_all_events_from_source */
        /*   (event_queue, get_timer_event_source (video_timer)); */
        /* al_set_timer_count (video_timer, 0); */
      }
      break;
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      acknowledge_resize ();
      show ();
      break;
    case ALLEGRO_EVENT_DISPLAY_EXPOSE:
      show ();
      break;
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
      quit_anim = QUIT_GAME;
      break;
    case ALLEGRO_EVENT_KEY_CHAR:
      key = event;

      char *text = NULL;

      /* CTRL+R: restart game */
      if (was_key_pressed (ALLEGRO_KEY_R, 0, ALLEGRO_KEYMOD_CTRL, true))
        quit_anim = RESTART_GAME;

      /* CTRL+Q: quit game */
      if (was_key_pressed (ALLEGRO_KEY_Q, 0, ALLEGRO_KEYMOD_CTRL, true))
        quit_anim = QUIT_GAME;

      /* CTRL+V: show engine name and version */
      if (was_key_pressed (ALLEGRO_KEY_V, 0, ALLEGRO_KEYMOD_CTRL, true)) {
        xasprintf (&text, "MININIM 0.9");
        draw_bottom_text (NULL, text);
        al_free (text);
      }

      /* CTRL+S: enable/disable sound */
      if (was_key_pressed (ALLEGRO_KEY_S, 0, ALLEGRO_KEYMOD_CTRL, true)) {
        audio_enabled = ! audio_enabled;
        enable_audio (audio_enabled);
        xasprintf (&text, "SOUND %s", audio_enabled ? "ON" : "OFF");
        draw_bottom_text (NULL, text);
        al_free (text);
      }

      /* SHIFT+I: flip screen */
      if (was_key_pressed (ALLEGRO_KEY_I, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
        char *flip = "NONE";
        switch (screen_flags) {
        case 0:
          screen_flags = ALLEGRO_FLIP_VERTICAL;
          flip = "VERTICAL"; break;
        case ALLEGRO_FLIP_VERTICAL:
          screen_flags = ALLEGRO_FLIP_HORIZONTAL;
          flip = "HORIZONTAL"; break;
        case ALLEGRO_FLIP_HORIZONTAL:
          screen_flags = ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL;
          flip = "VERTICAL + HORIZONTAL"; break;
        case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
          screen_flags = 0;
          break;
        }
        xasprintf (&text, "DISPLAY FLIP: %s", flip);
        draw_bottom_text (NULL, text);
        al_free (text);
      }

      /* SHIFT+K: flip keyboard */
      if (was_key_pressed (ALLEGRO_KEY_K, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
        char *flip = "NONE";
        if (! flip_keyboard_vertical && ! flip_keyboard_horizontal) {
          flip_keyboard_vertical = true;
          flip = "VERTICAL";
        } else if (flip_keyboard_vertical && ! flip_keyboard_horizontal) {
          flip_keyboard_vertical = false;
          flip_keyboard_horizontal = true;
          flip = "HORIZONTAL";
        } else if (! flip_keyboard_vertical && flip_keyboard_horizontal) {
          flip_keyboard_vertical = true;
          flip = "VERTICAL + HORIZONTAL";
        } else if (flip_keyboard_vertical && flip_keyboard_horizontal) {
          flip_keyboard_vertical = false;
          flip_keyboard_horizontal = false;
        }
        xasprintf (&text, "KEYBOARD FLIP: %s", flip);
        draw_bottom_text (NULL, text);
        al_free (text);
      }

      /* F12: change video mode */
      if (was_key_pressed (ALLEGRO_KEY_F12, 0, 0, true)) {
        char *vm_str = NULL;

        switch (vm) {
        case CGA:
          if (hgc) {
            hgc = false; vm = VGA; vm_str = "VGA";
          } else {
            hgc = true; vm = CGA; vm_str = "HGC";
          }
          break;
        case EGA: vm = CGA; vm_str = "CGA"; break;
        case VGA: vm = EGA; vm_str = "EGA"; break;
        }

        xasprintf (&text, "VIDEO MODE: %s", vm_str);
        draw_bottom_text (NULL, text);
        al_free (text);
      }

      break;
    }
  }

  al_stop_timer (timer);
}




int
create_anim (struct anim *a0, enum anim_type t, struct pos *p, enum dir dir)
{
  struct anim a;
  int i = anima_nmemb;
  memset (&a, 0, sizeof (a));

  if (a0) a = *a0;
  else {
    a.type = t;
    a.original_type = t;
    a.f.dir = dir;
    a.f.c.room = p->room;
    a.controllable = false;
  }

  a.id = i;

  switch (a.type) {
  case NO_ANIM: default: break;
  case KID: create_kid (a0, &a, p, dir); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    create_guard (a0, &a, p, dir); break;
  case MOUSE: create_mouse (a0, &a, p, dir); break;
  }

  anima = add_to_array (&a, 1, anima, &anima_nmemb, i, sizeof (a));
  anima[i].f.id = &anima[i];
  return i;
}

void
destroy_anim (struct anim *a)
{
  forget_enemy (a);

  if (a->type == KID) destroy_kid (a);

  size_t i =  a - anima;
  anima = remove_from_array (anima, &anima_nmemb, i, 1, sizeof (*a));
}

void
destroy_anims (void)
{
  while (anima_nmemb) destroy_anim (&anima[0]);
  anima = NULL;
  anima_nmemb = 0;
}

struct anim *
get_anim_by_id (int id)
{
  int i;
  if (id == -1) return NULL;
  for (i = 0; i < anima_nmemb; i++)
    if (anima[i].id == id) return &anima[i];
  return NULL;
}

void
draw_anim_frame (ALLEGRO_BITMAP *bitmap, struct anim *a, enum vm vm)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: draw_kid_frame (bitmap, a, vm); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    draw_guard_frame (bitmap, a, vm); break;
  case MOUSE: draw_mouse_frame (bitmap, a, vm); break;
  }
}

void
draw_anims (ALLEGRO_BITMAP *bitmap, enum em em, enum vm vm)
{
  struct coord ml; struct pos pml, pmlr, pmlra;
  struct anim *a;

  /* coord_wa = true; */

  qsort (anima, anima_nmemb, sizeof (*a), compare_anims);

  size_t i;
  for (i = 0; i < anima_nmemb; i++) {
    a = &anima[i];
    if (a->invisible) continue;
    a->f.id = a;

    _ml (&a->f, &ml); pos (&ml, &pml);
    prel (&pml, &pmlr, 0, +1);
    prel (&pml, &pmlra, -1, +1);

    draw_anim_frame (bitmap, a, vm);

    draw_falling_loose_floor (bitmap, &pmlr, em, vm);
    draw_falling_loose_floor (bitmap, &pmlra, em, vm);
    draw_room_anim_fg (bitmap, em, vm, a);
    a->xf.b = NULL;
  }

  /* coord_wa = false; */
}

int
compare_anims (const void *_a0, const void *_a1)
{
  struct coord bl0, bl1;

  struct anim *a0 = (struct anim *) _a0;
  struct anim *a1 = (struct anim *) _a1;

  _bl (&a0->f, &bl0);
  _bl (&a1->f, &bl1);

  coord2room (&bl1, bl0.room, &bl1);

  if (bl0.room < bl1.room) return -1;
  if (bl0.room > bl1.room) return 1;

  struct coord o = {bl0.room,0,ORIGINAL_HEIGHT};

  double d0 = dist_coord (&o, &bl0);
  double d1 = dist_coord (&o, &bl1);

  if (d0 < d1) return -1;
  if (d1 < d0) return 1;

  return 0;
}

/* int */
/* compare_anims (const void *a0, const void *a1) */
/* { */
/*   struct coord nc; */
/*   struct pos np, ptl0, ptl1, ptr0, ptr1, */
/*     pbl0, pbl1, pbr0, pbr1; */

/*   struct anim *_a0 = (struct anim *) a0; */
/*   struct anim *_a1 = (struct anim *) a1; */

/*   survey (_br, pos, &_a0->f, &nc, &np, &pbr0); */
/*   survey (_br, pos, &_a1->f, &nc, &np, &pbr1); */

/*   survey (_bl, pos, &_a0->f, &nc, &np, &pbl0); */
/*   survey (_bl, pos, &_a1->f, &nc, &np, &pbl1); */

/*   survey (_tr, pos, &_a0->f, &nc, &np, &ptr0); */
/*   survey (_tr, pos, &_a1->f, &nc, &np, &ptr1); */

/*   survey (_tl, pos, &_a0->f, &nc, &np, &ptl0); */
/*   survey (_tl, pos, &_a1->f, &nc, &np, &ptl1); */

/*   int cptr = cpos (&ptr0, &ptr1); */
/*   if (cptr && ptr0.room == ptr1.room) return cptr; */

/*   int cpbr = cpos (&pbr0, &pbr1); */
/*   if (cpbr && pbr0.room == pbr1.room) return cpbr; */

/*   int cptl = cpos (&ptl0, &ptl1); */
/*   if (cptl && ptl0.room == ptl1.room) return cptl; */

/*   int cpbl = cpos (&pbl0, &pbl1); */
/*   if (cpbl && pbl0.room == pbl1.room) return cpbl; */

/*   if (ptl0.room < ptl1.room) return -1; */
/*   if (ptl0.room > ptl1.room) return 1; */

/*   return 0; */
/* } */

void
draw_anim_if_at_pos (ALLEGRO_BITMAP *bitmap, struct anim *a, struct pos *p,
                     enum vm vm)
{
  struct coord nc;
  struct pos np, pbl, pmbo, pbr, pml, pm, pmr, ptl, pmt, ptr;
  survey (_bl, pos, &a->f, &nc, &pbl, &np);
  survey (_mbo, pos, &a->f, &nc, &pmbo, &np);
  survey (_br, pos, &a->f, &nc, &pbr, &np);
  survey (_ml, pos, &a->f, &nc, &pml, &np);
  survey (_m, pos, &a->f, &nc, &pm, &np);
  survey (_mr, pos, &a->f, &nc, &pmr, &np);
  survey (_tl, pos, &a->f, &nc, &ptl, &np);
  survey (_mt, pos, &a->f, &nc, &pmt, &np);
  survey (_tr, pos, &a->f, &nc, &ptr, &np);

  if (! peq (&pbl, p)
      && ! peq (&pmbo, p)
      && ! peq (&pbr, p)
      && ! peq (&pml, p)
      && ! peq (&pm, p)
      && ! peq (&pmr, p)
      && ! peq (&ptl, p)
      && ! peq (&pmt, p)
      && ! peq (&ptr, p)) return;

  draw_anim_frame (bitmap, a, vm);
}

void
clear_anims_keyboard_state (void)
{
  int i;
  for (i = 0; i < anima_nmemb; i++)
    memset (&anima[i].key, 0, sizeof (anima[i].key));
}

bool
is_anim_dead (struct frame *f)
{
  return is_kid_dead (f)
    || is_guard_dead (f);
}

bool
is_anim_chopped (struct frame *f)
{
  return is_kid_chopped (f)
    || is_guard_chopped (f);
}

bool
is_anim_fall (struct frame *f)
{
  return is_kid_fall (f)
    || is_guard_fall (f);
}

void
anim_die_suddenly (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_die_suddenly (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die_suddenly (a); break;
  }
}

void
anim_die_spiked (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_die_spiked (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die_spiked (a); break;
  }
}

void
anim_die_chopped (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_die_chopped (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die_chopped (a); break;
  }
}

void
anim_die (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_die (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die (a); break;
  }
}

void
anim_sword_hit (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_sword_hit (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_hit (a); break;
  }
}

void
anim_fall (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_fall (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_fall (a); break;
  }
}

void
anim_walkf (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_sword_walkf (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_walkf (a); break;
  }
}

void
anim_walkb (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_sword_walkb (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_walkb (a); break;
  }
}

struct coord *
splash_coord (struct frame *f, struct coord *c)
{
  int w = al_get_bitmap_width (v_kid_splash);
  int h = al_get_bitmap_height (v_kid_splash);
  int fw = al_get_bitmap_width (f->b);
  int fh = al_get_bitmap_height (f->b);
  c->x = f->c.x + (fw / 2) - (w / 2);
  c->y = f->c.y + (fh / 2) - (h / 2);
  c->room = f->c.room;
  return c;
}





void
draw_frame (ALLEGRO_BITMAP *bitmap, struct frame *f)
{
  if (! f->b) return;

  draw_bitmapc (f->b, bitmap, &f->c, f->flip);
}

void
draw_xframe (ALLEGRO_BITMAP *bitmap, struct frame *f, struct frame_offset *xf)
{
  struct coord c;
  if (! xf->b) return;
  draw_bitmapc (xf->b, bitmap, xframe_coord (f, xf, &c), f->flip);
}

struct coord *
xframe_coord (struct frame *f, struct frame_offset *xf, struct coord *c)
{
  int w = al_get_bitmap_width (xf->b);
  _tf (f, c);
  c->x += (f->dir == LEFT) ? xf->dx : -xf->dx - w + 1;
  c->y += xf->dy;
  c->room = f->c.room;
  return c;
}

struct frame *
xframe_frame (struct frame *f, struct frame_offset *xf, struct frame *nf)
{
  *nf = *f;
  nf->b = xf->b;
  xframe_coord (f, xf, &nf->c);
  return nf;
}

void
select_frame (struct anim *a, struct frameset *fs, int i)
{
  a->fo.b = fs[i].frame;
  a->fo.dx = fs[i].dx;
  a->fo.dy = fs[i].dy;
  a->i = i;
}

void
select_xframe (struct frame_offset *xf, struct frameset *fs, int j)
{
  xf->b = fs[j].frame;
  xf->dx = fs[j].dx;
  xf->dy = fs[j].dy;
  /* a->j = j; */
}

struct frame *
next_frame (struct frame *f, struct frame *nf, struct frame_offset *fo)
{
  *nf = *f;

  nf->oc = f->c;
  nf->ob = f->b;

  int ow = al_get_bitmap_width (nf->b);
  int oh = al_get_bitmap_height (nf->b);
  int w = al_get_bitmap_width (fo->b);
  int h = al_get_bitmap_height (fo->b);

  if (next_frame_inv) nf->c.x += (nf->dir == LEFT) ? ow - w - fo->dx : fo->dx;
  else nf->c.x += (nf->dir == LEFT) ? fo->dx : ow - w - fo->dx;
  nf->c.y += oh - h + fo->dy;

  nf->b = fo->b;
  if (! cutscene) nframe (nf, &nf->c);
  return nf;
}

struct frame *
prev_frame (struct frame *f, struct frame *pf)
{
  *pf = *f;
  pf->c = f->oc;
  pf->b = f->ob;
  return pf;
}

bool
wait_anim (int cycles)
{
  static int i = 0;
  if (i == 0) i = cycles + 1;
  return --i;
}

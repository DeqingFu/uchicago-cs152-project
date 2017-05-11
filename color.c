#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "raytracer-project2.h"

color *color_new(double r, double g, double b)
{
  //todo("color_new");
  if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1) { 
    fprintf(stderr, "warning: invalid inputs\n");
    return NULL; /* <-- unreachable */
  } else {
    color* c = (color*)malloc(sizeof(color));
    if (c == NULL) {
    fprintf(stderr, "malloc fails\n");
    exit(1);
  }
    c->r = r;
    c->g = g;
    c->b = b;
    return c;
  }
}

char *color_format = "(r=%lf,g=%lf,b=%lf)";

char *color_tos(color *c)
{
  char buf[256];
  sprintf(buf,color_format,c->r,c->g,c->b);
  return strdup(buf);
}

void color_show(FILE *f, color *c)
{
  fprintf(f,color_format,c->r,c->g,c->b);
}

void fix(double* comp)
{
  *comp = *comp > 1 ? 1 : *comp;
}

color   *color_add(color *c1, color *c2)
{
  double new_r = c1->r + c2->r;
  double new_g = c1->g + c2->g;
  double new_b = c1->b + c2->b;
  fix(&new_r);
  fix(&new_g);
  fix(&new_b);
  return color_new(new_r, new_g, new_b);
}

color   *color_modulate(color *c1, color *c2)
{
  double r_mod = c1->r * c2->r;
  double g_mod = c1->g * c2->g;
  double b_mod = c1->b * c2->b;
  fix(&r_mod);
  fix(&g_mod);
  fix(&b_mod);
  return color_new(r_mod, g_mod, b_mod);
}

color   *color_scale(double scalar, color *c)
{
  double r_scl = scalar * c->r;
  double g_scl = scalar * c->g;
  double b_scl = scalar * c->b;
  fix(&r_scl);
  fix(&g_scl);
  fix(&b_scl);
  return color_new(r_scl, g_scl, b_scl);
}
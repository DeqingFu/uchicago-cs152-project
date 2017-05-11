#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "utils.h"
#include "raytracer-project2.h"

vector3 *vector3_new(double x, double y, double z)
{
  vector3 *v = (vector3*)malloc(sizeof(vector3));
  check_malloc("vector3_new",v);
  v->x = x;
  v->y = y;
  v->z = z;
  return v;
}

char *vector_format = "<%lf,%lf,%lf>";

vector3 *vector3_add(vector3 *v1, vector3 *v2)
{
  if (v1 == NULL || v2 == NULL) {
    fprintf(stderr, "Input Invalid\n");
    exit(1);
  }
  vector3* v = (vector3*)malloc(sizeof(vector3));
  if (v == NULL) {
    fprintf(stderr, "malloc fails\n");
    exit(1);
  }
  v->x = v1->x + v2->x;
  v->y = v1->y + v2->y;
  v->z = v1->z + v2->z;
  return v;
}

vector3 *vector3_sub(vector3 *v1, vector3 *v2)
{
  if (v1 == NULL || v2 == NULL) {
    fprintf(stderr, "Input Invalid\n");
    exit(1);
  }
  vector3* v = (vector3*)malloc(sizeof(vector3));
  if (v == NULL) {
    fprintf(stderr, "malloc fails\n");
    exit(1);
  }
  v->x = v1->x - v2->x;
  v->y = v1->y - v2->y;
  v->z = v1->z - v2->z;
  return v; 
}

vector3 *vector3_negate(vector3 *v)
{
  if (v == NULL) {
    fprintf(stderr, "Input Invalid\n");
    exit(1);
  }
  vector3* v_neg = (vector3*)malloc(sizeof(vector3));
  if (v_neg == NULL) {
    fprintf(stderr, "malloc fails\n");
    exit(1);
  }
  v_neg->x = - v->x;
  v_neg->y = - v->y;
  v_neg->z = - v->z;
  return v_neg;
}

vector3 *vector3_scale(double scalar, vector3 *v)
{
  if (v == NULL) {
    fprintf(stderr, "Input Invalid\n");
    exit(1);
  }
  vector3* vec = (vector3*)malloc(sizeof(vector3));
  if (vec == NULL) {
    fprintf(stderr, "malloc fails\n");
    exit(1);
  }
  vec->x = scalar * v->x;
  vec->y = scalar * v->y;
  vec->z = scalar * v->z;
  return vec;
}

double vector3_dot(vector3 *v1, vector3 *v2)
{
  if (v1 == NULL || v2 == NULL) {
    fprintf(stderr, "Input Invalid\n");
    exit(1);
  }
  return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

double vector3_magnitude(vector3 *v)
{
  if (v == NULL) {
    fprintf(stderr, "Invalid Input\n");
    exit(1);
  }
  return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

vector3 *vector3_normalize(vector3 *v)
{
  double magn = vector3_magnitude(v);
  vector3* v_n = (vector3*)malloc(sizeof(vector3));
  if (v_n == NULL) {
    fprintf(stderr, "malloc fails\n");
    exit(1);
  }
  v_n->x = v->x / magn;
  v_n->y = v->y / magn;
  v_n->z = v->z / magn;
  return v_n;
}

void vector3_normify(vector3 *v)
{
  v = vector3_normalize(v);
}

char *vector3_tos(vector3 *v)
{
  char buf[256]; /* way more than enough, but chars are cheap */  
  sprintf(buf,vector_format,v->x,v->y,v->z);
  return strdup(buf);
}

void vector3_show(FILE *f, vector3 *v)
{
  fprintf(f,vector_format,v->x,v->y,v->z);
}

ray3 *ray3_new(vector3 *origin, vector3 *direction)
{
  ray3* r = (ray3*)malloc(sizeof(ray3));
  if (r == NULL) {
    fprintf(stderr, "malloc fails\n");
    exit(1);
  }
  r->origin = origin;
  r->direction = direction;
  return r;
}

void ray3_free(ray3 *r)
{
  free(r->origin);
  free(r->direction);
  free(r);
}

vector3 *ray3_position(ray3 *r, double t)
{
  vector3 *t1 = vector3_scale(t,r->direction);
  vector3 *t2 = vector3_add(r->origin,t1);
  free(t1);
  return t2;
}

char *ray3_tos(ray3 *r)
{
  char buf[256];
  sprintf(buf, "The origin of ray3 is %s\nThe direction of ray3 is %s\n", 
    vector3_tos(r->origin), vector3_tos(r->direction));
  return strdup(buf);
}

void ray3_show(FILE *f, ray3 *r)
{
  fprintf(f, "The origin of ray3 is %s\nThe direction of ray3 is %s\n", 
    vector3_tos(r->origin), vector3_tos(r->direction));
}

vector3 *logical_coord(uint image_height, uint image_width, 
  uint pixel_row, uint pixel_col)
{
  vector3* coord = (vector3*)malloc(sizeof(vector3));
  if (coord == NULL) {
    fprintf(stderr, "malloc fails\n");
    exit(1);
  }
  if (image_height < image_width) {
      double unit = 2.0 / image_width;
      coord->x = - 1.0 + (pixel_col * unit) + (unit / 2.0);
      coord->y = (1.0 - unit * (image_width - image_height) / 2.0) - 
      (pixel_row * unit + unit / 2.0);
      coord->z = 0.0;
  } else {
    double unit = 2.0 / image_height;
    coord->x = -1.0 + ((image_height - image_width) / 2.0 + pixel_col) * unit + 
    unit / 2.0; 
    coord->y = 1.0 - pixel_row * unit - unit / 2.0;
    coord->z = 0.0;
  }
  return coord;
}

int within_boundary(vector3* v, rectangle* r)
{
  int up;
  int down;
  int letf;
  int right;
  vector3* ul = r->upper_left;
  double upperbound = ul->y;
  double lowerbound = ul->y - r->h;
  double leftbound  = ul->x;
  double rightbound = ul->x + r->w;
  double x = v->x;
  double y = v->y;
  up = y < upperbound ? 1 : 0;
  down = y > lowerbound ? 1 : 0;
  letf = x > leftbound ? 1 : 0;
  right = x < rightbound ? 1 : 0;
  return up * down * letf * right;
}

hit *intersect(ray3 *r, object *obj)
{
  vector3* ro = r->origin;
  vector3* rd = r->direction;
  hit* h = (hit*)malloc(sizeof(hit));
  if (h == NULL) {
    fprintf(stderr, "malloc fails\n");
    exit(1);
  }
  if (obj->tag == SPHERE){
      sphere* s = (obj->o).s;
      vector3* center = s->center;
      double r = s->radius;
      vector3* a = vector3_sub(ro, center);
      double b = vector3_dot(a, rd);
      double c = vector3_dot(a, a) - r * r;
      double d = b * b - c;
      if (d <= 0) {
        h = NULL;
      } else {
        double t = - b - sqrt(d);
        h->shine = s->shine;
        h->t = t;
        vector3 *t1 = vector3_scale(t, rd);
        //vector3 *t2 = vector3_add(ro, t1);
        //free(t1);
        //vector3* p = t2;
        vector3* p = vector3_add(ro, t1);
        free(t1);
        if (t > 0) {
          color* color;
          if(s->surf.tag == CONSTANT){
            color = s->surf.c.k;
          } else if (s->surf.tag == FUNCTION) {
            color = (*(s->surf.c.f))(center, p);
          } else {
            fprintf(stderr, "warning: no such tag\n");
            exit(1);
          }
          h->surface_color = color;
          h->surface_normal = vector3_normalize(vector3_sub(p, center));
          //free(p); 
        } else {
          h = NULL;
        }
        free(p);
      }
      free(a);
    } else if (obj->tag == RECTANGLE){ 
      rectangle* r = (obj->o).r;
      int d = (r->upper_left)->z;
      vector3* n = vector3_new(0.0, 0.0, -1.0);
      double numerator = -(vector3_dot(ro, n) + d);
      double denominator = vector3_dot(rd, n);
      double t = numerator / denominator;
      if (t <= 0) {
        h = NULL;
      } else {
        vector3 *t1 = vector3_scale(t, rd);
        vector3 *t2 = vector3_add(ro, t1);
        free(t1);
        vector3* p = t2;
        if (within_boundary(p, r)) {
          h->shine = r->shine;
          h->t = t;
          if (r->surf.tag == CONSTANT){
            h->surface_color = r->surf.c.k;
          } else if (r->surf.tag == FUNCTION) {
            h->surface_color = (*(r->surf.c.f))(r->upper_left, p);
          } else {
            fprintf(stderr, "warning: no such tag\n");
            exit(1);
          }
          h->surface_normal = n;
        } else {
          h = NULL;
        }
        free(p);
      }
    } else {
      fprintf(stderr, "No such object\n");
      exit(1);
  }
  return h;
}


color* bg_color(ray3* r, scene* s)
{
  if (s->bg.tag == CONSTANT) {
    return s->bg.c.k;
  } else if (s->bg.tag == FUNCTION){
    color*(*f)(vector3*,vector3*) = s->bg.c.f;
    vector3* origin = r->origin;
    vector3* dir = r->direction;
    double t = - origin->z / dir->z;
    double x = origin->x + t * dir->x;
    double y = origin->y + t * dir->y;
    vector3* loc = vector3_new(x, y, 0);
    return (*f)(origin, loc);
  } else {
    return NULL;
  }
}

/* using Shaw's reference */
void hit_free(hit *h)
{
  if (h) {
    //free(h->surface_color);
    free(h->surface_normal);
    //free(h->shine);
    free(h);
  }
}

int in_shadow(vector3 *loc, light *dl, object_list *objs)
{
  vector3 *nudge = vector3_scale(0.0001, dl->direction); 
  vector3 *lifted = vector3_add(loc, nudge);
  vector3* normalize_dir = vector3_normalize(dl->direction);
  ray3* r = ray3_new(lifted, normalize_dir);
  object_list* current = objs;
  while (current != NULL) {
    hit* inter = intersect(r, &(current->first));
    if (inter){
      hit_free(inter);
      return 1;
    }
    hit_free(inter);
    current = current->rest;
  }
  free(nudge);
  ray3_free(r);
  return 0;
}

color *light_color(scene *s, ray3 *r, hit *h)
{
  if (h == NULL){
    return bg_color(r, s);
  }
  color* k = color_new(0, 0, 0);
  vector3* n = h->surface_normal;
  vector3* scl_tmp = vector3_scale(h->t, r->direction);
  vector3* loc = vector3_add(r->origin, scl_tmp);
  free(scl_tmp);
  color* amb = s->amb_light;
  color* surf_col = h->surface_color;
  if (in_shadow(loc, s->dir_light, s->objects)) {
    k = color_modulate(amb, surf_col);
    free(loc);
    return k;
  } else {
    vector3* d_vec = vector3_normalize(s->dir_light->direction);
    double prod = vector3_dot(n, d_vec);
    double m = prod > 0 ? prod : 0;
    color* diffuse = color_scale(m, s->dir_light->color);
    color* add_temp = color_add(amb, diffuse);
    k = color_modulate(add_temp, surf_col);
    free(add_temp);
    color* d;
    vector3* l = vector3_normalize(s->dir_light->direction);
    color* shine = h->shine;
    double n_dot_l = vector3_dot(n, l);
    double scalar = 2.0 * n_dot_l;
    vector3* r_scl_tmp2 = vector3_scale(scalar, n);
    vector3* r_vec = vector3_sub(r_scl_tmp2, l);
    free(r_scl_tmp2);
    vector3* v = vector3_negate(r->direction);
    if (n_dot_l > 0) {
      double r_dot_v = vector3_dot(r_vec, v);
      double m = r_dot_v > 0 ? r_dot_v : 0;
      d = color_scale(pow(m, 6), shine);
    } else {
      d = color_new(0.0, 0.0, 0.0);
    }
    free(d_vec);
    free(diffuse);
    free(l);
    free(r_vec);
    free(v);
    free(loc);
    return color_add(k, d);
  }
}

color *trace_ray(ray3 *r, scene *s)
{
  object_list* list = s->objects;
  color* c;
  double t = DBL_MAX;
  hit* h = (hit*)malloc(sizeof(hit));
  int i = 0; /* for the sake of freeing hit */
  while(list != NULL) {
    object obj = list->first;
    hit* tmp = intersect(r, &obj);
    if (tmp != NULL){
      if (tmp->t < t){
        if (i) hit_free(h); //not free the first h
        t = tmp->t;
        h = tmp;
        list = list->rest;
        i ++;
      } else {
        hit_free(tmp);
        list = list->rest;
      }
    } else {
      list = list->rest;
    }
  }
  if (t == DBL_MAX) {
    //c = s->background_color;
    c = bg_color(r, s);
  } else {
    c = light_color(s, r, h);
  }
  free(h);
  return c;
}
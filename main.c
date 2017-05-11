#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "raytracer-project2.h"

void render_ppm(FILE *f, environment *e)
{
  uint h = e->image_height;
  uint w = e->image_width;
  vector3* camera = vector3_new(0.0, 0.0, e->camera_z);
  scene* scn = e->scene;
  fprintf(f, "P3\n");
  fprintf(f, "%d %d\n", w, h);
  fprintf(f, "255\n");
  for (int i = 0; i < h; i ++) {
    for (int j = 0; j < w; j ++) {
      vector3* logic = logical_coord(h, w, i, j);
      vector3* dir = vector3_sub(logic, camera);
      free(logic);
      vector3* dir_n = vector3_normalize(dir);
      free(dir);
      ray3* ray = ray3_new(camera, dir_n);
      free(dir_n);
      color* color = trace_ray(ray, scn);
      int r = (int)(color->r * 255);
      int g = (int)(color->g * 255);
      int b = (int)(color->b * 255);
      fprintf(f, "%d %d %d\n", r, g, b);
      //free(color);
      free(ray);
    }
  }
  free(camera);
}

/* some convenience constructors for objects, etc. */

surface surf_const(double r, double g, double b)
{
  surface s;
  s.tag = CONSTANT;
  s.c.k = color_new(r,g,b);
  return s;
}

surface surf_fn(color*(*f)(vector3*,vector3*))
{
  surface s;
  s.tag = FUNCTION;
  s.c.f = f;
  return s;
}

/* create a container object for a sphere */
object *obj_sph(sphere *s)
{
  if (!s) {
    fprintf(stderr,"obj_sph given NULL\n");
    exit(1);
  }
  object *o = (object*)malloc(sizeof(object));
  check_malloc("obj_sph",o);
  o->tag = SPHERE;
  o->o.s = s;
  return o;
}

/* create a container object for a rectangle */
object *obj_rect(rectangle *r)
{
  if (!r) {
    fprintf(stderr,"obj_rect given NULL\n");
    exit(1);
  }
  object *o = (object*)malloc(sizeof(object));
  check_malloc("obj_rect",o);
  o->tag = RECTANGLE;
  o->o.r = r;
  return o;
}

/* private internal sphere constructor that leaves color slot uninitialized */
sphere *sph(double cx, double cy, double cz, double r, double sr, double sg, double \
	    sb)
{
  sphere *s = (sphere*)malloc(sizeof(sphere));
  check_malloc("sph",s);
  s->center = vector3_new(cx,cy,cz);
  if (r<0) {
    fprintf(stderr,"sph: r<0 (r=%lf)\n",r);
    exit(1);
  }
  s->radius = r;
  s->shine = color_new(sr,sg,sb);
  return s;
}

/* solid-color sphere constructor */
object *sphere_new(double cx, double cy, double cz,
                   double r,
                   double cr, double cg, double cb,
                   double sr, double sg, double sb)
{
  sphere *s = sph(cx,cy,cz,r,sr,sg,sb);
  s->surf   = surf_const(cr,cg,cb);
  return obj_sph(s);
}

/* private internal rectangle constructor that leaves color slot uninitialized */
rectangle *rect(double ulx, double uly, double ulz,
                double w, double h,
                double sr, double sg, double sb)
{
  rectangle *r = (rectangle*)malloc(sizeof(rectangle));
  check_malloc("rect",r);
  r->upper_left = vector3_new(ulx,uly,ulz);
  if (w<0) {
    fprintf(stderr,"rectangle_new: negative width (%lf)\n",w);
    exit(1);
  }
  r->w = w;
  if (h<0) {
    fprintf(stderr,"rectangle_new: negative height (%lf)\n",h);
    exit(1);
  }
  r->h = h;
  r->shine = color_new(sr,sg,sb);
  return r;
}

/* solid-color rectangle constructor */
object *rectangle_new(double ulx, double uly, double ulz,
                      double w, double h,
                      double cr, double cg, double cb,
                      double sr, double sg, double sb)
{
  rectangle *r = rect(ulx,uly,ulz,w,h,sr,sg,sb);
  r->surf = surf_const(cr,cg,cb);
  return obj_rect(r);
}

/* shallow-copy object list cons */
object_list *cons(object *o, object_list *os)
{
  object_list *l = (object_list*)malloc(sizeof(object_list));
  check_malloc("cons",l);
  l->first = *o;
  l->rest  = os;
  return l;
}

/* (mostly) shallow-copy scene constructor */
scene *scene_new(color *bg, color *amb, light *dl, object_list *objs)
{
  if (!bg || !amb || !dl) {
    fprintf(stderr,"scene_new: unexpected NULL\n");
    exit(1);
  }
  scene *sc = (scene*)malloc(sizeof(scene));
  check_malloc("scene_new",sc);
  sc->bg.tag = CONSTANT;
  sc->bg.c.k = color_new(bg->r,bg->g,bg->b);
  sc->amb_light = amb;
  sc->dir_light = dl;
  sc->objects = objs;
  return sc;
}

/* dl_new: new directional light */
/* note: direction vector need not be a unit vector, it is normalized here */
light *dl_new(double x, double y, double z, double r, double g, double b)
{
  light *dl = (light*)malloc(sizeof(light));
  check_malloc("dl_new",dl);
  dl->direction = vector3_new(x,y,z);  
  vector3_normify(dl->direction);
  dl->color = color_new(r,g,b);
  return dl;
}

/* shallow copy environment constructor */
environment *environment_new(double z, uint w, uint h, scene *sc)
{
  environment *e = (environment*)malloc(sizeof(environment));
  check_malloc("environment_new",e);
  e->camera_z = z;
  e->image_width = w;
  e->image_height = h;
  e->scene = sc;
  return e;					
}

/* *** stubs *** */
int is_pre(char* pre, char* buf)
{
  //printf("done\n");
  int len = strlen(pre);
  int bool = 1;
  for (int i = 0; i < len; i ++) {
    if (pre[i] - buf[i]) {
      bool = 0;
      break;
    } 
  }
  return bool;
}

environment *read_env()
{
  char buf[512];
  double r, g, b;
  double x, y, z;
  double rs, gs, bs;
  //environment* env = (environment*)malloc(sizeof(environment));
  scene* s = (scene*)malloc(sizeof(scene));
  s->amb_light = (color*)malloc(sizeof(color));
  s->dir_light = (light*)malloc(sizeof(light));
  s->objects = (object_list*)malloc(sizeof(object_list));
  s->objects = NULL;
  unsigned int h, w;
  unsigned int image_height, image_width;
  double cam;
  while (fgets(buf, 512, stdin) != NULL) {
    if (is_pre("BG",buf)) {
      //fprintf(stderr, "%s\n", "done bg");
      sscanf(buf,"BG %lf %lf %lf",&r,&g,&b);
      s->bg.tag = CONSTANT;
      s->bg.c.k = color_new(r,g,b);
    } else if (is_pre("AMB",buf)) {
      //fprintf(stderr, "%s\n", "done amb");
      sscanf(buf,"AMB %lf %lf %lf",&r,&g,&b);
      s->amb_light = color_new(r, g, b);
    } else if (is_pre("ENV", buf)) {
      //fprintf(stderr, "%s\n", "done env");
      sscanf(buf, "ENV %lf %u %u", &cam, &image_width, &image_height);
    } else if (is_pre("DL", buf)) {
      //fprintf(stderr, "%s\n", "done dl");
      sscanf(buf, "DL %lf %lf %lf %lf %lf %lf", &x, &y, &z, &r, &g, &b);
      s->dir_light->direction = vector3_new(x, y, z);
      s->dir_light->color = color_new(r, g, b);
    } else if (is_pre("SPHERE", buf)) {
      //fprintf(stderr, "%s\n", "done sphere");
      double radius;
      sscanf(buf, "SPHERE %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
        &x, &y, &z, &radius, &r, &g, &b, &rs, &gs, &bs);
      object* sphere = sphere_new(x, y, z, radius, r, g, b, rs, gs, bs);
      s->objects = cons(sphere, s->objects);
    } else if (is_pre("RECTANGLE", buf)) {
      //fprintf(stderr, "%s\n", "done rectangle");
      sscanf(buf, "RECTANGLE %lf %lf %lf %u %u %lf %lf %lf %lf %lf %lf",
        &x, &y, &z, &w, &h, &r, &g, &b, &rs, &gs, &bs);
      object* rectangle = rectangle_new(x,y,z,w,h,r,g,b,rs,gs,bs);
      s->objects = cons(rectangle, s->objects);
    } else {
      if (!strcmp(buf, " \n")){
        fprintf(stderr,"skipping \"%s\"\n",buf);
      }
    }
  }
  return environment_new(cam, image_width, image_height, s);
}

/* ***free operations*** */
void light_free(light* l)
{
  free(l->color);
  free(l->direction);
  free(l);
}

void surf_free(surface* surf)
{
  switch (surf->tag){
    case CONSTANT:
      free(surf->c.k);
      break;
    case FUNCTION:
      break;
    default:
      break;
  }
}
void sphere_free(sphere* sph)
{
  free(sph->center);
  surf_free(&sph->surf);
  free(sph->shine);
  free(sph);
}

void rect_free(rectangle* rect)
{
  free(rect->upper_left);
  surf_free(&rect->surf);
  free(rect->shine);
  free(rect);
}

void obj_free(object* obj)
{
  switch(obj->tag){
    case SPHERE:
      sphere_free(obj->o.s);
      break;
    case RECTANGLE:
      rect_free(obj->o.r);
      break;
    default:
      break;
  }
}

void objects_free(object_list* objs)
{
  object_list* curr = objs;
  while (curr != NULL) {
    obj_free(&curr->first);
    curr = curr->rest;
  }
  free(objs);
}

void scene_free(scene* s)
{
  free(s->amb_light);
  light_free(s->dir_light);
  objects_free(s->objects);
  free(s);
}

void env_free(environment *e)
{
  scene_free(e->scene);
  free(e);
}

/* test case 2 */
color *sphere_color_fn1(vector3 *c, vector3 *hp)
{
  double r = sin((hp->x+hp->y+hp->z)*16);
  double d = r/2.0+0.5;
  return color_new(d/2.0,d/1.5,d);
}

color *sphere_color_fn2(vector3 *c, vector3 *hp)
{
  double r = cos((hp->x+hp->y*hp->z)*2);
  double d = r/2.0+0.5;
  return color_new(1.0,d/1.5,d/1.1);
}

color *sunset(vector3 *ro, vector3 *vp)
{
  double grad = (1.0 - -vp->y)/2.0;
  return color_new((1.0-grad)/1.5,0.0,grad/2.0);
}

object* function_sphere(color*(*f)(vector3*, vector3*), double x, double y, 
                        double z, double r, double rs, double gs, double bs)
{
  sphere* sphere = sph(x, y, z, r, rs, gs, bs);
  sphere->surf = surf_fn(f);
  return obj_sph(sphere); 
}

scene* function_scene(color*(*f)(vector3*, vector3*), color* amb_light, 
                      light* dir_light, object_list* objs)
{
  scene* s = (scene*)malloc(sizeof(scene));
  s->bg = surf_fn(f);
  s->amb_light = amb_light;
  s->dir_light = dir_light;
  s->objects = objs;
  return s;
}

/* *** main program *** */
int main(int argc, char *argv[]) 
{
  if (argc==2 && !strcmp(argv[1],"1")) {
    /* n.b. WHITE sphere (so you can tell this apart from other similar scenes) */
    object *sphere0    = sphere_new(1,0,3,0.6,1,1,1,0,0,0);
    object *rectangle0 = rectangle_new(1,1.3,4,1,2.5,0,0,1,0,0,0);
    object_list *objs0 = cons(sphere0,cons(rectangle0,NULL));
    scene *scene0      = scene_new(color_new(0.8,0.8,0.8),
       color_new(0.2,0.2,0.2),
       dl_new(-1,1,-1,1,1,1),
       objs0);
    environment *env0  = environment_new(-3.3,600,400,scene0);
    render_ppm(stdout,env0);
    env_free(env0);
  } else if (argc==2 && !strcmp(argv[1],"2")) {
    /* two spheres with stripes, one blue and another red 
     * background is "sunset" */
    object* sphere_fn_1 = function_sphere(sphere_color_fn1, 
                                          -0.6,0.2,13,1.1,0.8,0.8,0.8);
    object* sphere_fn_2 = function_sphere(sphere_color_fn2,
                                          1.4,-0.15,16,1.1,0.8,0.8,0.8);
    object_list* objs_fn = cons(sphere_fn_2, cons(sphere_fn_1, NULL));
    scene* scene_fn = function_scene(sunset, color_new(0.2,0.2,0.2), 
                                    dl_new(-1,1,-1,1,1,1), objs_fn);
    environment* env_fn = environment_new(-3.3, 800, 240, scene_fn);
    render_ppm(stdout, env_fn);
    env_free(env_fn);
  } else if (argc==2 && !strcmp(argv[1],"3")) {
    uint scene_w = 1200;
    uint scene_h = 800;
    srand(time(NULL));
    object_list *objs = NULL;
    for (double i=-3; i<3; i+=.6) {
      for (double j=-2.5; j<2.5; j+=.5) {
        double rand10 = ((double)rand()/(double)(RAND_MAX)) * 10;
        double r = (double)rand()/(double)(RAND_MAX);
        double g = (double)rand()/(double)(RAND_MAX);
        double b = (double)rand()/(double)(RAND_MAX);
        object *sphere = sphere_new(i,j,rand10+5,rand10/10+.01,r,g,b,1,1,1);
        objs = cons(sphere, objs);
      }
    }
    scene *scene0 = scene_new(color_new(0.8,0.8,0.8),
           color_new(0.2,0.2,0.2),
           dl_new(-1,1,-1,1,1,1),
           objs);
    environment *env0  = environment_new(-3.3,scene_w,scene_h,scene0);
    render_ppm(stdout,env0);
    env_free(env0);
  }
  else if (argc==1) {
    environment *e = read_env();
    render_ppm(stdout,e);
    env_free(e);
  }
  return 0;
}

#version 330 core
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 1

struct PointLight {
  vec3 pos;
  vec3 color;
  float a;
  float b;
  float intensity;
};
uniform int point_light_count;
uniform PointLight point_lights[MAX_POINT_LIGHTS];

struct SpotLight {
  vec3 pos;
  vec3 color;
  vec3 direction;
  float outer_cone_angle;
  float inner_cone_angle;
  float intensity;
};
uniform int spot_light_count;
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

struct DirectionalLight {
  vec3 pos;
  vec3 color;
  vec3 direction;
  float intensity;
};
uniform int directional_light_count;
uniform DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];

out vec4 FragColor;

in vec2 texture_coord;
in vec3 normal;
in vec3 position;

uniform vec3 scale = vec3(1.0, 1.0, 1.0);
uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
uniform float specular_strength = 0.5;
uniform vec3 camera_position;
uniform vec3 ambient_color = vec3(1.0, 1.0, 1.0);
uniform float ambient_strength = 0.2;
uniform bool unlit = false;

vec3 get_scaled_normal() { return normalize(normal * scale); }

vec4 point_light() {
  vec3 n = normalize(get_scaled_normal());
  vec3 view_dir = normalize(camera_position - position);

  vec4 result = vec4(0.0);

  for (int i = 0; i < point_light_count; i++) {
    vec3 light_vec = point_lights[i].pos - position;
    float distance = length(light_vec);
    float point_intensity = 1.0 / (point_lights[i].a * distance * distance +
                                   point_lights[i].b * distance + 1.0);
    vec3 light_dir = normalize(light_vec);

    float diffuse = max(dot(get_scaled_normal(), light_dir), 0.0);

    vec3 reflect_dir = reflect(-light_dir, get_scaled_normal());
    vec3 halfway = normalize(view_dir + light_dir);
    float specular = pow(max(dot(get_scaled_normal(), halfway), 0.0), 8);

    vec3 dif_res = diffuse * vec3(texture(diffuse_texture, texture_coord));
    if (diffuse == 0.0) {
      specular = 0.0;
    }
    vec3 spec_res = specular_strength * specular *
                    vec3(texture(specular_texture, texture_coord));

    result += vec4(point_lights[i].color *
                       (dif_res * point_intensity * point_lights[i].intensity *
                            vec3(color) +
                        spec_res * point_intensity * point_lights[i].intensity),
                   1.0);
  }

  vec3 amb_res = ambient_strength *
                 vec3(texture(diffuse_texture, texture_coord)) * ambient_color;

  return result + vec4(amb_res, 1.0) * color;
}

vec4 spot_light() {
  vec3 n = normalize(get_scaled_normal());
  vec3 view_dir = normalize(camera_position - position);

  vec4 result = vec4(0.0);

  for (int i = 0; i < spot_light_count; i++) {
    float cos_inner = cos(spot_lights[i].inner_cone_angle);
    float cos_outer = cos(spot_lights[i].outer_cone_angle);

    vec3 light_dir = normalize(spot_lights[i].pos - position);

    float diffuse = max(dot(get_scaled_normal(), light_dir), 0.0);
    vec3 reflect_dir = reflect(-light_dir, get_scaled_normal());
    vec3 halfway = normalize(view_dir + light_dir);
    float specular = pow(max(dot(get_scaled_normal(), halfway), 0.0), 8);

    vec3 dif_res = diffuse * vec3(texture(diffuse_texture, texture_coord));
    if (diffuse == 0.0) {
      specular = 0.0;
    }
    vec3 spec_res = specular_strength * specular *
                    vec3(texture(specular_texture, texture_coord));

    float angle = dot(spot_lights[i].direction, -light_dir);
    float spot_intensity =
        clamp((angle - cos_outer) / (cos_inner - cos_outer), 0.0, 1.0);

    result += vec4(
        spot_lights[i].color *
            (dif_res * spot_intensity * spot_lights[i].intensity * vec3(color) +
             spec_res * spot_intensity * spot_lights[i].intensity),
        1.0);
  }

  vec3 amb_res = ambient_strength *
                 vec3(texture(diffuse_texture, texture_coord)) * ambient_color;

  return result + vec4(amb_res, 1.0) * color;
}

vec4 directional_light() {
  vec3 n = normalize(get_scaled_normal());
  vec3 view_dir = normalize(camera_position - position);

  vec4 result = vec4(0.0);

  for (int i = 0; i < directional_light_count; i++) {
    vec3 light_dir = normalize(directional_lights[i].direction);

    float diffuse = max(dot(get_scaled_normal(), light_dir), 0.0);
    vec3 reflect_dir = reflect(-light_dir, get_scaled_normal());
    vec3 halfway = normalize(view_dir + light_dir);
    float specular = pow(max(dot(get_scaled_normal(), halfway), 0.0), 8);

    vec3 dif_res = diffuse * vec3(texture(diffuse_texture, texture_coord));
    if (diffuse == 0.0) {
      specular = 0.0;
    }
    vec3 spec_res = specular_strength * specular *
                    vec3(texture(specular_texture, texture_coord));

    result = vec4(directional_lights[i].color *
                      (dif_res * directional_lights[i].intensity * vec3(color) +
                       spec_res * directional_lights[i].intensity),
                  1.0);
  }

  vec3 amb_res = ambient_strength *
                 vec3(texture(diffuse_texture, texture_coord)) * ambient_color;

  return result + vec4(amb_res, 1.0) * color;
}

void main() {
  if (texture(diffuse_texture, texture_coord).a < 0.1)
    discard;

  if (unlit == false) {
    vec4 lighting = point_light() + spot_light() + directional_light();
    FragColor = lighting;
  } else {
    FragColor = texture(diffuse_texture, texture_coord);
  }
}
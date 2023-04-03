#version 330 core

struct Material {
    vec3 ambient, diffuse, specular;
    float shine;
};

struct PointLight {
    vec3 pos, color;

    float constant, linear, quadratic;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
out vec4 color;

uniform vec3 E;
uniform Material mat;
uniform sampler2D DiffuseMap0;

vec3 pointLight(PointLight light, vec3 normal, vec3 eyeDir, vec3 diffuseColor) {
    vec3 lightDir = normalize(light.pos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient  = diffuseColor;
    vec3 diffuse  = diffuseColor * max(dot(normal, lightDir), 0.0);
    vec3 specular = mat.specular * pow(max(dot(eyeDir, reflectDir), 0.0), mat.shine);

    float d = length(light.pos - FragPos);
    float fade = 1.0 / (light.constant + d*light.linear + d*d*light.quadratic);

    vec3 res = (ambient + diffuse + specular) * light.color * fade;
    return res;
}

#define N 5
void main() {
    vec3 normal = normalize(Normal);
    vec3 eyeDir = normalize(E - FragPos);

    vec3 lights[N];
    lights[0] = vec3(25.0, 14.00, -22.5);
    lights[1] = vec3(-16.0, 14.00, -22.5);

    lights[2] = vec3(11.73, 15, 14.24);
    lights[3] = vec3(-14.55, 15, 14.17);

    lights[4] = vec3(2.97, 22, -20.96);

    vec3 diffuseColor = vec3(texture(DiffuseMap0, TexCoord));

    PointLight l;
    l.color = vec3(1, 0.77, 0.56);
    l.constant = 1.0;
    l.linear = 0.07;
    l.quadratic = 0.017;

    vec3 res = vec3(0.0);
    for (int i = 0; i < N; i++) {
        l.pos = lights[i];
        res += pointLight(l, normal, eyeDir, diffuseColor);
    }

    color = vec4(res, 1.0);
}

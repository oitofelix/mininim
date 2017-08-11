#ifdef GL_ES
precision lowp float;
#endif

uniform sampler2D al_tex;
uniform bool al_use_tex;
varying vec4 varying_color;
varying vec2 varying_texcoord;

#define MAX 64
uniform vec4 palette_table_in[MAX];
uniform vec4 palette_table_out[MAX];

void
main (void)
{
  vec4 color;

  if (al_use_tex)
    color = varying_color * texture2D (al_tex, varying_texcoord);
  else
    color = varying_color;

  for (int i = 0;
       i < MAX && palette_table_in[i] != vec4 (-1, -1, -1, -1); i++) {
    if (color == palette_table_in[i]) {
      gl_FragColor = palette_table_out[i];
      return;
    }
  }

  gl_FragColor = color;
}
